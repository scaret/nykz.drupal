#include "www.h"

int bbssendmsg_main()
{
    int i;
    int mode, destpid = 0;
    int is_his_friend = 0;
    char destid[20], msg[256];
    struct userec *user;

    init_all();
    if (!loginok)
        http_quit_print("�Ҵҹ��Ͳ��ܷ�ѶϢ, ���ȵ�¼��");
    if (!HAS_PERM(PERM_LOGINOK))
        http_quit_print("δע���û����ܷ�ѶϢ");
    if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_PAGE))
        http_quit_print("�������������Ȩ��");
    strsncpy(destid, getparm("destid"), 13);
    strsncpy(msg, getparm("msg"), 51);
    destpid = atoi(getparm("destpid"));
    if (destid[0] == 0 || msg[0] == 0)
    {
        char buf3[256];

        strcpy(buf3, "<body onload='document.form0.msg.focus()'>");
        if (destid[0] == 0)
            strcpy(buf3, "<body onload='document.form0.destid.focus()'>");
        printf("%s\n", buf3);
        printf
            ("\n               <form name=form0 action=bbssendmsg method=post>\n               <input type=hidden name=destpid value=%d>\n               ��ѶϢ��: <input name=destid maxlength=12 value='%s' size=12><br>\n               ѶϢ����: <input name=msg maxlength=25 size=50 value='%s' onkeypress='if(event.keyCode==10) event.keyCode=13'>\n               (ENTER �� CTRL+ENTER����) <br>\n               <input type=submit value=ȷ�� width=6>\n               </form>\n               ",
             destpid, destid, msg);
        http_quit();
    }
    user = getuser(destid);
    if (user == 0)
        http_quit_print("���޴���");
    loadfriend(user->userid);
    is_his_friend = isfriend(currentuser.userid);
    if (isbad(user->userid, currentuser.userid)
        || isbad(currentuser.userid, user->userid))
    {
        http_quit_print("���޷�����˷���ѶϢ");
    }
    printf("<body onload='document.form1.b1.focus()'>\n");
    for (i = 0; i < MAXACTIVE; i++)
        if (shm_utmp->uinfo[i].active)
            if (!strcasecmp(shm_utmp->uinfo[i].userid, destid))
            {
                if (destpid != 0 && shm_utmp->uinfo[i].pid != destpid)
                    continue;
                destpid = shm_utmp->uinfo[i].pid;
                if (!(shm_utmp->uinfo[i].pager & ALLMSG_PAGER)
                    && (is_his_friend == 0))
                    continue;
                if (!
                    (shm_utmp->uinfo[i].
                     pager & FRIENDMSG_PAGER) && is_his_friend)
                    continue;
                if (shm_utmp->uinfo[i].invisible
                    && !(currentuser.userlevel & PERM_SEECLOAK))
                    continue;
                mode = shm_utmp->uinfo[i].mode;

                if (mode == BBSNET || mode == PAGE || mode == LOCKSCREEN)
                    continue;
                if (!strcasecmp(destid, currentuser.userid))
                    printf("�㲻�ܸ��Լ���ѶϢ��");
                else
                {
                    if (send_msg
                        (currentuser.userid, u_info->pid,
                         user->userid, destpid, msg,
                         is_web_user(&(shm_utmp->uinfo[i]))) == 0)

                        printf("�Ѿ������ͳ���Ϣ");
                    else
                        printf("������Ϣʧ��");
                }
                printf
                    ("<script>if(top.refresh_msg) top.refresh_msg(); else top.fmsg.location=top.fmsg.location;</script>\n");
                printf
                    ("<br><form name=form1><input name=b1 type=button onkeypress='if(event.keyCode==10) event.keyCode=13' onclick='history.go(-2)' value='[����]'>");
                printf("</form>");
                http_quit();
            }
    printf("����Ŀǰ�����߻����޷�������Ϣ");
    printf
        ("<br><form name=form1><input name=b1 type=button onclick='history.go(-2)' value='[����]'>");
    printf("</form>");
    printf("</html>");
    return 0;
}
