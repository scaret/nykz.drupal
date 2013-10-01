/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "bbs.h"

extern time_t login_start_time;
extern char fromhost[60];
char *genpasswd(char *pw);
char *sysconf_str(char *key);

void disply_userinfo(struct userec *u)
{
    int num;

#ifdef REG_EXPIRED

    time_t now;

#endif

    move(2, 0);
    clrtobot();
    now = time(0);
    set_safe_record();
    prints("���Ĵ���     : %-14s", u->userid);
    prints("�ǳ� : %-20s", u->username);
    prints("     �Ա� : %s", (u->gender == 'M' ? "��" : "Ů"));
    prints("\n��ʵ����     : %-40s", u->realname);
    prints("  �������� : %d/%d/%d",
           u->birthmonth, u->birthday, u->birthyear + 1900);
    prints("\n��ססַ     : %-38s", u->address);
    {
        int tyear, tmonth, tday;

        tyear = u->birthyear + 1900;
        tmonth = u->birthmonth;
        tday = u->birthday;
        countdays(&tyear, &tmonth, &tday, now);
        prints("�ۼ��������� : %d\n", abs(tyear));
    }
    prints("�����ʼ����� : %s\n", u->email);
    prints("��ʵ E-mail  : %s\n", u->reginfo);
    if HAS_PERM
        (PERM_ADMINMENU) prints("Ident ����   : %s\n", u->ident);
    prints("������ٻ��� : %-22s", u->lasthost);
    prints("�ն˻���̬ : %s\n", u->termtype);
    getdatestring(u->firstlogin, NA);
    prints("�ʺŽ������� : %s[��� %d ��]\n",
           datestring, (now - (u->firstlogin)) / 86400);
    getdatestring(u->lastlogin, NA);
    prints("����������� : %s[��� %d ��]\n",
           datestring, (now - (u->lastlogin)) / 86400);
#ifndef AUTOGETPERM
#ifndef REG_EXPIRED

    getdatestring(u->lastjustify, NA);
    prints("���ȷ������ : %s\n",
           (u->lastjustify == 0) ? "δ��ע��" : datestring);
#else

    if (u->lastjustify == 0)
        prints("���ȷ��     : δ��ע��\n");
    else
    {

        prints("���ȷ��     : ����ɡ�\n");

    }
#endif
#endif
    prints("������Ŀ     : %d\n", u->numposts);
    prints("˽������     : %d ��\n", u->nummails);
    prints("��վ����     : %d ��      ", u->numlogins);
    prints("��վ��ʱ��   : %d �� %d Сʱ %d ����\n",
           u->stay / 86400, (u->stay / 3600) % 24, (u->stay / 60) % 60);
    strcpy(genbuf, "bTCPRD#@XWBA#VS-DOM-F0s23456789t");
    for (num = 0; num < (int) strlen(genbuf); num++)
        if (!(u->userlevel & (1 << num)))
            genbuf[num] = '-';
    prints("ʹ����Ȩ��   : %s\n", genbuf);
    prints("\n");
    if (u->userlevel & PERM_SYSOP)
    {
        prints("  ���Ǳ�վ��վ��, ��л���������Ͷ�.\n");
    }
    else if (u->userlevel & PERM_BOARDS)
    {
        prints("  ���Ǳ�վ�İ���, ��л���ĸ���.\n");
    }
    else if (u->userlevel & PERM_LOGINOK)
    {
        prints("  ����ע������Ѿ����, ��ӭ���뱾վ.\n");
    }
    else if (u->lastlogin - u->firstlogin < 3 * 86400)
    {
        prints("  ������·, ���Ķ� Announce ������.\n");
    }
    else
    {
        prints("  ע����δ�ɹ�, ��ο���վ��վ����˵��.\n");
    }
}

void uinfo_change1(int i, struct userec *u, struct userec *newinfo)
{
    char buf[STRLEN], genbuf[128];
    int age;
    time_t now;

    sprintf(genbuf, "���ߴ��� [%d]: ", u->numlogins);
    getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
    if (buf[0] != '\0' && atoi(buf) >= 0)
        newinfo->numlogins = atoi(buf);

    sprintf(genbuf, "���������� [%d]: ", u->numposts);
    getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
    if (buf[0] != '\0' && atoi(buf) >= 0)
        newinfo->numposts = atoi(buf);

    now = time(0);
    age = (now - (u->firstlogin)) / 86400;
    sprintf(genbuf, "���� [%d]: ", age);
    getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
    if (buf[0] != '\0' && atoi(buf) >= 0)
        newinfo->firstlogin = now - atoi(buf) * 86400;

    sprintf(genbuf, "��վ��ʱ��[%ld��]: ", u->stay / 86400);
    getdata(i++, 0, genbuf, buf, 10, DOECHO, YEA);
    if (buf[0] != '\0' && atoi(buf) >= 0)
        newinfo->stay = atoi(buf) * 86400;
}

void check_uinfo(struct userec *u, int MUST)
{
    int changeIT = 0, changed = 0, pos = 2;
    char ans[5];

    while (1)
    {
        changeIT = MUST || (strlen(u->username) < 2)
            || (strstr(u->username, "  ")) || (strstr(u->username, "��"));
        if (!changeIT)
        {
            if (changed)
            {
                pos++;
                changed = 0;
            }
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }

        move(pos++, 0);
        prints
            ("�����������ǳ� (Enter nickname) [1;31m��ȡ�����س�![m ��");
        getdata(pos, 0, "> ", u->username, NAMELEN, DOECHO, YEA);
        strcpy(uinfo.username, u->username);
        update_utmp();
    }
    while (1)
    {
        changeIT = MUST || (strlen(u->realname) < 4)
            || (strstr(u->realname, "  ")) || (strstr(u->realname, "��"));
        if (!changeIT)
        {
            if (changed)
            {
                pos += 2;
                changed = 0;
            }
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }
        move(pos, 0);
        prints
            ("������������ʵ���� (Enter realname) [1;31m��ʹ������[m��\n");
        getdata(pos + 1, 0, "> ", u->realname, NAMELEN, DOECHO, YEA);
    }
    while (1)
    {
        changeIT = MUST || (strlen(u->address) < 10)
            || (strstr(u->address, "  ")) || (strstr(u->address, "��"));
        if (!changeIT)
        {
            if (changed)
            {
                pos += 2;
                changed = 0;
            }
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }
        move(pos, 0);
        prints
            ("����������Ŀǰסַ (Enter home address) [1;31m�������һ����ƺ���[m ��\n");
        getdata(pos + 1, 0, "> ", u->address, STRLEN - 20, DOECHO, YEA);
    }
    while (1)
    {
        changeIT = MUST || (strchr(u->email, '@') == NULL);
        if (!changeIT)
        {
#ifdef MAILCHECK
            if (changed)
            {
                pos += 4;
                changed = 0;
            }
#else
            if (changed)
            {
                pos += 3;
                changed = 0;
            }
#endif
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }
        move(pos, 0);
        prints("���������ʽΪ: [1;37muserid@your.domain.name[m\n");
#ifdef MAILCHECK

        prints
            ("[32m��վ�Ѿ��ṩ[33m�ʼ�ע��[32m����, ������ͨ���ʼ����ٵ�ͨ��ע����֤.[m\n");
#endif

        prints("������������� (�����ṩ�߰� <Enter>)");
#ifdef MAILCHECK

        getdata(pos + 3, 0, "> ", u->email, 48, DOECHO, YEA);
#else

        getdata(pos + 2, 0, "> ", u->email, 48, DOECHO, YEA);
#endif

        if (strchr(u->email, '@') == NULL)
        {
            sprintf(genbuf, "%s.bbs@%s", u->userid, BBSHOST);
            strncpy(u->email, genbuf, 48);
        }
    }
    {
        changeIT = MUST || (strchr("MF", u->gender) == NULL);
        if (changeIT)
        {
            getdata(pos, 0, "����������Ա�: M.�� F.Ů [M]: ", ans, 2,
                    DOECHO, YEA);
            if ((ans[0] != 'F' && ans[0] != 'f') || ans[0] == 'm')
                u->gender = 'M';
            else
                u->gender = 'F';
            pos++;
        }
    }
    while (1)
    {
        changeIT = MUST || (u->birthyear < 20) || (u->birthyear > 98);
        if (!changeIT)
        {
            if (changed)
            {
                pos++;
                changed = 0;
            }
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }
        getdata(pos, 0, "�����������������(��λ��): ", ans, 5, DOECHO,
                YEA);
        if (atoi(ans) < 1920 || atoi(ans) > 1998)
        {
            MUST = 1;
            continue;
        }
        u->birthyear = atoi(ans) - 1900;
    }
    while (1)
    {
        changeIT = MUST || (u->birthmonth < 1) || (u->birthmonth > 12);
        if (!changeIT)
        {
            if (changed)
            {
                pos++;
                changed = 0;
            }
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }
        getdata(pos, 0, "���������������·�: ", ans, 3, DOECHO, YEA);
        u->birthmonth = atoi(ans);
    }
    while (1)
    {
        changeIT = MUST || (u->birthday < 1) || (u->birthday > 31)
            ||
            (((u->birthmonth < 8 && !(u->birthmonth % 2))
              || (u->birthmonth > 7 && (u->birthmonth % 2)))
             && u->birthday > 30)
            || (u->birthmonth == 2 && u->birthday > 29);
        if (!changeIT)
        {
            if (changed)
            {
                pos++;
                changed = 0;
            }
            break;
        }
        else
        {
            MUST = 0;
            changed = 1;
        }
        getdata(pos, 0, "���������ĳ�����: ", ans, 3, DOECHO, YEA);
        u->birthday = atoi(ans);
    }
}

int uinfo_query(struct userec *u, int real, int unum)
{
    struct userec newinfo;
    char ans[3], buf[STRLEN], genbuf[128];
    char src[STRLEN], dst[STRLEN];
    int i, fail = 0;

#ifdef MAILCHANGED

    int netty_check = 0;
#endif

    time_t now;
    struct tm *tmnow;

    memcpy(&newinfo, u, sizeof(currentuser));
    getdata(t_lines - 1, 0, real ?
            "��ѡ�� (0)���� (1)�޸����� (2)�趨���� (3) �� ID ==> [0]" :
            "��ѡ�� (0)���� (1)�޸����� (2)�趨���� (3) ѡǩ���� ==> [0]",
            ans, 2, DOECHO, YEA);
    clear();
    refresh();
    now = time(0);
    tmnow = localtime(&now);

    i = 3;
    move(i++, 0);
    if (ans[0] != '3' || real)
        prints("ʹ���ߴ���: %s\n", u->userid);
    switch (ans[0])
    {
    case '1':
        move(1, 0);
        prints("�������޸�,ֱ�Ӱ� <ENTER> ����ʹ�� [] �ڵ����ϡ�\n");
        sprintf(genbuf, "�ǳ� [%s]: ", u->username);
        getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
        if (buf[0])
            strncpy(newinfo.username, buf, NAMELEN);

        sprintf(genbuf, "��ʵ���� [%s]: ", u->realname);
        getdata(i++, 0, genbuf, buf, NAMELEN, DOECHO, YEA);
        if (buf[0])
            strncpy(newinfo.realname, buf, NAMELEN);

        sprintf(genbuf, "��ס��ַ [%s]: ", u->address);
        getdata(i++, 0, genbuf, buf, STRLEN - 10, DOECHO, YEA);
        if (buf[0])
            strncpy(newinfo.address, buf, NAMELEN);

        sprintf(genbuf, "�������� [%s]: ", u->email);
        getdata(i++, 0, genbuf, buf, 48, DOECHO, YEA);
        if (buf[0])
        {
#ifdef MAILCHECK
#ifdef MAILCHANGED
            if (u->uid == usernum)
                netty_check = 1;
#endif
#endif

            strncpy(newinfo.email, buf, 48);
        }

        sprintf(genbuf, "�ն˻���̬ [%s]: ", u->termtype);
        getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
        if (buf[0])
            strncpy(newinfo.termtype, buf, 16);

        sprintf(genbuf, "������ [%d]: ", u->birthyear + 1900);
        getdata(i++, 0, genbuf, buf, 5, DOECHO, YEA);
        if (buf[0] && atoi(buf) > 1920 && atoi(buf) < 1998)
            newinfo.birthyear = atoi(buf) - 1900;

        sprintf(genbuf, "������ [%d]: ", u->birthmonth);
        getdata(i++, 0, genbuf, buf, 3, DOECHO, YEA);
        if (buf[0] && atoi(buf) >= 1 && atoi(buf) <= 12)
            newinfo.birthmonth = atoi(buf);

        sprintf(genbuf, "������ [%d]: ", u->birthday);
        getdata(i++, 0, genbuf, buf, 3, DOECHO, YEA);
        if (buf[0] && atoi(buf) >= 1 && atoi(buf) <= 31)
            newinfo.birthday = atoi(buf);

        sprintf(genbuf, "�Ա�(M.��)(F.Ů) [%c]: ", u->gender);
        getdata(i++, 0, genbuf, buf, 2, DOECHO, YEA);
        if (buf[0])
        {
            if (strchr("MmFf", buf[0]))
                newinfo.gender = toupper(buf[0]);
        }

        if (real)
            uinfo_change1(i, u, &newinfo);
        break;
    case '2':
        if (!real)
        {
            getdata(i++, 0, "������ԭ����: ", buf, 9, NOECHO, YEA);

            if (*buf == '\0' || !checkpasswd(u->passwd, buf))
            {
                prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
                fail++;
                break;
            }
        }
        getdata(i++, 0, "���趨������: ", buf, 9, NOECHO, YEA);
        if (buf[0] == '\0')
        {
            prints("\n\n�����趨ȡ��, ����ʹ�þ�����\n");
            fail++;
            break;
        }
        strncpy(genbuf, buf, PASSLEN);
        getdata(i++, 0, "����������������: ", buf, 9, NOECHO, YEA);
        if (strncmp(buf, genbuf, PASSLEN))
        {
            prints("\n\n������ȷ��ʧ��, �޷��趨�����롣\n");
            fail++;
            break;
        }
        buf[8] = '\0';
        strncpy(newinfo.passwd, genpasswd(buf), ENCPASSLEN);
        break;
    case '3':
        if (!real)
        {
            sprintf(genbuf, "Ŀǰʹ��ǩ���� [%d]: ", u->signature);
            getdata(i++, 0, genbuf, buf, 16, DOECHO, YEA);
            if (atoi(buf) > 0)
                newinfo.signature = atoi(buf);
        }
        else
        {
            struct user_info uin;
            extern int t_cmpuids(int uid, struct user_info *up);

            if (t_search_ulist(&uin, t_cmpuids, unum, NA, NA) != 0)
            {
                prints("\n�Բ��𣬸��û�Ŀǰ�������ϡ�");
                fail++;
            }
            else if (!strcmp(lookupuser.userid, "SYSOP"))
            {
                prints("\n�Բ����㲻�����޸� SYSOP �� ID��");
                fail++;
            }
            else
            {
                getdata(i++, 0, "�µ�ʹ���ߴ���: ", genbuf, IDLEN + 1,
                        DOECHO, YEA);
                if (*genbuf != '\0')
                {
                    if (id_with_num(genbuf))
                    {
                        prints("�ʺű���ȫΪӢ����ĸ!\n");
                        fail++;
                    }
                    else if (getuser(genbuf))
                    {
                        prints("\n�Բ���! �Ѿ���ͬ�� ID ��ʹ����\n");
                        fail++;
                    }
                    else
                    {
                        strncpy(newinfo.userid, genbuf, IDLEN + 2);
                    }
                }
                else
                    fail++;
            }
        }
        break;
    default:
        clear();
        return 0;
    }
    if (fail != 0)
    {
        pressreturn();
        clear();
        return 0;
    }
    if (askyn("ȷ��Ҫ�ı���", NA, YEA) == YEA)
    {
        if (real)
        {
            char secu[STRLEN];

            sprintf(secu, "�޸� %s �Ļ������ϻ����롣", u->userid);
            securityreport(secu);
        }
        if (strcmp(u->userid, newinfo.userid))
        {
            sprintf(src, "mail/%c/%c/%s", toupper(u->userid[0]),
                    toupper(u->userid[1]), u->userid);
            sprintf(dst, "mail/%c/%c/%s", toupper(newinfo.userid[0]),
                    toupper(newinfo.userid[1]), newinfo.userid);
            rename(src, dst);
            sethomepath(src, u->userid);
            sethomepath(dst, newinfo.userid);
            rename(src, dst);
            sethomefile(src, u->userid, "register");
            unlink(src);
            sethomefile(src, u->userid, "register.old");
            unlink(src);
            setuserid(unum, newinfo.userid);
        }
        if (!strcmp(u->userid, currentuser.userid))
        {
            extern int WishNum;

            strncpy(uinfo.username, newinfo.username, NAMELEN);
            WishNum = 9999;
        }
#ifdef MAILCHECK
#ifdef MAILCHANGED
        if ((netty_check == 1) && !HAS_PERM(PERM_SYSOP))
        {
            sprintf(genbuf, "%s", BBSHOST);
            if ((!strstr(newinfo.email, genbuf)) &&
                (!invalidaddr(newinfo.email)) &&
                (!invalid_email(newinfo.email)))
            {
                strcpy(u->email, newinfo.email);
                send_regmail(u);
            }
            else
            {
                move(t_lines - 5, 0);
                prints("\n������ĵ����ʼ���ַ ��[1;33m%s[m��\n",
                       newinfo.email);
                prints
                    ("ˡ���ܱ�վ���ϣ�ϵͳ����Ͷ��ע���ţ������������...\n");
                pressanykey();
                return 0;
            }
        }
#endif
#endif
        memcpy(u, &newinfo, (size_t) sizeof(currentuser));
#ifdef MAILCHECK
#ifdef MAILCHANGED

        if ((netty_check == 1) && !HAS_PERM(PERM_SYSOP))
        {
            newinfo.userlevel &=
                ~(PERM_LOGINOK | PERM_PAGE | PERM_MESSAGE);
            sethomefile(src, newinfo.userid, "register");
            sethomefile(dst, newinfo.userid, "register.old");
            rename(src, dst);
        }
#endif
#endif
        substitute_record(PASSFILE, &newinfo, sizeof(newinfo), unum);
    }
    clear();
    return 0;
}

void x_info(void)
{
    if (!strcmp("guest", currentuser.userid))
        return;
    modify_user_mode(GMENU);
    disply_userinfo(&currentuser);
    uinfo_query(&currentuser, 0, usernum);
}

void getfield(int line, char *info, char *desc, char *buf, int len)
{
    char prompt[STRLEN];

    sprintf(genbuf, "  ԭ���趨: %-46.46s [1;32m(%s)[m",
            (buf[0] == '\0') ? "(δ�趨)" : buf, info);
    move(line, 0);
    prints("%s", genbuf);
    sprintf(prompt, "  %s: ", desc);
    getdata(line + 1, 0, prompt, genbuf, len, DOECHO, YEA);
    if (genbuf[0] != '\0')
        strncpy(buf, genbuf, len);
    move(line, 0);
    clrtoeol();
    prints("  %s: %s\n", desc, buf);
    clrtoeol();
}

int id_in_form(char *id)
{
    FILE *fp_src = NULL;
    struct regform form;

    fp_src = fopen("register_sjtubbs", "r");
    if (fp_src == NULL)
        return 0;
    while (fread(&form, sizeof(struct regform), 1, fp_src) > 0)
    {
        if (strncmp(form.userid, id, IDLEN) == 0)
        {
            fclose(fp_src);
            return 1;
        }
    }
    fclose(fp_src);
    return 0;
}

void x_fillform(void)
{
    char rname[NAMELEN], addr[STRLEN];
    char phone[STRLEN], dept[STRLEN], assoc[STRLEN];
    char ans[5], *mesg;
    FILE *fn;
    time_t now;

    if (!strcmp("guest", currentuser.userid))
        return;
    modify_user_mode(NEW);
    clear();
    move(2, 0);
    clrtobot();
    if (currentuser.userlevel & PERM_LOGINOK)
    {
        prints("���Ѿ���ɱ�վ��ʹ����ע������, ��ӭ���뱾վ������.");
        pressreturn();
        return;
    }
#ifdef PASSAFTERTHREEDAYS
    if (currentuser.lastlogin - currentuser.firstlogin < 3 * 86400)
    {
        prints("���״ε��뱾վδ������(72��Сʱ)...\n");
        prints("�����Ĵ���Ϥһ�£����������Ժ�����дע�ᵥ��");
        pressreturn();
        return;
    }
#endif

    if (id_in_form(currentuser.userid))
    {
        prints("վ����δ��������ע�����뵥, ���ȵ���������.");
        pressreturn();
        return;
    }
    strncpy(rname, currentuser.realname, NAMELEN);
    strncpy(addr, currentuser.address, STRLEN - 8);
    dept[0] = phone[0] = assoc[0] = '\0';
    while (1)
    {
        move(3, 0);
        clrtoeol();
        prints("%s ����, ���ʵ��д���µ�����:\n", currentuser.userid);
        getfield(6, "��������", "��ʵ����", rname, NAMELEN);
        getfield(8, "ѧУϵ����˾ְ��", "ѧУϵ��", dept, STRLEN);
        getfield(10, "�������һ����ƺ���", "Ŀǰסַ", addr, STRLEN);
        getfield(12, "����������ʱ��", "����绰", phone, STRLEN);
        mesg = "���������Ƿ���ȷ, �� Q ����ע�� (Y/N/Quit)? [Y]: ";
        getdata(t_lines - 1, 0, mesg, ans, 3, DOECHO, YEA);
        if (ans[0] == 'Q' || ans[0] == 'q')
            return;
        if (ans[0] != 'N' && ans[0] != 'n')
            break;
    }
    strncpy(currentuser.realname, rname, NAMELEN);
    strncpy(currentuser.address, addr, STRLEN - 8);
    if ((fn = fopen("register_sjtubbs", "a")) != NULL)
    {
        struct regform form;

        bzero(&form, sizeof(form));
        now = time(NULL);
        getdatestring(now, NA);
        form.usernum = usernum;
        strncpy(form.userid, currentuser.userid, IDLEN);
        strncpy(form.date, datestring, strlen(datestring));
        strncpy(form.realname, rname, NAMELEN - 1);
        strncpy(form.dept, dept, STRLEN - 1);
        strncpy(form.addr, addr, STRLEN - 1);
        strncpy(form.phone, phone, STRLEN - 1);
        form.status = 0;
        fwrite(&form, sizeof(struct regform), 1, fn);
        fclose(fn);
    }
    setuserfile(genbuf, "mailcheck");
    if ((fn = fopen(genbuf, "w")) == NULL)
        return;
    fprintf(fn, "usernum: %d\n", usernum);
    fclose(fn);
}
