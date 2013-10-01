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

#ifdef WITHOUT_ADMIN_TOOLS
#define kick_user
#endif

void mail_info(char *lastword);

void offline(void)
{
    int i;
    char buf[STRLEN], lastword[640];

    modify_user_mode(OFFLINE);
    clear();
    if (HAS_PERM(PERM_SYSOP) || HAS_PERM(PERM_BOARDS)
        || HAS_PERM(PERM_BLEVELS)
        || HAS_PERM(PERM_ADMINMENU) || HAS_PERM(PERM_SEEULEVELS)
        || HAS_PERM(PERM_XEMPT) || HAS_PERM(PERM_SPECIAL6)
        || HAS_PERM(PERM_SPECIAL7) || HAS_PERM(PERM_SPECIAL8))
    {
        move(1, 0);
        prints("\n\n������������, ���������ɱ��!!\n");
        pressreturn();
        clear();
        return;
    }
    if (currentuser.stay < 86400)
    {
        move(1, 0);
        prints("\n\n�Բ���, ����δ���ʸ�ִ�д�����!!\n");
        prints("�� mail �� SYSOP ˵����ɱԭ��, лл��\n");
        pressreturn();
        clear();
        return;
    }
    getdata(1, 0, "�������������: ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !checkpasswd(currentuser.passwd, buf))
    {
        prints("\n\n�ܱ�Ǹ, ����������벻��ȷ��\n");
        pressreturn();
        clear();
        return;
    }
    getdata(3, 0, "�������ʲ������? ", buf, NAMELEN, DOECHO, YEA);
    if (*buf == '\0' || strcmp(buf, currentuser.realname))
    {
        prints("\n\n�ܱ�Ǹ, �Ҳ�����ʶ�㡣\n");
        pressreturn();
        clear();
        return;
    }
    clear();
    move(1, 0);
    prints
        ("[1;5;31m����[0;1;31m�� ��ɱ��, �����޷����ô��ʺŽ��뱾վ����");
    move(3, 0);
    prints("[1;32m���ʺ�Ҫ�� 30 ����Ż�ɾ�������ѹ�� :( .....[m");
    move(5, 0);
    i = 0;
    if (askyn("�����᲻���㣬����֮ǰ��ʲô����˵ô", YEA, NA) == YEA)
    {
        strcpy(lastword, ">\n>");
        buf[0] = '\0';
        for (i = 0; i < 8; i++)
        {
            getdata(i + 6, 0, ": ", buf, 77, DOECHO, YEA);
            if (buf[0] == '\0')
                break;
            strcat(lastword, buf);
            strcat(lastword, "\n>");
        }
        if (i == 0)
            lastword[0] = '\0';
        else
            strcat(lastword, "\n\n");
        move(i + 8, 0);
        if (i == 0)
            prints("�����㻹��ʲô����Ը��˵���ǲ��ǻ�����˼δ�ˣ�");
        else if (i <= 4)
            prints("�������㲵��������Ķ����� ... ");
        else
            prints
                ("�һ�ǵ���ģ����ѣ���Ҳ֪������뿪Ҳ��û�а취���£�������");
    }
    move(i + 10, 0);
    if (askyn("��ȷ��Ҫ�뿪������ͥ", NA, NA) == 1)
    {
        clear();
        currentuser.userlevel = 0;
        substitute_record(PASSFILE, &currentuser, sizeof(struct userec),
                          usernum);
        mail_info(lastword);
        modify_user_mode(OFFLINE);
        kick_user(&uinfo);
        exit(0);
    }
}

void getuinfo(FILE * fn)
{
    int num;
    char buf[40];

    fprintf(fn, "\n\n���Ĵ���     : %s\n", currentuser.userid);
    fprintf(fn, "�����ǳ�     : %s\n", currentuser.username);
    fprintf(fn, "��ʵ����     : %s\n", currentuser.realname);
    fprintf(fn, "��ססַ     : %s\n", currentuser.address);
    fprintf(fn, "�����ʼ����� : %s\n", currentuser.email);
    fprintf(fn, "��ʵ E-mail  : %s\n", currentuser.reginfo);
    fprintf(fn, "Ident ����   : %s\n", currentuser.ident);
    getdatestring(currentuser.firstlogin, NA);
    fprintf(fn, "�ʺŽ������� : %s\n", datestring);
    getdatestring(currentuser.lastlogin, NA);
    fprintf(fn, "����������� : %s\n", datestring);
    fprintf(fn, "������ٻ��� : %s\n", currentuser.lasthost);
    fprintf(fn, "��վ����     : %d ��\n", currentuser.numlogins);
    fprintf(fn, "������Ŀ     : %d\n", currentuser.numposts);
    fprintf(fn, "��վ��ʱ��   : %ld Сʱ %ld ����\n",
            currentuser.stay / 3600, (currentuser.stay / 60) % 60);
    strcpy(buf, "bTCPRp#@XWBA#VS-DOM-F0123456789t");
    for (num = 0; num < 30; num++)
        if (!(currentuser.userlevel & (1 << num)))
            buf[num] = '-';
    buf[num] = '\0';
    fprintf(fn, "ʹ����Ȩ��   : %s\n\n", buf);
}

void mail_info(char *lastword)
{
    FILE *fn;
    time_t now;
    char filename[STRLEN];

    now = time(0);
    getdatestring(now, NA);
    sprintf(filename, "tmp/suicide.%s", currentuser.userid);
    if ((fn = fopen(filename, "w")) != NULL)
    {
        fprintf(fn,
                "[1m%s[m �Ѿ��� [1m%s[m �Ǽ���ɱ�ˣ��������������ϣ��뱣��...",
                currentuser.userid, datestring);
        getuinfo(fn);
        fclose(fn);
        Postfile(filename, "syssecurity", "�Ǽ���ɱ֪ͨ(30������Ч)...",
                 2);
        unlink(filename);
    }
    if ((fn = fopen(filename, "w")) != NULL)
    {
        fprintf(fn, "��Һ�,\n\n");
        fprintf(fn, "���� %s (%s)���Ҽ��������� 30 ����뿪�����ˡ�\n\n",
                currentuser.userid, currentuser.username);
        getdatestring(currentuser.firstlogin, NA);
        fprintf(fn,
                "�� %14.14s �������Ѿ����� %d ���ˣ������ܼ� %ld ���ӵ����������У�\n",
                datestring, currentuser.numlogins, currentuser.stay / 60);
        fprintf(fn,
                "������λ����������أ������ҵ�����...  ���εΣ������������У�\n\n");
        fprintf(fn, "%s", lastword);
        fprintf(fn,
                "�����ǣ���� %s �����ǵĺ����������õ��ɡ���Ϊ�Ҽ��������뿪������!\n\n",
                currentuser.userid);
        fprintf(fn, "�����г�һ���һ�����ġ� ����!! �ټ�!!\n\n\n");
        getdatestring(now, NA);
        fprintf(fn, "%s �� %s ��.\n\n", currentuser.userid, datestring);
        fclose(fn);
        Postfile(filename, "message", "�����������...", 2);
        unlink(filename);
    }
}
