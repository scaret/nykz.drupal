#include <stdlib.h>
#include "nt.h"

extern int ntp_bbslogin(void);
extern int ntp_bbslogout(void);

static gboolean www_login_user(struct userec *user)
{
    char buf[256];
    struct user_info *u;
    int n;

    n = nt_new_uinfo(user->userid, &u);
    if (n < 0)
        return FALSE;

    u->mode = 10001;
    if (HAS_PERM(user, PERM_LOGINCLOAK) && (user->flags[0] & CLOAK_FLAG))
        u->invisible = YEA;
    u->pager = 0;
    if (user->userdefine & DEF_FRIENDCALL)
        u->pager |= FRIEND_PAGER;
    if (user->flags[0] & PAGER_FLAG)
    {
        u->pager |= ALL_PAGER;
        u->pager |= FRIEND_PAGER;
    }
    if (user->userdefine & DEF_FRIENDMSG)
        u->pager |= FRIENDMSG_PAGER;
    if (user->userdefine & DEF_ALLMSG)
    {
        u->pager |= ALLMSG_PAGER;
        u->pager |= FRIENDMSG_PAGER;
    }
    if (!HAS_PERM(user, PERM_LOGINOK))
    {

        nt_sethomefile(buf, sizeof(buf), user->userid, "register");
        if (g_file_test(buf, G_FILE_TEST_IS_REGULAR))
        {
            SET_PERM(user, PERM_DEFAULT);
            nt_set_userec_with_uid(user, u->uid);
        }
    }
    g_strlcpy(u->from, cgiRemoteAddr, 60);
    u->logintime = ntw_now;
    u->idle_time = ntw_now;
    g_strlcpy(u->username, user->username, 20);
    u->utmpkey = rand();
    cgiHeaderCookieSetInteger(COOKIE_UTMPNUM, n, 0, "/", NULL);
    cgiHeaderCookieSetInteger(COOKIE_UTMPKEY, u->utmpkey, 0, "/", NULL);
    cgiHeaderCookieSetString(COOKIE_UTMPUSERID, u->userid, 0, "/", NULL);
    g_message("user %s@%s login from www", u->userid, cgiRemoteAddr);
    return TRUE;
}

static void www_check_multi(struct userec *puser)
{

    struct user_info *puinfos[USER_LOGIN_LIMIT_NUM];
    int total;

    if (HAS_PERM_SYSOP(puser))
        return;

    total =
        nt_get_active_uinfo(puser->userid, puinfos, USER_LOGIN_LIMIT_NUM);
    if (total == USER_LOGIN_LIMIT_NUM)
    {
        ntw_logout_user(puinfos[0]);
    }
    return;

}

static gboolean www_check_multi_guest(char *fromhost)
{
    int i, n = 0;

    for (i = 0; i < MAXACTIVE; i++)
    {
        if (shm_utmp->uinfo[i].active == 0)
            continue;
        if (!strcasecmp("guest", shm_utmp->uinfo[i].userid)
            && !strcmp(shm_utmp->uinfo[i].from, fromhost))
        {
            ++n;
            if (n > 100)
                return FALSE;
        }
    }
    return TRUE;
}

/* denytype: 0: deny post, 1: deny base */
int nt_updateqzdenyuser(int district, char userid[IDLEN], int denytype)
{
    FILE *fp;
    int i;
    char path[80], *exp, buf[256];
    struct deny denyuser[256];
    int denynum = 0;

    sprintf(path, "boards/deny_users_%s_%d", denytype == 0 ? "" : "base", district);
    fp = fopen(path, "r");
    if (fp == 0)
        return 0;
    while (fgets(buf, 80, fp) != NULL)
    {
        sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
               denyuser[denynum].exp, denyuser[denynum].autoundeny,
               denyuser[denynum].free_time);
        denynum++;
    }
    fclose(fp);

    fp = fopen(path, "w");
    if (fp == 0)
        return 0;
    for (i = 0; i < denynum; i++)
    {
        int m;

        exp = denyuser[i].exp;
        if (denyuser[i].id[0] == 0)
            continue;
        for (m = 0; exp[m]; m++)
        {
            if (exp[m] <= 32 && exp[m] > 0)
                exp[m] = '.';
        }
        if (strncmp(denyuser[i].id, userid, IDLEN) != 0)
        {
            fprintf(fp, "%-12s %-40s %s %16s\n", denyuser[i].id,
                    denyuser[i].exp, denyuser[i].autoundeny,
                    denyuser[i].free_time);

        }
    }
    fclose(fp);
    return 0;
}

//denytype = 0: deny post; denytype = 1: deny base
int nt_seek_in_qzdenyfile(struct userec *user, int denytype)
{
    char filename[STRLEN];
    char filename2[30];
    FILE *fp;
    char buf[STRLEN];
    char *namep, *date;
	char currtime[30];

    char flag;
    time_t nowtime;

    int i = 0;

    if(denytype == 0)
    {
        sprintf(filename2, "boards/deny_users_");
    }
	else
    {
        sprintf(filename2, "boards/deny_users_base_");
    }

    for (i = 0; i < 30; i++)
        filename[i] = filename2[i];
    for (i = 0; i < SECNUM + 1; i++)
    {
        if (i < 10)
        {
            filename[strlen(filename2)] = i + 48;
            filename[strlen(filename2) + 1] = '\0';
        }
        else
        {
            filename[strlen(filename2)] = i / 10 + 48;
            filename[strlen(filename2) + 1] = i % 10 + 48;
            filename[strlen(filename2) + 2] = '\0';
        }
        if ((fp = fopen(filename, "r")) == NULL)
        {
            continue;
        }
        while (fgets(buf, STRLEN, fp) != NULL)
        {
            namep = (char *) strtok(buf, ": \n\r\t");
            if (namep != NULL && nt_ci_strcmp(namep, user->userid) == 0)
            {
                nowtime = time(0);
				sprintf(currtime, "%s", nt_ctime_cn(nowtime));
                flag = buf[55];
                date = (char *) (buf + 58);
                if (flag == 'a' && (strncmp(currtime, date, 16) >= 0))
                {
                    FILE *fp;
                    char path[80], title[80];
                    time_t now = time(0);

                    nt_updateqzdenyuser(i, user->userid, denytype);

					if(denytype == 0)
					{
                        user->userlevel |= PERM_POST;
					}
					else
					{
                        user->userlevel |= PERM_CHAT;
                        user->userlevel |= PERM_POST;
                        user->userlevel |= PERM_PAGE;
                        user->userlevel |= PERM_MESSAGE;
					}
                    nt_set_userec(user);
                    sprintf(path, "tmp/%d.tmp", getpid());
                    fp = fopen(path, "w");
					if(denytype == 0) //deny post
                    {
                        sprintf(title, "[����] �ָ�%s��ȫվ��POSTȨ",
                            user->userid);
						fprintf(fp, "�����������Զ�����ϵͳ�Զ�������\n\n");
						fprintf(fp,
								"\n  %s ���ѣ�\n\n\t����ʱ���ѹ����ָֻ�����ȫվ�ġ��������¡�Ȩ����\n\n"
								"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy SJTUBBS",
								user->userid);
                    }
					else //deny base
                    {
                        sprintf(title, "[����] �ָ�%s��ȫվ�Ļ���Ȩ��",
                            user->userid);
						fprintf(fp, "�����������Զ�����ϵͳ�Զ�������\n\n");
						fprintf(fp,
								"\n  %s ���ѣ�\n\n\t����ʱ���ѹ����ָֻ�����ȫվ�Ļ���Ȩ�ޡ�\n\n"
								"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy SJTUBBS",
								user->userid);
                    }
                    fclose(fp);
#ifdef DENY_POST_BOARDNAME
                    nt_post_article(DENY_POST_BOARDNAME, title, path,
                                 "SJTUBBS", "�Զ�����ϵͳ", "�Զ�����ϵͳ",
                                 -1, 0, now, now);
#endif
                    nt_post_article("syssecurity", title, path, "SJTUBBS",
                                 "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1, 0,
                                 now, now);

                    nt_post_mail(user->userid, title, path, user->userid,
                              user->username, 0, "�Զ�����ϵͳ", -1);
                    unlink(path);
                }
                break;
            }
        }
        fclose(fp);
    }

    return 0;
}

int ntp_bbslogin(void)
{
    int t;
    char buf[256], id[20], pw[20];
    struct userec user;
    char *errmsg;

    nt_inc_login_count();

    cgiFormString("id", id, 20);
    cgiFormString("pw", pw, 20);

    if (ntw_haslogin)
    {
        if (strcasecmp(id, ntw_curuser.userid))
        {
            /*ע���Ѿ���¼���û� */
            ntw_logout_user(ntw_puinfo);
        }
        else
        {
            /*��ͬ�û��Ѿ��ڱ�����¼ */
            cgiHeaderLocation("/frame2.html");
            return 0;
        }
    }

    if (!nt_get_userec(id, &user))
    {
        errmsg = "�޴��û�";
        goto ERROR;
    }

    if (strcasecmp(id, "guest"))
    {
        if (!nt_check_passwd(user.passwd, pw))
        {
            char path2[80];

            if (pw[0] != '\0')
            {

                sprintf(buf, "%s %s %s WWW\n",
                        nt_ctime(ntw_now) + 4, id, cgiRemoteAddr);
                nt_file_append("badlogin.www", buf);
                nt_sethomefile(path2, 80, user.userid, "logins.bad");
                nt_file_append(path2, buf);
                nt_sleep(2);
            }
            errmsg = "�������";
            goto ERROR;
        }

        if (!HAS_PERM_LOGIN(&user))
        {
            errmsg =
                "���ʺ��ѱ�ͣ��, ��������, ���������ʺ���sysop��ѯ��.";
            goto ERROR;
        }

        if (nt_is_bad_host(cgiRemoteAddr))
        {
            errmsg = "�Բ���, ��վ����ӭ���Դ�IP�ĵ�¼";
            goto ERROR;
        }

        t = user.lastlogin;
        user.lastlogin = ntw_now;

        if (abs(t - user.lastlogin) < 5)
        {
            errmsg = "���ε�¼�������!";
            goto ERROR;
        }

        if (!nt_check_login_limit(&user))
        {
            errmsg = "��¼�������࣬����ʹ����վ��";
            goto ERROR;
        }
        www_check_multi(&user);
        if (ntw_haslogin)
            ntw_logout_user(ntw_puinfo);
        user.numlogins++;
        g_strlcpy(user.lasthost, cgiRemoteAddr, 60);
        if (user.passwd[0] < ' ' || user.passwd[0] > 'z'
            || user.passwd[1] < ' ' || user.passwd[1] > 'z')
        {
            g_strlcpy(user.passwd, nt_crypt_passwd(pw), 14);
        }
        nt_set_userec(&user);
    }
    else
    {
        if (!www_check_multi_guest(cgiRemoteAddr))
        {
            errmsg = "web�����û���Ŀ������,��ʹ��ע��id��½";
            goto ERROR;
        }
        user.lastlogin = ntw_now;
        user.numlogins++;
        g_strlcpy(user.lasthost, cgiRemoteAddr, 60);
        nt_set_userec(&user);
    }

    if (!www_login_user(&user))
    {
        errmsg = "�Ѿ��ﵽϵͳ��¼���ޣ�����ϵͳ��æ�����Ժ�����";
        goto ERROR;
    }

    nt_seek_in_qzdenyfile(&user, 0);
    nt_seek_in_qzdenyfile(&user, 1);
    if (HAS_PERM_REALNAME(&user))
        cgiHeaderLocation("/frame2.html");
    else
        cgiHeaderLocation("/bbsrlmail");

    return 0;

  ERROR:
    ntw_normal_headers();
    tpl_print_err(errmsg);
    return 0;
}

int ntp_bbslogout(void)
{

    if (ntw_haslogin)
    {
        ntw_logout_user(ntw_puinfo);
        cgiHeaderCookieSetString(COOKIE_UTMPKEY, "", -1000000000, "/",
                                 NULL);
        cgiHeaderCookieSetString(COOKIE_UTMPNUM, "", -1000000000, "/",
                                 NULL);
        cgiHeaderCookieSetString(COOKIE_UTMPUSERID, "", -1000000000, "/",
                                 NULL);
    }
    cgiHeaderLocation("/");
    return 0;
}
