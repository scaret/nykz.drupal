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

#define BADLOGINFILE    "logins.bad"
#define VISITLOG	BBSHOME"/.visitlog"
int ERROR_READ_SYSTEM_FILE = NA;
int RMSG = YEA;
int msg_num = 0;
int count_friends = 0, count_users = 0;
int iscolor = 1;
int mailXX = 0;
char *getenv(const char *);
int friend_login_wall(struct user_info *pageinfo);
char *sysconf_str(char *key);
struct user_info *t_search(char *sid, int pid);

void c_recover(void);
void tlog_recover(void);
void refreshdate();
int listmode;
int numofsig = 0;
jmp_buf byebye;
extern struct BCACHE *brdshm;
extern struct UTMPFILE *utmpshm;
FILE *ufp;
int talkrequest = NA;

int enter_uflags;
time_t lastnote;

struct user_info uinfo;

#ifndef BBSD
char tty_name[20];
#endif
char fromhost[60];

char BoardName[STRLEN];
char ULIST[STRLEN];
int utmpent = -1;
time_t login_start_time;
int showansi = 1;
int started = 0;

char GoodWish[20][STRLEN - 3];
int WishNum = 0;
int orderWish = 0;
extern int enabledbchar;

#ifdef ALLOWSWITCHCODE
int convcode = 0;
extern void resolve_GbBig5Files();
#endif

void log_usies(char *mode, char *mesg)
{
    time_t now;
    char buf[256], *fmt;

    now = time(0);
    fmt = currentuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n";
    getdatestring(now, NA);
    sprintf(buf, fmt, datestring, mode, currentuser.userid, mesg);
    file_append("usies", buf);
}

void u_enter(void)
{
    enter_uflags = currentuser.flags[0];
    memset(&uinfo, 0, sizeof(uinfo));
    uinfo.active = YEA;
    uinfo.pid = getpid();

    if (!HAS_PERM(PERM_CLOAK))
        currentuser.flags[0] &= ~CLOAK_FLAG;
    if (HAS_PERM(PERM_LOGINCLOAK) && (currentuser.flags[0] & CLOAK_FLAG))
        uinfo.invisible = YEA;
    uinfo.mode = LOGIN;
    uinfo.pager = 0;
    uinfo.logintime = time(0);
#ifdef BBSD

    uinfo.idle_time = time(0);
#endif

    if (DEFINE(DEF_DELDBLCHAR))
        enabledbchar = 1;
    else
        enabledbchar = 0;
    if (DEFINE(DEF_FRIENDCALL))
    {
        uinfo.pager |= FRIEND_PAGER;
    }
    if (currentuser.flags[0] & PAGER_FLAG)
    {
        uinfo.pager |= ALL_PAGER;
        uinfo.pager |= FRIEND_PAGER;
    }
    if (DEFINE(DEF_FRIENDMSG))
    {
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    if (DEFINE(DEF_ALLMSG))
    {
        uinfo.pager |= ALLMSG_PAGER;
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    uinfo.uid = usernum;
    strncpy(uinfo.from, fromhost, 60);

#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

    strncpy(uinfo.tty, tty_name, 20);
#endif

    iscolor = (DEFINE(DEF_COLOR)) ? 1 : 0;
    strncpy(uinfo.userid, currentuser.userid, 20);
    strncpy(uinfo.realname, currentuser.realname, 20);
    strncpy(uinfo.username, currentuser.username, NAMELEN);
    getfriendstr();
    getrejectstr();
    if (HAS_PERM(PERM_EXT_IDLE))
        uinfo.ext_idle = YEA;

    listmode = 0;
    while (1)
    {
        utmpent = getnewutmpent(&uinfo);
        if (utmpent >= 0 || utmpent == -1)
            break;
        if (utmpent == -2 && listmode <= 100)
        {
            listmode++;
            usleep(250);
            continue;
        }
        if (listmode > 100)
        {
            sprintf(genbuf, "getnewutmpent(): too much times, give up.");
            report(genbuf);
            prints("getnewutmpent(): ʧ��̫���, ����. ��ر�վ��.\n");
            sleep(3);
            exit(0);
        }
    }
    if (utmpent < 0)
    {
        sprintf(genbuf, "Fault: No utmpent slot for %s\n", uinfo.userid);
        report(genbuf);
    }
    listmode = 0;
    digestmode = NA;
}

void setflags(int mask, int value)
{
    if (((currentuser.flags[0] & mask) && 1) != value)
    {
        if (value)
            currentuser.flags[0] |= mask;
        else
            currentuser.flags[0] &= ~mask;
    }
}

int kick_web_user(struct user_info *user)
{
    int stay = 0;
    FILE *fp;
    struct userec u;

    if (!user->active)
        return -1;
    stay = abs(time(0) - user->logintime);
    if (stay > 3600 * 24 * 3)
        stay = 3600 * 24 * 3;
    else if (stay < 0)
        stay = 0;

    fp = fopen(".PASSWDS", "r+");
    fseek(fp, (user->uid - 1) * sizeof(struct userec), SEEK_SET);
    fread(&u, sizeof(struct userec), 1, fp);
    if (strcasecmp(u.userid, user->userid))
    {
        fclose(fp);
        memset(user, 0, sizeof(struct user_info));
        return -2;
    }
    u.stay += stay;
    u.lastlogout = time(0);
    fseek(fp, -sizeof(struct userec), SEEK_CUR);
    fwrite(&u, sizeof(struct userec), 1, fp);
    fclose(fp);
    memset(user, 0, sizeof(struct user_info));
    return 0;
}

void u_exit(void)
{
    int fd;

    signal(SIGHUP, SIG_DFL);
    signal(SIGALRM, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);

    setflags(PAGER_FLAG, (uinfo.pager & ALL_PAGER));
    if (HAS_PERM(PERM_LOGINCLOAK))
        setflags(CLOAK_FLAG, uinfo.invisible);

    if (!ERROR_READ_SYSTEM_FILE)
    {
        time_t stay;

        set_safe_record();
        stay = time(0) - login_start_time;
        currentuser.stay += stay;
        currentuser.lastlogout = time(0);
        substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                          usernum);
    }

    uinfo.invisible = YEA;
    uinfo.sockactive = NA;
    uinfo.sockaddr = 0;
    uinfo.destuid = 0;
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

    strcpy(uinfo.tty, "NoTTY");
#endif

    uinfo.pid = 0;
    uinfo.active = NA;

    if ((fd = open(ULIST, O_RDWR | O_CREAT, 0600)) > 0)
    {
        flock(fd, LOCK_EX);
    }
    update_utmp();
    if (fd > 0)
    {
        flock(fd, LOCK_UN);
        close(fd);
    }
    utmpent = -1;
}

int cmpuids(char *uid, void *vup)
{
    struct userec *up = (struct userec *) vup;

    return !strncasecmp(uid, up->userid, sizeof(up->userid));
}

int dosearchuser(char *userid)
{
    int id;

    if ((id = getuser(userid)) != 0)
    {
        if (cmpuids(userid, &lookupuser))
        {
            memcpy(&currentuser, &lookupuser, sizeof(currentuser));
            return usernum = id;
        }
    }
    memset(&currentuser, 0, sizeof(currentuser));
    return usernum = 0;
}

void talk_request(int num)
{
    signal(SIGUSR1, talk_request);
    talkrequest = YEA;
    bell();
    bell();
    bell();
    sleep(1);
    bell();
    bell();
    bell();
    bell();
    bell();
    return;
}

void abort_bbs(void)
{
    extern int child_pid;

    if (child_pid)
        kill(child_pid, 9);
    if (uinfo.mode == POSTING || uinfo.mode == SMAIL || uinfo.mode == EDIT
        || uinfo.mode == EDITUFILE || uinfo.mode == EDITSFILE
        || uinfo.mode == EDITANN)
        keep_fail_post();

    if (started)
    {
        time_t stay;

        stay = time(0) - login_start_time;
        sprintf(genbuf, "Stay: %3ld (%s)", stay / 60,
                currentuser.username);

        userexitboard(currboard);

        log_usies("AXXED", genbuf);
        u_exit();
    }
    exit(0);
}

int cmpuids2(int unum, struct user_info *urec)
{
    return (unum == urec->uid);
}

int count_multi(struct user_info *uentp)
{
    static int count;

    if (uentp == NULL)
    {
        int num = count;

        count = 0;
        return num;
    }
    if (!uentp->active || !uentp->pid)
        return 0;
    if (uentp->uid == usernum)
        count++;
    return 1;
}

int count_user(void)
{
    count_multi(NULL);
    apply_ulist(count_multi);
    return count_multi(NULL);
}

void multi_user_check(void)
{
    struct user_info uin;
    int logins, mustkick = 0, pos;

    if (HAS_PERM(PERM_MULTILOG))
        return;

#ifdef SPECIALID

    if (strcmp(currentuser.userid, SPECIALID1) == 0
        || strcmp(currentuser.userid, SPECIALID2) == 0)
        return;
#endif

    logins = count_user();

    if (heavyload() && logins)
    {
        prints("[1;33m��Ǹ, Ŀǰϵͳ���ɹ���, �����ظ� Login��[m\n");
        oflush();
        sleep(3);
        exit(1);
    }
    if (!strcasecmp("guest", currentuser.userid))
    {
        if (logins > MAXGUEST)
        {
            prints
                ("[1;33m��Ǹ, Ŀǰ����̫�� [1;36mguest[33m, ���Ժ����ԡ�[m\n");
            oflush();
            sleep(3);
            exit(1);
        }
        return;
    }
    else if (logins >= MULTI_LOGINS)
    {
        prints
            ("[1;32mΪȷ��������վȨ��, ��վ���������ø��ʺŵ�½ %d ����\n[0m",
             MULTI_LOGINS);
        prints
            ("[1;36m��Ŀǰ�Ѿ�ʹ�ø��ʺŵ�½�� %d ����������Ͽ����������ӷ��ܽ��뱾վ��\n[0m",
             logins);
        mustkick = 1;
    }
    if (!(pos = search_ulist(&uin, cmpuids2, usernum)))
        return;

    if (!uin.active || (uin.pid && kill(uin.pid, 0) == -1))
        return;
    getdata(0, 0, "[1;37m����ɾ���ظ��� login �� (Y/N)? [N][m",
            genbuf, 4, DOECHO, YEA);
    if (genbuf[0] == 'N' || genbuf[0] == 'n' || genbuf[0] == '\0')
    {
        if (!mustkick)
            return;
        prints
            ("[33m�ܱ�Ǹ�����Ѿ��ø��ʺŵ�½ %d �������ԣ������߽���ȡ����[m\n",
             logins);
        oflush();
        sleep(3);
        exit(1);
    }
    else
    {
        if (!uin.pid)
            return;

        if (is_web_user(&uin))
            kick_web_user(&(utmpshm->uinfo[pos - 1]));
        else
            kill(uin.pid, SIGHUP);

        report("kicked (multi-login)");
        log_usies("KICK ", currentuser.username);
    }
}
static void signal_abort_bbs(int num)
{
    abort_bbs();
}

#ifndef BBSD
void system_init(argc, argv)
int argc;
char **argv;
#else
void system_init(void)
#endif
{
#ifndef BBSD
    char *rhost;
#endif

    struct sigaction act;

    gethostname(genbuf, 256);
    sprintf(ULIST, "%s.%s", ULIST_BASE, genbuf);

#ifndef BBSD

    if (argc >= 3)
    {
        strncpy(fromhost, argv[2], 60);
    }
    else
    {
        fromhost[0] = '\0';
    }
    if ((rhost = getenv("REMOTEHOST")) != NULL)
        strncpy(fromhost, rhost, 60);
    fromhost[59] = '\0';
#if defined(SHOW_IDLE_TIME)

    if (argc >= 4)
    {
        strncpy(tty_name, argv[3], 20);
    }
    else
    {
        tty_name[0] = '\0';
    }
#endif
#endif

#ifndef lint
    signal(SIGHUP, signal_abort_bbs);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
#ifdef DOTIMEOUT

    init_alarm();
    uinfo.mode = LOGIN;
    alarm(LOGIN_TIMEOUT);
#else

    signal(SIGALRM, SIG_SIG);
#endif

    signal(SIGTERM, SIG_IGN);
    signal(SIGURG, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
#endif

    signal(SIGUSR1, talk_request);

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    act.sa_handler = r_msg;
    sigaction(SIGUSR2, &act, NULL);
}

void system_abort(void)
{
    if (started)
    {
        log_usies("ABORT", currentuser.username);
        u_exit();
    }
    clear();
    refresh();
    prints("лл����, �ǵó���� !\n");
    exit(0);
}

void logattempt(char *uid, char *frm)
{
    char fname[STRLEN];

    getdatestring(time(0), NA);
    sprintf(genbuf, "%-12.12s  %-30s %s\n", uid, datestring, frm);
    file_append(BADLOGINFILE, genbuf);
    sethomefile(fname, uid, BADLOGINFILE);
    file_append(fname, genbuf);

}

void check_tty_lines(void)
{
    static unsigned char buf1[] = { 255, 253, 31 };
    unsigned char buf2[100];
    int n;

    if (ttyname(0))
        return;
    write(0, buf1, 3);
    n = read(0, buf2, 80);
    if (n == 12)
    {
        if (buf2[0] != 255 || buf2[1] != 251 || buf2[2] != 31)
            return;
        if (buf2[3] != 255 || buf2[4] != 250 || buf2[5] != 31
            || buf2[10] != 255 || buf2[11] != 240)
            return;
        t_lines = buf2[9];
    }
    if (n == 9)
    {
        if (buf2[0] != 255 || buf2[1] != 250 || buf2[2] != 31
            || buf2[7] != 255 || buf2[8] != 240)
            return;
        t_lines = buf2[6];
    }
    if (t_lines < 24 || t_lines > 100)
        t_lines = 24;
}

struct max_log_record
{
    int year;
    int month;
    int day;
    int logins;
    unsigned long visit;
} max_log;
void visitlog(void)
{
    int vfp;
    time_t now;
    struct tm *tm;
    extern struct UTMPFILE *utmpshm;

    vfp = open(VISITLOG, O_RDWR | O_CREAT, 0644);
    if (vfp == -1)
    {
        report("Can NOT write visit Log to .visitlog");
        return;
    }
    flock(vfp, LOCK_EX);
    lseek(vfp, (off_t) 0, SEEK_SET);
    read(vfp, &max_log, (size_t) sizeof(max_log));
    if (max_log.year < 1990 || max_log.year > 2020)
    {
        now = time(0);
        tm = localtime(&now);
        max_log.year = tm->tm_year + 1900;
        max_log.month = tm->tm_mon + 1;
        max_log.day = tm->tm_mday;
        max_log.visit = 0;
        max_log.logins = 0;
    }
    max_log.visit++;
    if (max_log.logins > utmpshm->max_login_num)
        utmpshm->max_login_num = max_log.logins;
    else
        max_log.logins = utmpshm->max_login_num;
    lseek(vfp, (off_t) 0, SEEK_SET);
    write(vfp, &max_log, (size_t) sizeof(max_log));
    flock(vfp, LOCK_UN);
    close(vfp);
    sprintf(genbuf,
            "[1;32m�� [[36m%4d��%2d��%2d��[32m] ��, ���������¼: [[36m%d[32m] �ۼƷ����˴�: [[36m%lu[32m][m\n",
            max_log.year, max_log.month, max_log.day, max_log.logins,
            max_log.visit);
    prints(genbuf);
}

void login_query(void)
{
    char uid[IDLEN + 2];
    char passbuf[PASSLEN];
    int curr_login_num;
    int attempts;
    char genbuf[STRLEN];
    char *ptr;
    extern struct UTMPFILE *utmpshm;

    curr_login_num = num_active_users();
    if (curr_login_num >= MAXACTIVE)
    {
        ansimore("etc/loginfull", NA);
        oflush();
        sleep(1);
        exit(1);
    }
#ifdef BBSNAME
    strcpy(BoardName, BBSNAME);
#else

    ptr = sysconf_str("BBSNAME");
    if (ptr == NULL)
        ptr = "��δ��������վ";
    strcpy(BoardName, ptr);
#endif

    if (fill_shmfile(1, "etc/issue", "ISSUE_SHMKEY"))
    {
        show_issue();
    }
    prints
        ("[1;35m��ӭ����[1;40;33m�� %s �� [m[[0;1;33;41m Add '.' after YourID to login for BIG5 [m]\n[0;1;32m��վ��ע���ʺ���: [[1;36m%d[0;1;32m] [m",
         BoardName, MAXUSERS);
    resolve_utmp();
    if (utmpshm->usersum == 0)
        utmpshm->usersum = allusers();
    if (utmpshm->max_login_num < curr_login_num)
        utmpshm->max_login_num = curr_login_num;
    prints
        ("[1;32mĿǰ�����ʺ���: [[1;36m%d[32m] [1;32mĿǰ��վ����: [[1;36m%d[1;32m/[1;36m%d[1;32m] \n",
         utmpshm->usersum, curr_login_num, MAXACTIVE);

    visitlog();

#ifdef MUDCHECK_BEFORELOGIN

    prints("[1;33mΪ��ֹʹ�ó�ʽ��վ���밴 [1;36mCTRL + C[m : ");
    genbuf[0] = igetkey();
    if (genbuf[0] != Ctrl('C'))
    {
        prints("\n�Բ����㲢û�а��� CTRL+C ����\n");
        oflush();
        exit(1);
    }
    else
    {
        prints("[CTRL] + [C]\n");
    }
#endif

    attempts = 0;
    while (1)
    {
        if (attempts++ >= LOGINATTEMPTS)
        {
            ansimore("etc/goodbye", NA);
            oflush();
            sleep(1);
            exit(1);
        }
        prints
            ("[1;33m�������ʺ�[m(���������� `[1;36mguest[m', ע��������`[1;31mnew[m')");
        getdata(0, 0, ": ", uid, IDLEN + 1, DOECHO, YEA);
#ifdef ALLOWSWITCHCODE

        ptr = strchr(uid, '.');
        if (ptr)
        {
            convcode = 1;
            *ptr = '\0';
        }
#endif
        if ((strcasecmp(uid, "guest") == 0)
            && (MAXACTIVE - curr_login_num < 10))
        {
            ansimore("etc/loginfull", NA);
            oflush();
            sleep(1);
            exit(1);
        }
        if (strcasecmp(uid, "new") == 0)
        {
#ifdef LOGINASNEW
            memset(&currentuser, 0, sizeof(currentuser));
            new_register();
            ansimore_withmode("etc/firstlogin", YEA, MORE_MODE_STUFF);
            break;
#else

            prints
                ("[1;37m��ϵͳĿǰ�޷��� [36mnew[37m ע��, ����[36m guest[37m ����...[m\n");
#endif

        }
        else if (*uid == '\0');
        else if (!dosearchuser(uid))
        {
            prints("[1;31m����֤���޴� ID (User ID Error)...[m\n");
        }
        else if (strcasecmp(uid, "guest") == 0)
        {
            currentuser.userlevel = 0;
            break;
#ifdef SYSOPLOGINPROTECT

        }
        else if (!strcasecmp(uid, "SYSOP")
                 && strcmp(fromhost, "localhost")
                 && strcmp(fromhost, "127.0.0.1"))
        {
            prints("[1;32m ����: �� %s ��¼�ǷǷ���! ��������![m\n",
                   fromhost);
            prints
                ("[ע��] Ϊ��ȫ�������վ�Ѿ��趨 SYSOP ֻ�ܴ�������½��\n       �����ȷʵ�Ǳ�վ�� SYSOP �����½���� BBS ��������Ȼ��: \n              telnet localhost port.\n");
            oflush();
            sleep(1);
            exit(1);
#endif

        }
        else
        {
#ifdef ALLOWSWITCHCODE
            if (!convcode)
                convcode = !(currentuser.userdefine & DEF_USEGB);
#endif

            getdata(0, 0, "[1;37m����������: [m", passbuf, 9,
                    NOECHO, YEA);
            passbuf[8] = '\0';

            if (!checkpasswd(currentuser.passwd, passbuf))
            {
                logattempt(currentuser.userid, fromhost);
                prints("[1;31m����������� (Password Error)...[m\n");
            }
            else
            {
#ifdef SPECIALID
                if (strcmp(uid, SPECIALID1) == 0)
                {
                    ansimore("etc/specialwelcomeboy", NA);
                    prints("�����������ǳ� [��������boy] ��");
                    getdata(0, 0, "", currentuser.username, NAMELEN,
                            DOECHO, YEA);
                    if (strlen(currentuser.username) == 0)
                        strcpy(currentuser.username, "��������boy");
                }
                else if (strcmp(uid, SPECIALID2) == 0)
                {
                    ansimore("etc/specialwelcomegirl", NA);
                    prints("�����������ǳ� [��������girl] ��");
                    getdata(0, 0, "", currentuser.username, NAMELEN,
                            DOECHO, YEA);
                    if (strlen(currentuser.username) == 0)
                        strcpy(currentuser.username, "��������girl");
                }
                else
                {
#endif
                    if (!HAS_PERM(PERM_BASIC) && !HAS_PERM(PERM_SYSOP))
                    {
                        prints
                            ("[1;32m���ʺ���ͣ�������� [36mSYSOP[32m ��ѯԭ��[m\n");
                        oflush();
                        sleep(1);
                        exit(1);
                    }
#ifdef CHECK_LESS60SEC
                    if (abs(time(0) - currentuser.lastlogin) < 60
                        && !HAS_PERM(PERM_SYSOP)
                        && strcasecmp(currentuser.userid, "guest") != 0)
                    {
                        prints
                            ("���� 60 �����ظ���վ.�밴 [1;36mCtrl+C[m ��ȷ���������: ");
                        genbuf[0] = igetkey();
                        if (genbuf[0] != Ctrl('C'))
                        {
                            prints
                                ("\n�Բ�������û�а��� CTRL+C ���������ܽ��뱾վ\n");
                            prints("����������֪ͨվ����Ա, лл.\n");
                            oflush();
                            sleep(3);
                            exit(1);
                        }
                        prints("\n");
                    }
#endif
#ifdef SPECIALID

                }
#endif

#ifdef CHECK_SYSTEM_PASS
                if (HAS_PERM(PERM_SYSOP))
                {
                    if (!check_systempasswd())
                    {
                        prints("\n�������, ����ǩ�� ! !\n");
                        oflush();
                        sleep(2);
                        exit(1);
                    }
                }
#endif
                bzero(passbuf, PASSLEN - 1);
                break;
            }
        }
    }
    multi_user_check();
    if (!term_init(currentuser.termtype))
    {
        prints("Bad terminal type.  Defaulting to 'vt100'\n");
        strcpy(currentuser.termtype, "vt100");
        term_init(currentuser.termtype);
    }
    check_tty_lines();
    sethomepath(genbuf, currentuser.userid);
    mkdir(genbuf, 0755);
    login_start_time = time(0);
}

#ifdef REAL_NAME_EMAIL
int check_email_id(const char *id)
{
    int i;

    for (i = 0; id[i]; i++)
        if (!
            (isalnum(id[i]) || id[i] == '.' || id[i] == '_'
             || id[i] == '-'))
            return 1;
    return 0;
}
inline int is_strcase_with_prefix(const char *prefix, int len,
                                  const char *str)
{
    return !strncasecmp(prefix, str, len);
}
inline int is_strcase_with_suffix(const char *suffix, int len,
                                  const char *str)
{
    int slen;

    slen = strlen(str);
    if (slen < len)
        return 0;
    return !strncasecmp(suffix, str + slen - len, len);
}
int check_email_domain(const char *domain)
{
    FILE *fp;
    char line[512];
    int len;
    int i;

    if (!(isalnum(domain[0]) && isalpha(domain[strlen(domain) - 1])))
        return 1;
    for (i = 0; domain[i]; i++)
        if (!(isalnum(domain[i]) || domain[i] == '.' || domain[i] == '-'))
            return 1;

    if (!(fp = fopen(REAL_NAME_EMAIL_RULE, "r")))
        return 1;
    while (fgets(line, sizeof(line), fp))
    {
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
            len--;
        }
        if (len < 2)
            continue;

        switch (line[0])
        {
        case 'P':
            if (is_strcase_with_prefix(&line[1], len - 1, domain))
            {
                fclose(fp);
                return 1;
            }
            else
                break;
        case 'p':
            if (is_strcase_with_prefix(&line[1], len - 1, domain))
            {
                fclose(fp);
                return 0;
            }
            else
                break;
        case 'S':
            if (is_strcase_with_suffix(&line[1], len - 1, domain))
            {
                fclose(fp);
                return 1;
            }
            else
                break;
        case 's':
            if (is_strcase_with_suffix(&line[1], len - 1, domain))
            {
                fclose(fp);
                return 0;
            }
            else
                break;
        case 'A':
            if (!strcasecmp(&line[1], domain))
            {
                fclose(fp);
                return 1;
            }
            else
                break;
        case 'a':
            if (!strcasecmp(&line[1], domain))
            {
                fclose(fp);
                return 0;
            }
            else
                break;
        default:
            break;
        }
    }
    fclose(fp);
    return 1;
}
int check_real_name_email(const char *email)
{
    char *pat, *domain, buf[REAL_NAME_EMAIL_LEN + 1];

    strcpy(buf, email);
    pat = strchr(buf, '@');
    if (!pat)
        return 1;
    *pat = '\0';
    domain = pat + 1;
    if (check_email_id(buf) || check_email_domain(domain))
        return 1;
    else
        return 0;
}
void get_real_name_email(char *buf)
{
    char path[128];
    int fd;
    ssize_t readnum;

    sethomefile(path, currentuser.userid, REAL_NAME_FILE);
    if ((fd = open(path, O_RDONLY)) == -1)
    {
        buf[0] = '\0';
        return;
    }
    readnum = read(fd, buf, REAL_NAME_EMAIL_LEN);
    close(fd);
    buf[readnum] = '\0';
    return;
}
int write_real_name_email(const char *email)
{
    char path[128];
    FILE *file;

    sethomefile(path, currentuser.userid, REAL_NAME_FILE);
    if ((file = fopen(path, "w")) == NULL)
    {
        return 1;
    }
    fprintf(file, email);
    fclose(file);
    return 0;
}
int mailto_real_name_mail(char *userid)
{
    char email[REAL_NAME_EMAIL_LEN + 1], fname[256], code_fname[256];
    const char title[] = BBSNAME "ע��ȷ����";
    unsigned int rand[4], return_no;
    FILE *content, *code;

    get_real_name_email(email);
    if (!email[0])
        return 1;
    get_random(rand, 4);
    sprintf(fname, "tmp/check_mail.%s", userid);
    content = fopen(fname, "w");
    if (content == NULL)
        return 2;
    fprintf(content, "\n\
    ��ã�����%s(%s)ע��ȷ���š�\n\
    ���������%sע��ID %s,����������������ע�᣺\n\
    http://%s/bbsdochkmail?id=%s&code=%08x%08x%08x%08x\n\
    �����û��ע�����ID����ɾ�����š�����Ϊ���ŵĴ��Ÿе�Ǹ�⡣\n\n\n\
                                                  %sվ����\n", BBSNAME, BBSHOST, BBSNAME, userid, BBSHOST, userid, rand[0], rand[1], rand[2], rand[3], BBSNAME);
    fclose(content);
    sethomefile(code_fname, userid, REAL_NAME_CHK_CODE);
    code = fopen(code_fname, "w");
    if (code == NULL)
    {
        unlink(fname);
        return 3;
    }
    fprintf(code, "%08x%08x%08x%08x", rand[0], rand[1], rand[2], rand[3]);
    fclose(code);
    return_no = bbs_sendmail(fname, (char *) title, email, NA, NA);
    unlink(fname);
    return return_no;
}
static int check_real_name_email_blocked(const char *email)
{
    char buf[REAL_NAME_EMAIL_LEN + 1];
    char namelist[] = BBSHOME "/etc/bad_remail";
    FILE *fp;

    fp = fopen(namelist, "r");
    if (fp == NULL)
        return 1;
    while (fgets(buf, 255, fp) != NULL)
    {
        if (strncasecmp(email, buf, strlen(email)))
        {
            continue;
        }
        else
        {
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return 1;
}
void ask_real_mail(void)
{
    char email[REAL_NAME_EMAIL_LEN + 1], genbuf[128],
        buf[REAL_NAME_EMAIL_LEN + 1];

    if (HAS_PERM(PERM_REALNAME))
        return;

    if (strcmp("guest", currentuser.userid) == 0)
        return;

    show_help(REAL_NAME_EMAIL_RULE_DSC);
    clear();
    move(3, 0);
    if (askyn("����Ҫ��������", YEA, YEA) == NA)
        return;
    get_real_name_email(email);
    buf[0] = '\0';
    sprintf(genbuf, "����������[%s] :", email);
    while (check_real_name_email(buf))
    {
        clear();
        getdata(1, 0, genbuf, buf, REAL_NAME_EMAIL_LEN, DOECHO, YEA);
        if (askyn("������ȷ�� ", YEA, NA))
        {
            if (strlen(buf) == 0)
                strcpy(buf, email);
        }
        else
            buf[0] = '\0';
    }
    if (!check_real_name_email_blocked(buf))
    {

        pressanykey();
        return;
    }
    write_real_name_email(buf);
    if (mailto_real_name_mail(currentuser.userid))
        prints("����ȷ���ʼ�ʧ��");
    else
        prints("ȷ���ʼ��ѷ��ͣ������");
    pressanykey();

}
#endif
int get_random(unsigned int buf[], int size)
{
    int fd;

    if ((fd = open("/dev/urandom", O_RDONLY)) != -1)
    {
        read(fd, buf, sizeof(unsigned int) * size);
        close(fd);
    }
    else
    {
        int i;

        for (i = 0; i < size; i++)
            buf[i] = random();
    }
    return 0;
}

int valid_ident(char *ident)
{
    static char *invalid[] = { "unknown@", "root@", "gopher@", "bbs@",
        "guest@", "nobody@", "www@", NULL
    };
    int i;

    if (ident[0] == '@')
        return 0;
    for (i = 0; invalid[i] != NULL; i++)
        if (strstr(ident, invalid[i]) != NULL)
            return 0;
    return 1;
}

void write_defnotepad(void)
{
    currentuser.notedate = time(0);
    set_safe_record();
    substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                      usernum);
    return;
}

void notepad_init(void)
{
    FILE *check;
    char notetitle[STRLEN];
    char tmp[STRLEN * 2];
    char *fname, *bname, *ntitle;
    long int maxsec;
    time_t now;

    maxsec = 86400;
    lastnote = 0;
    if ((check = fopen("etc/checknotepad", "r")) != NULL)
    {
        fgets(tmp, sizeof(tmp), check);
        lastnote = atol(tmp);
        fclose(check);
    }
    now = time(0);
    if ((now - lastnote) >= maxsec)
    {
        move(t_lines - 1, 0);
        prints("�Բ���ϵͳ�Զ����ţ����Ժ�.....");
        refresh();
        check = fopen("etc/checknotepad", "w");
        lastnote = now - (now % maxsec);
        fprintf(check, "%ld", lastnote);
        fclose(check);
        if ((check = fopen("etc/autopost", "r")) != NULL)
        {
            while (fgets(tmp, STRLEN, check) != NULL)
            {
                fname = strtok(tmp, " \n\t:@");
                bname = strtok(NULL, " \n\t:@");
                ntitle = strtok(NULL, " \n\t:@");
                if (fname == NULL || bname == NULL || ntitle == NULL)
                    continue;
                else
                {
                    getdatestring(now, NA);
                    sprintf(notetitle, "[%8.8s %6.6s] %s", datestring + 6,
                            datestring + 23, ntitle);
                    if (dashf(fname))
                    {
                        Postfile(fname, bname, notetitle, 1);
                        sprintf(tmp, "%s �Զ�����", ntitle);
                        report(tmp);
                    }
                }
            }
            fclose(check);
        }
        getdatestring(now, NA);
        sprintf(notetitle, "[%s] ���԰��¼", datestring);
        if (dashf("etc/notepad"))
        {
            Postfile("etc/notepad", "message", notetitle, 1);
            unlink("etc/notepad");
        }
        report("�Զ�����ʱ�����");
    }
    return;
}

void user_login(void)
{
    char fname[STRLEN];
    char *ruser;

    if (strcmp(currentuser.userid, "SYSOP") == 0)
    {
        currentuser.userlevel = ~0;
        substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                          usernum);
    }
    ruser = getenv("REMOTEUSER");
    sprintf(genbuf, "%s@%s", ruser ? ruser : "?", fromhost);
    log_usies("ENTER", genbuf);
    u_enter();
    if (ruser != NULL)
    {
        sprintf(genbuf, "%s@%s", ruser, fromhost);
        if (valid_ident(genbuf))
        {
            strncpy(currentuser.ident, genbuf, NAMELEN);
            currentuser.ident[NAMELEN - 1] = '\0';
        }
    }
    report("Enter");
    started = 1;
    if ((!HAS_PERM(PERM_MULTILOG | PERM_SYSOP))
        && count_user() > MULTI_LOGINS
        && strcmp(currentuser.userid, "guest"))
    {
        report("kicked (multi-login)[©��֮��]");
        abort_bbs();
    }
    initscr();
#ifdef USE_NOTEPAD

    notepad_init();
    if (strcmp(currentuser.userid, "guest") != 0)
    {
        if (DEFINE(DEF_NOTEPAD))
        {
            int noteln;

            if (lastnote > currentuser.notedate)
                currentuser.noteline = 0;
            noteln = countln("etc/notepad");
            if (currentuser.noteline == 0)
            {
                shownotepad();
            }
            else if ((noteln - currentuser.noteline) > 0)
            {
                move(0, 0);
                ansimore_partial("etc/notepad", NA, 0,
                          noteln - currentuser.noteline + 1);
                igetkey();
                clear();
            }
            currentuser.noteline = noteln;
            write_defnotepad();
        }
    }
#endif
    if (DEFINE(DEF_GRAPH))
    {
        show_help("0Announce/bbslist/countusr");
    }
    if ((vote_flag(NULL, '\0', 2) == 0))
    {

        {
            if (dashf("Welcome"))
            {
                ansimore("Welcome", YEA);
                vote_flag(NULL, 'R', 2);
            }

        }
    }
    else
    {
        if (fill_shmfile(3, "Welcome2", "WELCOME_SHMKEY"))
            show_welcomeshm();
    }
    if (DEFINE(DEF_TOP10))
    {
        show_help("etc/posts/day");
    }
    refresh();
    move(t_lines - 2, 0);
    clrtoeol();

    if (currentuser.numlogins < 1)
    {
        currentuser.numlogins = 0;
        getdatestring(time(0), NA);
        prints
            ("\033[1;36m�� �������� \033[33m1\033[36m �ΰݷñ�վ�����ס����ɡ�\n");
        prints("�� ����һ�����뱾վ��ʱ��Ϊ \033[33m%s\033[m ",
               datestring);
    }
    else
    {
        getdatestring(currentuser.lastlogin, NA);
        prints
            ("\033[1;36m�� �������� \033[33m%d\033[36m �ΰݷñ�վ���ϴ����Ǵ� \033[33m%s\033[36m ������վ��\n",
             currentuser.numlogins + 1, currentuser.lasthost);
        prints("�� �ϴ�����ʱ��Ϊ \033[33m%s\033[m ", datestring);
    }
    igetkey();
    WishNum = 9999;
    setuserfile(fname, BADLOGINFILE);
    if (ansimore(fname, NA) != -1)
    {
        getdata(t_lines - 1, 0,"������������¼  (Y) ɾ����¼ (N) ���� (M) �Ļ����䣿[N]��", genbuf, 4, DOECHO, YEA);
        if (genbuf[0] == 'M' || genbuf[0] == 'm')
        {
            mail_file(fname, currentuser.userid, "������������¼");
            unlink(fname);
        }
        if (genbuf[0] == 'Y' || genbuf[0] == 'y')
            unlink(fname);
    }

    set_safe_record();
    check_uinfo(&currentuser, 0);
    strncpy(currentuser.lasthost, fromhost, 60);
    currentuser.lasthost[59] = '\0';
    currentuser.lastlogin = time(0);

    if (HAS_PERM(PERM_SYSOP) || !strcmp(currentuser.userid, "guest"))
        currentuser.lastjustify = time(0);

    currentuser.numlogins++;
    if (currentuser.firstlogin == 0)
    {
        currentuser.firstlogin = time(0) - 7 * 86400;
    }
    substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                      usernum);
    check_register_info();

    if (HAS_PERM(PERM_BOARDS | PERM_EXT_IDLE))
    {
        FILE *blog_file;

        sethomefile(fname, currentuser.userid, "blog");
        if ((blog_file = fopen(fname, "r")) != NULL)
        {
            fgets(blog_user_id, 32, blog_file);
            fgets(blog_id, 32, blog_file);
            fgets(blog_catId, 32, blog_file);
            if (blog_user_id[strlen(blog_user_id) - 1] == '\n')
                blog_user_id[strlen(blog_user_id) - 1] = '\0';
            if (blog_id[strlen(blog_id) - 1] == '\n')
                blog_id[strlen(blog_id) - 1] = '\0';
            if (blog_catId[strlen(blog_catId) - 1] == '\n')
                blog_catId[strlen(blog_catId) - 1] = '\0';
            fclose(blog_file);
        }

    }

}

void set_numofsig(void)
{
    int sigln;
    char signame[STRLEN];

    setuserfile(signame, "signatures");
    sigln = countln(signame);
    numofsig = sigln / MAXSIGLINES;
    if ((sigln % MAXSIGLINES) != 0)
        numofsig += 1;
}

int check_maxmail(void)
{
    extern char currmaildir[STRLEN];
    int maxmail, maxsize, mailsize, oldnum;
    int bmns = 0;

    if (HAS_PERM(PERM_SYSOP))
    {
        maxmail = MAX_SYSOPMAIL_HOLD;
    }
    else if (HAS_PERM(PERM_BOARDS))
    {
        bmns = getbmns(currentuser.userid);
        if (bmns > 0)
            maxmail = MAX_BMMAIL_HOLD + MAX_MAIL_HOLD * (bmns - 1);
        else
            maxmail = MAX_BMMAIL_HOLD;
    }
    else
    {
        maxmail = MAX_MAIL_HOLD;
    }

    set_safe_record();
    oldnum = currentuser.nummails;
    currentuser.nummails =
        get_num_records(currmaildir, sizeof(struct fileheader));
    if (oldnum != (int) currentuser.nummails)
        substitute_record(PASSFILE, &currentuser, sizeof(currentuser),
                          usernum);
    if (HAS_PERM(PERM_MAILINFINITY))
        return 0;
    maxsize = getmailboxsize(currentuser.userid, currentuser.userlevel);
    mailsize = getmailsize(currentuser.userid);
    if ((int) currentuser.nummails > maxmail || mailsize > maxsize)
    {
        mailXX = 1;
        clear();
        move(4, 0);
        if ((int) currentuser.nummails > maxmail)
            prints("����˽���ż��ߴ� %d ��, �����ʼ�����: %d ��\n",
                   currentuser.nummails, maxmail);
        if (mailsize > maxsize)
            prints("�����ż������ߴ� %d K��������������: %d K\n",
                   mailsize, maxsize);
        prints
            ("����˽���ż��Ѿ�����, ���������䣬�����޷�ʹ�ñ�վ�����Ź��ܡ�\n");
        if ((int) currentuser.nummails > maxmail + 100)
        {
            sprintf(genbuf, "˽���ż�����: %d ��", currentuser.nummails);
            securityreport(genbuf);
        }
        if (mailsize > maxsize + 1000)
        {
            sprintf(genbuf, "˽���ż�����: %d K", mailsize);
            securityreport(genbuf);
        }
    }
    else
    {
        mailXX = 0;
    }
    return mailXX;
}

int ci_strcmp(s1, s2)
register char *s1, *s2;
{
    char c1, c2;

    while (1)
    {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 += 'A' - 'a';
        if (c2 >= 'a' && c2 <= 'z')
            c2 += 'A' - 'a';
        if (c1 != c2)
            return (c1 - c2);
        if (c1 == 0)
            return 0;
    }
}

//add parameter denytype by Ellias, July 9th, 2011, 0 means deny post, 1 means deny base
int seek_in_qzdenyfile(int denytype)
{
    char filename[STRLEN];
    char filename2[30];
    FILE *fp;
    char buf[STRLEN];
    char *namep, *date;

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
    for (i = 0; i < SectNum + 1; i++)
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
            if (namep != NULL && ci_strcmp(namep, currentuser.userid) == 0)
            {

                nowtime = time(0);
                getdatestring(nowtime, NA);
                flag = buf[55];
                date = (char *) (buf + 58);
                if (flag == 'a' && (strncmp(datestring, date, 16) >= 0))
                {
                    if (del_from_file(filename, currentuser.userid))
                    {
                        char msgbuf[STRLEN];
                        char repbuf[STRLEN];
                        int id = getuser(currentuser.userid);

						if(denytype == 0)
                        {
							lookupuser.userlevel |= PERM_POST;
                        }
                        else
                        {
                            lookupuser.userlevel |= PERM_CHAT;
                            lookupuser.userlevel |= PERM_POST;
                            lookupuser.userlevel |= PERM_PAGE;
                            lookupuser.userlevel |= PERM_MESSAGE;
                        }
                        substitute_record(PASSFILE, &lookupuser,
                                          sizeof(lookupuser), id);
                        setuserid(id, lookupuser.userid);
						if(denytype == 0)
						{
							sprintf(repbuf, "�ָ�%s��ȫվ�ķ���Ȩ��",
									currentuser.userid);
							sprintf(msgbuf, "\n  %s ���ѣ�\n\n"
									"\t����ʱ���ѹ����ָֻ�����ȫվ�ġ��������¡�Ȩ����\n\n"
									"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
									currentuser.userid, BBSID);
						}
						else
						{
							sprintf(repbuf, "�ָ�%s��ȫվ�Ļ���Ȩ��",
									currentuser.userid);
							sprintf(msgbuf, "\n  %s ���ѣ�\n\n"
									"\t����ʱ���ѹ����ָֻ�����ȫվ�Ļ���Ȩ����\n\n"
									"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
									currentuser.userid, BBSID);
						}
#ifdef DENY_POST_BOARDNAME
                        if (strncmp(currboard, "BMTraining", 10) == 0)
                        {
                            autoreport(repbuf, msgbuf, currentuser.userid,
                                       TRAIN_ANNOUNCE_BOARDNAME);
                        }
                        else
                        {
                            autoreport(repbuf, msgbuf, currentuser.userid,
                                       DENY_POST_BOARDNAME);
                        }

#else

                        autoreport(repbuf, msgbuf, currentuser.userid,
                                   NULL);
#endif

                        break;
                    }
                }
                break;
            }
        }
        fclose(fp);
    }

    return 0;
}

#ifndef BBSD
int main(argc, argv)
int argc;
char **argv;
#else
void start_client(void)
#endif
{
    load_sysconf();

#ifndef BBSD

    if (argc < 2 || *argv[1] != 'h')
    {
        printf("You cannot execute this program directly.\n");
        exit(-1);
    }
    system_init(argc, argv);
#else

    system_init();
#endif

    if (setjmp(byebye))
    {
        system_abort();
    }
#ifndef BBSD
    get_tty();
    init_tty();
#endif

    login_query();
    user_login();
    m_init();
    RMSG = NA;
    clear();
    c_recover();
#ifdef TALK_LOG

    tlog_recover();
#endif
    if (strcmp(currentuser.userid, "guest"))
    {
        if (HAS_PERM(PERM_ACCOUNTS) && dashf("register_sjtubbs"))
        {
            struct stat st;

            stat("register_sjtubbs", &st);
            if (st.st_size > 0)
            {
                prints("[0;1m\n\n");
                prints("[33mĿǰ����ʹ���ߵȺ�����������[37m\n\n");
                pressanykey();
            }
        }
        if (check_maxmail())
            pressanykey();
        move(9, 0);
        clrtobot();
        if (!DEFINE(DEF_NOLOGINSEND))
            if (!uinfo.invisible)
                apply_ulist(friend_login_wall);
        clear();
        set_numofsig();
    }

    ActiveBoard_Init();

    fill_date();
    b_closepolls();

    seek_in_qzdenyfile(0); //check deny post
	seek_in_qzdenyfile(1); //check deny base
	
    num_alcounter();

    show_global_vote();
#ifdef REAL_NAME_EMAIL
    ask_real_mail();
#endif
    if (count_friends > 0 && DEFINE(DEF_LOGFRIEND))
        t_friends();
    while (1)
    {
        if (DEFINE(DEF_NORMALSCR))
            domenu("TOPMENU");
        else
            domenu("TOPMENU2");
        Goodbye();
    }
}

int refscreen = NA;

int egetch(void)
{
    int rval;

    check_calltime();
    if (talkrequest)
    {
        talkreply();
        refscreen = YEA;
        return -1;
    }
    while (1)
    {
        rval = igetkey();
        if (talkrequest)
        {
            talkreply();
            refscreen = YEA;
            return -1;
        }
        if (rval != Ctrl('L'))
            break;
        redoscr();
    }
    refscreen = NA;
    return rval;
}

char *boardmargin(void)
{
    static char buf[STRLEN];
    char ch;
    int district;

    district = board_in_which_dist(currboard);
    if (district <= 0)
        ch = '0';
    if (district <= 9)
        ch = district + '0';
    else
        ch = district - 10 + 'A';

    if (selboard)
        sprintf(buf, " %c�� [%s] ", ch, currboard);
    else
    {
        brc_initial(DEFAULTBOARD);
        if (!getbnum(currboard))
            setoboard(currboard);
        selboard = 1;
        sprintf(buf, " %c�� [%s] ", ch, currboard);
    }
    return buf;
}

void update_endline(void)
{
    char buf[255], fname[STRLEN], *ptr;
    time_t now;
    FILE *fp;
    int i, allstay, foo, foo2;

    move(t_lines - 1, 0);
    clrtoeol();

    if (!DEFINE(DEF_ENDLINE))
        return;

    now = time(0);
    allstay = getdatestring(now, YEA);
    if (allstay == 0)
    {
      nowishfile:
        resolve_boards();
        strcpy(datestring, brdshm->date);
        allstay = 1;
    }
    if (allstay < 5)
    {
        allstay = (now - login_start_time) / 60;
        sprintf(buf, "[[36m%.12s[33m]", currentuser.userid);
        num_alcounter();
        prints
            ("[1;44;33m[[36m%29s[33m][[36m%4d[33m��/[1;36m%3d[33m��][[36m%1s%1s%1s%1s%1s%1s[33m]�ʺ�%-24s[[36m%3d[33m:[36m%2d[33m] [m",
             datestring, count_users, count_friends,
             (uinfo.pager & ALL_PAGER) ? "P" : "p",
             (uinfo.pager & FRIEND_PAGER) ? "O" : "o",
             (uinfo.pager & ALLMSG_PAGER) ? "M" : "m",
             (uinfo.pager & FRIENDMSG_PAGER) ? "F" : "f",
             (DEFINE(DEF_MSGGETKEY)) ? "X" : "x",
             (uinfo.invisible == 1) ? "C" : "c", buf,
             (allstay / 60) % 1000, allstay % 60);
        return;
    }
    setuserfile(fname, "HaveNewWish");
    if (WishNum == 9999 || dashf(fname))
    {
        if (WishNum != 9999)
            unlink(fname);
        WishNum = 0;
        orderWish = 0;

        if (is_birth(currentuser))
        {
            strcpy(GoodWish[WishNum],
                   "                     �������������տ���!   �ǵ�Ҫ���Ӵ :P                   ");
            WishNum++;
        }

        setuserfile(fname, "GoodWish");
        if ((fp = fopen(fname, "r")) != NULL)
        {
            for (; WishNum < 20;)
            {
                if (fgets(buf, 255, fp) == NULL)
                    break;
                buf[STRLEN - 4] = '\0';
                ptr = strtok(buf, "\n\r");
                if (ptr == NULL || ptr[0] == '#')
                    continue;
                strcpy(buf, ptr);
                for (ptr = buf; *ptr == ' ' && *ptr != 0; ptr++);
                if (*ptr == 0 || ptr[0] == '#')
                    continue;
                for (i = strlen(ptr) - 1; i < 0; i--)
                    if (ptr[i] != ' ')
                        break;
                if (i < 0)
                    continue;
                foo = strlen(ptr);
                foo2 = (STRLEN - 3 - foo) / 2;
                strcpy(GoodWish[WishNum], "");
                for (i = 0; i < foo2; i++)
                    strcat(GoodWish[WishNum], " ");
                strcat(GoodWish[WishNum], ptr);
                for (i = 0; i < STRLEN - 3 - (foo + foo2); i++)
                    strcat(GoodWish[WishNum], " ");
                GoodWish[WishNum][STRLEN - 4] = '\0';
                WishNum++;
            }
            fclose(fp);
        }
    }
    if (WishNum == 0)
        goto nowishfile;
    if (orderWish >= WishNum * 2)
        orderWish = 0;
    prints("[0;1;44;33m[[36m%s[33m][m", GoodWish[orderWish / 2]);
    orderWish++;
}

void showtitle(char *title, char *mid)
{
    char buf[STRLEN], *note;
    int spc1, spc2;

    note = boardmargin();
    spc1 = 39 + num_ans_chr(title) - strlen(title) - strlen(mid) / 2;
    if (spc1 < 2)
        spc1 = 2;
    spc2 =
        80 - (strlen(title) - num_ans_chr(title) + spc1 + strlen(note) +
              strlen(mid));

    if (spc2 < 1)
        spc2 = 1;
    move(0, 0);
    clrtoeol();
    sprintf(buf, "%*s", spc1, "");
    if (!strcmp(mid, BoardName))
        prints("[1;44;33m%s%s[37m%s[1;44m", title, buf, mid);
    else if (mid[0] == '[')
        prints("[1;44;33m%s%s[5;36m%s[m[1;44m", title, buf, mid);
    else
        prints("[1;44;33m%s%s[36m%s", title, buf, mid);
    sprintf(buf, "%*s", spc2, "");
    prints("%s[33m%s[m\n", buf, note);
    update_endline();
    move(1, 0);
}

void firsttitle(char *title)
{
    char middoc[30];

    if (chkmail())

        strcpy(middoc,
               strstr(title,
                      "�������б�") ? "[�����ż����� L ������]" :
               "[�����ż�]");
    else if (mailXX == 1)
        strcpy(middoc, "[�ż��������������ż�!]");
    else
        strcpy(middoc, BoardName);

    showtitle(title, middoc);
}

void docmdtitle(char *title, char *prompt)
{
    firsttitle(title);
    move(1, 0);
    clrtoeol();
    prints("%s", prompt);
    clrtoeol();
}

void c_recover(void)
{
    char fname[STRLEN];
    struct stat st;

    sprintf(fname, "home/%c/%c/%s/%s.deadve",
            toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
            currentuser.userid, currentuser.userid);
    if (!dashf(fname) || strcmp(currentuser.userid, "guest") == 0)
        return;
    clear();
    strcpy(genbuf, "");

    getdata(0, 0,
            "[1;32m����һ���༭��ҵ�������жϣ� (M) �Ļ����� (Q) ���ˣ�[M]��[m",
            genbuf, 2, DOECHO, YEA);
    switch (genbuf[0])
    {
    case 'Q':
    case 'q':
        unlink(fname);
        break;

    default:
        stat(fname, &st);
        if (st.st_size > 65536 * 10)
        {
            if (getmailsize(currentuser.userid) + st.st_size >
                (getmailboxsize(currentuser.userid, currentuser.userlevel)
                 * 2))
            {
                move(5, 0);
                prints("�����������޷��Ļأ�");
                pressanykey();
                unlink(fname);
                return;
            }
        }
        mail_file(fname, currentuser.userid, "�����������������Ĳ���...");
        unlink(fname);
        break;
    }
}

#ifdef TALK_LOG
void tlog_recover(void)
{
    char buf[256];

    sprintf(buf, "home/%c/%c/%s/talk_log", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);

    if (strcasecmp(currentuser.userid, "guest") == 0 || !dashf(buf))
        return;

    clear();
    strcpy(genbuf, "");
    getdata(0, 0,
            "[1;32m����һ���������������������������¼, ��Ҫ .. (M) �Ļ����� (Q) ���ˣ�[Q]��[m",
            genbuf, 2, DOECHO, YEA);

    if (genbuf[0] == 'M' || genbuf[0] == 'm')
    {
        mail_file(buf, currentuser.userid, "�����¼");
    }
    unlink(buf);
    return;
}
#endif
