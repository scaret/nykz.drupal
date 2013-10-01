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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define ASCFILEPATH "0Announce/groups/GROUP_0/JuhuiAdmin/asc/"

void SelectMOde(char *ch);
void Asc_show();
void Asc_more(int i);
void GetFid1();
void GetFid2();
int WaitInput(long usec);
void Show_sleep(long usec);

void lottery()
{
    char ch[3];

    modify_user_mode(SYSINFO);
    SelectMOde(ch);
    if (ch[0] != '1' && ch[0] != '2')
        return;
    Asc_show();
    if (ch[0] == '1')
        GetFid1();
    else
        GetFid2();
    while (ch[0] != 'q')
        read(0, ch, 1);
}

void SelectMOde(char *ch)
{
    clear();
    refresh();
    getdata(0, 0, "����ѡ��齱ģʽ (1)�ֳ� (2)���� (���ո����):", ch, 2,
            DOECHO, YEA);
}

void Asc_show()
{
    int i, n;
    char buf;

    alarm(0);

    for (n = 15; n > 0; n--)
    {
        Asc_more((40 - n) % 25);
        Show_sleep(80000 + n * 8000);
    }

    while (buf != ' ')
    {
        Asc_more(rand() % 25);
        n++;
        i = WaitInput(80000);
        if (i == 0)
            continue;
        if (i == -1)
            break;
        read(0, &buf, 1);
    }

    for (n = 20; n > 0; n--)
    {
        Asc_more(rand() % 25);
        Show_sleep(200000 - n * 6000);

    }
}

void Asc_more(int i)
{
    char file[80];

    sprintf(file, "%s%d", ASCFILEPATH, i);
    ansimore(file, NA);
}

void GetFid1()
{
    int size, sum, num;
    char filename[STRLEN];
    struct stat st;
    struct override fboy;
    FILE *fp;

    size = sizeof(struct override);
    setuserfile(filename, "friends");
    if (stat(filename, &st) == -1)
        return;
    sum = st.st_size / size;

    if (sum == 0)
    {
        clear();
        move(1, 24);
        prints("[1;31m��ף��ˮ˼ԴBBS����������![m");
        move(9, 24);
        prints("[1;41m����, �ֳ�һ���˶�û��![m");
        move(10, 38);
        prints("���Ȱ��ֳ� ID �����������");
        move(20, 50);
        prints("�� q ���˳�");
        refresh();
        return;
    }

    num = rand() % sum;

    fp = fopen(filename, "r");
    fseek(fp, num * size, SEEK_SET);
    fread(&fboy, size, 1, fp);
    fclose(fp);

    clear();
    move(1, 24);
    prints("[1;31m��ף��ˮ˼ԴBBS����������![m");
    move(9, 24);
    prints("�����ֳ�����Ϊ: [1;41m%s[m", fboy.id);
    move(10, 38);
    prints("ף��ta!");
    move(20, 50);
    prints("�� q ���˳�");
    refresh();
}

void GetFid2()
{
    int num;
    char buf[STRLEN];

    num = rand() % count_users;
    fill_userlist();
    getoluserid(buf, num);
    while (strcmp(buf, "guest") == 0)
        getoluserid(buf, num);

    clear();
    move(1, 24);
    prints("[1;31m��ף��ˮ˼ԴBBS����������![m");
    move(9, 24);

    prints("����BBS���߻���Ϊ: [1;41m%s[m", buf);

    move(10, 38);
    prints("ף��ta!");
    move(20, 50);
    prints("�� q ���˳�");
    refresh();
}

int WaitInput(long usec)
{
    fd_set rfds;
    struct timeval tv;
    char retval;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = usec;

    retval = select(1, &rfds, NULL, NULL, &tv);

    return retval;
}

void Asc_alarm(long usec)
{
    struct itimerval ascitimerval;

    ascitimerval.it_interval.tv_sec = 0;
    ascitimerval.it_interval.tv_usec = 0;
    ascitimerval.it_value.tv_sec = 0;
    ascitimerval.it_value.tv_usec = usec;
    setitimer(ITIMER_REAL, &ascitimerval, NULL);
}

static void sig_alrm(int signo)
{

}

void Show_sleep(long usec)
{
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        return;

    {
        Asc_alarm(usec);
        pause();
    }
    return;
}
