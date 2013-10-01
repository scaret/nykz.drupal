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
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

/*
$Id: main.c,v 1.3 2004/07/17 10:28:30 jawahh Exp $
*/
#include "bbs.h"

//#ifndef DLM
//#undef  ALLOWGAME
//#else 
long int bank;
int NowInGame = NA;
//#endif

#define BADLOGINFILE    "logins.bad"
#define VISITLOG	BBSHOME"/.visitlog"

#define MAX_ID_FOR_REALMAIL 3 /* each real mail can be used for 3 ids */

int ERROR_READ_SYSTEM_FILE = NA;
int RMSG = YEA;
int msg_num = 0;
int count_friends = 0, count_users = 0;
int iscolor = 1;
int mailXX = 0;
char *getenv ();
int friend_login_wall ();
char *sysconf_str ();
struct user_info *t_search ();
void r_msg ();
void count_msg ();
void c_recover ();
void tlog_recover ();
void refreshdate ();
int listmode;
int numofsig = 0;
jmp_buf byebye;
extern struct BCACHE *brdshm;
extern struct UTMPFILE *utmpshm;
FILE *ufp;
int talkrequest = NA;

/* int ntalkrequest = NA ; */
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
extern void resolve_GbBig5Files ();
#endif

void
log_usies (mode, mesg)
char *mode, *mesg;
{
    time_t now;
    char buf[256], *fmt;

    now = time (0);
    fmt = currentuser.userid[0] ? "%s %s %-12s %s\n" : "%s %s %s%s\n";
    getdatestring (now, NA);
    sprintf (buf, fmt, datestring, mode, currentuser.userid, mesg);
    file_append ("usies", buf);
}

void
u_enter ()
{
    enter_uflags = currentuser.flags[0];
    memset (&uinfo, 0, sizeof (uinfo));
    uinfo.active = YEA;
    uinfo.pid = getpid ();
    /* 使非 SYSOP 权限 ID 登陆时自动恢复到非隐身状态 */
    if (!HAS_PERM (PERM_CLOAK))
        currentuser.flags[0] &= ~CLOAK_FLAG;
    if (HAS_PERM (PERM_LOGINCLOAK) && (currentuser.flags[0] & CLOAK_FLAG))
        uinfo.invisible = YEA;
    uinfo.mode = LOGIN;
    uinfo.pager = 0;
#ifdef BBSD

    uinfo.idle_time = time (0);
#endif

    if (DEFINE (DEF_DELDBLCHAR))
        enabledbchar = 1;
    else
        enabledbchar = 0;
    if (DEFINE (DEF_FRIENDCALL))
    {
        uinfo.pager |= FRIEND_PAGER;
    }
    if (currentuser.flags[0] & PAGER_FLAG)
    {
        uinfo.pager |= ALL_PAGER;
        uinfo.pager |= FRIEND_PAGER;
    }
    if (DEFINE (DEF_FRIENDMSG))
    {
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    if (DEFINE (DEF_ALLMSG))
    {
        uinfo.pager |= ALLMSG_PAGER;
        uinfo.pager |= FRIENDMSG_PAGER;
    }
    uinfo.uid = usernum;
    strncpy (uinfo.from, fromhost, 60);

   if (!DEFINE(DEF_NOTHIDEIP)) 
   {
      uinfo.from[22] = 'H';
   }   
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

    strncpy (uinfo.tty, tty_name, 20);
#endif

    iscolor = (DEFINE (DEF_COLOR)) ? 1 : 0;
    strncpy (uinfo.userid, currentuser.userid, 20);
    strncpy (uinfo.realname, currentuser.realname, 20);
    strncpy (uinfo.username, currentuser.username, NAMELEN);
    getfriendstr ();
    getrejectstr ();
    if (HAS_PERM (PERM_EXT_IDLE))
        uinfo.ext_idle = YEA;

    listmode = 0;			/* 借用一下, 用来纪录到底 utmpent 卡位失败几次 */
    while (1)
    {
        utmpent = getnewutmpent (&uinfo);
        if (utmpent >= 0 || utmpent == -1)
            break;
        if (utmpent == -2 && listmode <= 100)
        {
            listmode++;
            usleep (250);		/* 休息四分之一秒再接在励 */
            continue;
        }
        if (listmode > 100)
        {			/* 放弃吧 */
            sprintf (genbuf, "getnewutmpent(): too much times, give up.");
            report (genbuf);
            prints ("getnewutmpent(): 失败太多次, 放弃. 请回报站长.\n");
            sleep (3);
            exit (0);
        }
    }
    if (utmpent < 0)
    {
        sprintf (genbuf, "Fault: No utmpent slot for %s\n", uinfo.userid);
        report (genbuf);
    }
    listmode = 0;
    digestmode = NA;
}

void
setflags (mask, value)
int mask, value;
{
    if (((currentuser.flags[0] & mask) && 1) != value)
    {
        if (value)
            currentuser.flags[0] |= mask;
        else
            currentuser.flags[0] &= ~mask;
    }
}
/*
user: the user_info[num] member  in utmpshm struct
*/
int
kick_web_user(struct user_info *user)
{
	int stay = 0;
	FILE *fp;
	struct userec u;
	if (!user->active)
		return -1;
	stay =abs(time(0)-*(int *) (user->from + 32));
	if(stay > 3600*24*3)
		stay = 3600*24*3;
	else if(stay < 0)
		stay = 0;
    
	fp = fopen(".PASSWDS","r+");
	fseek(fp,(user->uid-1)*sizeof(struct userec),SEEK_SET);
	fread(&u,sizeof(struct userec),1,fp);
	if( strcasecmp(u.userid,user->userid) )
	{
		fclose(fp);
		memset(user,0,sizeof(struct user_info));
		return -2;
	}
	u.stay += stay;
	u.lastlogout = time(0);
	fseek(fp,-sizeof(struct userec),SEEK_CUR);
	fwrite(&u,sizeof(struct userec),1,fp);
	fclose(fp);
	memset(user,0,sizeof(struct user_info));
	return 0;
}

void
u_exit ()
{
    int fd;

    /*这些信号的处理要关掉, 否则在离线时等候回车时出现  (ylsdd)
          信号会导致重写名单, 这个导致的名单混乱比kick user更多*/
    signal (SIGHUP, SIG_DFL);
    signal (SIGALRM, SIG_DFL);
    signal (SIGPIPE, SIG_DFL);
    signal (SIGTERM, SIG_DFL);
    signal (SIGUSR1, SIG_IGN);
    signal (SIGUSR2, SIG_IGN);

    setflags (PAGER_FLAG, (uinfo.pager & ALL_PAGER));
    if (HAS_PERM (PERM_LOGINCLOAK))
        setflags (CLOAK_FLAG, uinfo.invisible);

    if (!ERROR_READ_SYSTEM_FILE)
    {
        time_t stay;

        set_safe_record ();
        stay = time (0) - login_start_time;
        currentuser.stay += stay;
        currentuser.lastlogout = time (0);
        substitute_record (PASSFILE, &currentuser, sizeof (currentuser),
                           usernum);
    }

    uinfo.invisible = YEA;
    uinfo.sockactive = NA;
    uinfo.sockaddr = 0;
    uinfo.destuid = 0;
#if !defined(BBSD) && defined(SHOW_IDLE_TIME)

    strcpy (uinfo.tty, "NoTTY");
#endif

    uinfo.pid = 0;
    uinfo.active = NA;
    //update_utmp();
    if ((fd = open (ULIST, O_RDWR | O_CREAT, 0600)) > 0)
    {
        flock (fd, LOCK_EX);
    }
    update_utmp ();
    if (fd > 0)
    {
        flock (fd, LOCK_UN);
        close (fd);
    }
    utmpent = -1;			// add by quickmouse to prevent supercloak
}

int
cmpuids (uid, up)
char *uid;
struct userec *up;
{
    return !ci_strncmp (uid, up->userid, sizeof (up->userid));
}

int
dosearchuser (userid)
char *userid;
{
    int id;

    if ((id = getuser (userid)) != 0)
    {
        if (cmpuids (userid, &lookupuser))
        {
            memcpy (&currentuser, &lookupuser, sizeof (currentuser));
            return usernum = id;
        }
    }
    memset (&currentuser, 0, sizeof (currentuser));
    return usernum = 0;
}


void
talk_request ()
{
    signal (SIGUSR1, talk_request);
    talkrequest = YEA;
    bell ();
    bell ();
    bell ();
    sleep (1);
    bell ();
    bell ();
    bell ();
    bell ();
    bell ();
    return;
}

void
abort_bbs ()
{
    extern int child_pid;
    int i;
#ifdef ALLOWGAME   
   char inGame[STRLEN];
   if(NowInGame==YEA){
      setuserfile(inGame,"inGame");
      if(dashf(inGame)) unlink(inGame);
   }   
#endif

    if (child_pid)
        kill (child_pid, 9);
    if (uinfo.mode == POSTING || uinfo.mode == SMAIL || uinfo.mode == EDIT
            || uinfo.mode == EDITUFILE || uinfo.mode == EDITSFILE
            || uinfo.mode == EDITANN)
        keep_fail_post ();
    /*if (started)
      {
        time_t stay;

        stay = time (0) - login_start_time;
        sprintf (genbuf, "Stay: %3ld (%s)", stay / 60, currentuser.username);
        log_usies ("AXXED", genbuf);
        u_exit ();
      }*/
    if (started)
    {
        time_t stay;
        stay = time(0) - login_start_time;
        sprintf(genbuf, "Stay: %3ld (%s)", stay / 60, currentuser.username);

        /* inboard user count  by ZV 2002.12.13 */
        if ((uinfo.mode == READING) || (uinfo.mode == POSTING) ||
                (uinfo.mode == EDIT))
        {
            for (i=0; i<MAXBOARD; i++)
                if (!strcmp(brdshm->bcache[i].filename, currboard))
                {
                    if (brdshm->inboard[i]>0)
                        brdshm->inboard[i]--;
                    break;
                }
        }

        log_usies("AXXED", genbuf);
        u_exit();
    }
    exit (0);
}

int
cmpuids2 (unum, urec)
int unum;
struct user_info *urec;
{
    return (unum == urec->uid);
}

int
count_multi (uentp)
struct user_info *uentp;
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

int
count_user ()
{
    count_multi (NULL);
    apply_ulist (count_multi);
    return count_multi (NULL);
}

void
multi_user_check ()
{
    struct user_info uin;
    int logins, mustkick = 0,pos;

    if (HAS_PERM (PERM_MULTILOG))
        return;			/* don't check sysops */

#ifdef SPECIALID

    if (strcmp (currentuser.userid, SPECIALID1) == 0
            || strcmp (currentuser.userid, SPECIALID2) == 0)
        return;
#endif
    /* allow multiple guest user */
    logins = count_user ();

    if (heavyload () && logins)
    {
        prints ("[1;33m抱歉, 目前系统负荷过重, 请勿重复 Login。[m\n");
        oflush ();
        sleep (3);
        exit (1);
    }
    if (!strcasecmp ("guest", currentuser.userid))
    {
        if (logins > MAXGUEST)
        {
            prints
            ("[1;33m抱歉, 目前已有太多 [1;36mguest[33m, 请稍后再试。[m\n");
            oflush ();
            sleep (3);
            exit (1);
        }
        return;
    }
    else if (logins >= MULTI_LOGINS)
    {
        prints
        ("[1;32m为确保他人上站权益, 本站仅允许您用该帐号登陆 %d 个。\n[0m",
         MULTI_LOGINS);
        prints
        ("[1;36m您目前已经使用该帐号登陆了 %d 个，您必须断开其他的连接方能进入本站！\n[0m",
         logins);
        mustkick = 1;
    }
    if (!(pos=search_ulist (&uin, cmpuids2, usernum)))
        return;			/* user isn't logged in */

    if (!uin.active || (uin.pid && kill (uin.pid, 0) == -1))
        return;			/* stale entry in utmp file */
    getdata (0, 0, "[1;37m您想删除重复的 login 吗 (Y/N)? [N][m",
             genbuf, 4, DOECHO, YEA);
    if (genbuf[0] == 'N' || genbuf[0] == 'n' || genbuf[0] == '\0')
    {
        if (!mustkick)
            return;
        prints
        ("[33m很抱歉，您已经用该帐号登陆 %d 个，所以，此连线将被取消。[m\n",
         logins);
        oflush ();
        sleep (3);
        exit (1);
    }
    else
    {
        if (!uin.pid)
            return;
		/*web用户没有单独的进程,要单独kick */
		if( is_web_user(&uin) )
			kick_web_user(&(utmpshm->uinfo[pos-1]));
		else
	        kill (uin.pid, SIGHUP);
        //以前不是SIGHUP，会导致编辑作业丢失 by sunner
        report ("kicked (multi-login)");
        log_usies ("KICK ", currentuser.username);
    }
}

#ifndef BBSD
void
system_init (argc, argv)
int argc;
char **argv;
#else
void
system_init ()
#endif
{
#ifndef BBSD
    char *rhost;
#endif

    struct sigaction act;

    gethostname (genbuf, 256);
    sprintf (ULIST, "%s.%s", ULIST_BASE, genbuf);

#ifndef BBSD

    if (argc >= 3)
    {
        strncpy (fromhost, argv[2], 60);
    }
    else
    {
        fromhost[0] = '\0';
    }
    if ((rhost = getenv ("REMOTEHOST")) != NULL)
        strncpy (fromhost, rhost, 60);
    fromhost[59] = '\0';
#if defined(SHOW_IDLE_TIME)

    if (argc >= 4)
    {
        strncpy (tty_name, argv[3], 20);
    }
    else
    {
        tty_name[0] = '\0';
    }
#endif
#endif

#ifndef lint
    signal (SIGHUP, abort_bbs);
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGPIPE, SIG_IGN);
#ifdef DOTIMEOUT

    init_alarm ();
    uinfo.mode = LOGIN;
    alarm (LOGIN_TIMEOUT);
#else

    signal (SIGALRM, SIG_SIG);
#endif

    signal (SIGTERM, SIG_IGN);
    signal (SIGURG, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
#endif

    signal (SIGUSR1, talk_request);

    sigemptyset (&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    act.sa_handler = r_msg;
    sigaction (SIGUSR2, &act, NULL);
}

void
system_abort ()
{
    if (started)
    {
        log_usies ("ABORT", currentuser.username);
        u_exit ();
    }
    clear ();
    refresh ();
    prints ("谢谢光临, 记得常来喔 !\n");
    exit (0);
}

void
logattempt (uid, frm)
char *uid, *frm;
{
    char fname[STRLEN];

    getdatestring (time (0), NA);
    sprintf (genbuf, "%-12.12s  %-30s %s\n", uid, datestring, frm);
    file_append (BADLOGINFILE, genbuf);
    sethomefile (fname, uid, BADLOGINFILE);
    file_append (fname, genbuf);

}

int
check_tty_lines ()
{				/* dii.nju.edu.cn  zhch  2000.4.11 */
    static unsigned char buf1[] = { 255, 253, 31 };
    unsigned char buf2[100];
    int n;

    if (ttyname (0))
        return;
    write (0, buf1, 3);
    n = read (0, buf2, 80);
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
}
max_log;
void
visitlog (void)
{
    int vfp;
    time_t now;
    struct tm *tm;
    extern struct UTMPFILE *utmpshm;

    vfp = open (VISITLOG, O_RDWR | O_CREAT, 0644);
    if (vfp == -1)
    {
        report ("Can NOT write visit Log to .visitlog");
        return;
    }
    flock (vfp, LOCK_EX);
    lseek (vfp, (off_t) 0, SEEK_SET);
    read (vfp, &max_log, (size_t) sizeof (max_log));
    if (max_log.year < 1990 || max_log.year > 2020)
    {
        now = time (0);
        tm = localtime (&now);
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
    lseek (vfp, (off_t) 0, SEEK_SET);
    write (vfp, &max_log, (size_t) sizeof (max_log));
    flock (vfp, LOCK_UN);
    close (vfp);
    sprintf (genbuf,
             "[1;32m从 [[36m%4d年%2d月%2d日[32m] 起, 最高人数记录: [[36m%d[32m] 累计访问人次: [[36m%u[32m][m\n",
             max_log.year, max_log.month, max_log.day, max_log.logins,
             max_log.visit);
    prints (genbuf);
}

void
login_query ()
{
    char uid[IDLEN + 2];
    char passbuf[PASSLEN];
    int curr_login_num;
    int attempts;
    char genbuf[STRLEN];
    char *ptr;
    extern struct UTMPFILE *utmpshm;

    curr_login_num = num_active_users ();
    if (curr_login_num >= MAXACTIVE)
    {
        ansimore ("etc/loginfull", NA);
        oflush ();
        sleep (1);
        exit (1);
    }
#ifdef BBSNAME
    strcpy (BoardName, BBSNAME);
#else

    ptr = sysconf_str ("BBSNAME");
    if (ptr == NULL)
        ptr = "尚未命名测试站";
    strcpy (BoardName, ptr);
#endif

    if (fill_shmfile (1, "etc/issue", "ISSUE_SHMKEY"))
    {
        show_issue ();		/* is anibanner ready, remark this and put * \n\n */
    }
    prints
    ("[1;35m欢迎光临[1;40;33m【 %s 】 [m[[0;1;33;41m Add '.' after YourID to login for BIG5 [m]\n[0;1;32m本站可注册帐号数: [[1;36m%d[0;1;32m] [m",
     BoardName, MAXUSERS);
    resolve_utmp ();
    if (utmpshm->usersum == 0)
        utmpshm->usersum = allusers ();
    if (utmpshm->max_login_num < curr_login_num)
        utmpshm->max_login_num = curr_login_num;
    prints
    ("[1;32m目前已有帐号数: [[1;36m%d[32m] [1;32m目前上站人数: [[1;36m%d[1;32m/[1;36m%d[1;32m] \n",
     utmpshm->usersum, curr_login_num, MAXACTIVE);
    //    utmpshm->usersum, curr_login_num-CountCloakMan(), MAXACTIVE);
    visitlog ();

#ifdef MUDCHECK_BEFORELOGIN

    prints ("[1;33m为防止使用程式上站，请按 [1;36mCTRL + C[m : ");
    genbuf[0] = igetkey ();
    if (genbuf[0] != Ctrl ('C'))
    {
        prints ("\n对不起，你并没有按下 CTRL+C 键！\n");
        oflush ();
        exit (1);
    }
    else
    {
        prints ("[CTRL] + [C]\n");
    }
#endif

    attempts = 0;
    while (1)
    {
        if (attempts++ >= LOGINATTEMPTS)
        {
            ansimore ("etc/goodbye", NA);
            oflush ();
            sleep (1);
            exit (1);
        }
        prints("[1;33m请输入帐号[m(试用请输入 `[1;36mguest[m', 注册请输入`[1;31mnew[m')");
        getdata (0, 0,": ",uid,IDLEN +1,DOECHO,YEA);
#ifdef ALLOWSWITCHCODE

        ptr = strchr (uid, '.');
        if (ptr)
        {
            convcode = 1;
            *ptr = '\0';
        }
#endif
        if ((strcasecmp (uid, "guest") == 0)
                && (MAXACTIVE - curr_login_num < 10))
        {
            ansimore ("etc/loginfull", NA);
            oflush ();
            sleep (1);
            exit (1);
        }
        if (strcasecmp (uid, "new") == 0)
        {
#ifdef LOGINASNEW
            memset (&currentuser, 0, sizeof (currentuser));
            new_register ();
            ansimore3 ("etc/firstlogin", YEA);
            break;
#else

            prints
            ("[1;37m本系统目前无法以 [36mnew[37m 注册, 请用[36m guest[37m 进入...[m\n");
#endif

        }
        else if (*uid == '\0')
            ;
        else if (!dosearchuser (uid))
        {
            prints ("[1;31m经查证，无此 ID (User ID Error)...[m\n");
        }
        else if (strcasecmp (uid, "guest") == 0)
        {
            currentuser.userlevel = 0;
            break;
#ifdef SYSOPLOGINPROTECT

        }
        else if (!strcasecmp (uid, "SYSOP")
                 && strcmp (fromhost, "localhost")
                 && strcmp (fromhost, "127.0.0.1"))
        {
            prints ("[1;32m 警告: 从 %s 登录是非法的! 请勿再试![m\n",
                    fromhost);
            prints
            ("[注意] 为安全起见，本站已经设定 SYSOP 只能从主机登陆。\n       如果您确实是本站的 SYSOP ，请登陆到本 BBS 服务器，然后: \n              telnet localhost port.\n");
            oflush ();
            sleep (1);
            exit (1);
#endif

        }
        else
        {
#ifdef ALLOWSWITCHCODE
            if (!convcode)
                convcode = !(currentuser.userdefine & DEF_USEGB);
#endif

            getdata (0, 0, "[1;37m请输入密码: [m", passbuf, PASSLEN,
                     NOECHO, YEA);
            passbuf[8] = '\0';

            if (!checkpasswd (currentuser.passwd, passbuf))
            {
                logattempt (currentuser.userid, fromhost);
                prints ("[1;31m密码输入错误 (Password Error)...[m\n");
            }
            else
            {
#ifdef SPECIALID
                if (strcmp (uid, SPECIALID1) == 0)
                {
                    ansimore ("etc/specialwelcomeboy", NA);
                    prints ("请输入您的昵称 [我是新生boy] ：");
                    getdata (0, 0, "", currentuser.username, NAMELEN,
                             DOECHO, YEA);
                    if (strlen (currentuser.username) == 0)
                        strcpy (currentuser.username, "我是新生boy");
                }
                else if (strcmp (uid, SPECIALID2) == 0)
                {
                    ansimore ("etc/specialwelcomegirl", NA);
                    prints ("请输入您的昵称 [我是新生girl] ：");
                    getdata (0, 0, "", currentuser.username, NAMELEN,
                             DOECHO, YEA);
                    if (strlen (currentuser.username) == 0)
                        strcpy (currentuser.username, "我是新生girl");
                }
                else
                {
#endif
				if (!HAS_PERM (PERM_BASIC) && !HAS_PERM (PERM_SYSOP))
                    {
                        prints
                        ("[1;32m本帐号已停机。请向 [36mSYSOP[32m 查询原因[m\n");
                        oflush ();
                        sleep (1);
                        exit (1);
                    }
#ifdef CHECK_LESS60SEC
                    if (abs (time (0) - currentuser.lastlogin) < 60
                            && !HAS_PERM (PERM_SYSOP)
                            && strcasecmp (currentuser.userid, "guest") != 0)
                    {
                        prints
                        ("您在 60 秒内重复上站.请按 [1;36mCtrl+C[m 键确认您的身份: ");
                        genbuf[0] = igetkey ();
                        if (genbuf[0] != Ctrl ('C'))
                        {
                            prints
                            ("\n对不起，您并没有按下 CTRL+C 键！您不能进入本站\n");
                            prints ("若有疑问请通知站务人员, 谢谢.\n");
                            oflush ();
                            sleep (3);
                            exit (1);
                        }
                        prints ("\n");
                    }
#endif
#ifdef SPECIALID

                }
#endif

                /*
                			      if (HAS_PERM (PERM_RECLUSION) && !HAS_PERM (PERM_SYSOP))
                				{		// add byquickmouse
                				  attempts = (time (0) - currentuser.lastlogin) / 86400;
                				  if (attempts < 30)
                				    {
                				      prints
                					("\033[1;32m用户已经归隐田园%d天了，30天内不会回来\033[m\n",
                					 attempts);
                				      oflush ();
                				      sleep (1);
                				      exit (1);
                				    }
                				  else
                				    {
                				      prints (";32m欢迎您结束隐居生活，再次回到这个大家庭\n");
                				      currentuser.userlevel =
                					currentuser.userlevel & (~PERM_RECLUSION);
                				      substitute_record (PASSFILE, &currentuser,
                							 sizeof (currentuser), usernum);
                				      pressanykey ();
                				    }
                				}*/

#ifdef CHECK_SYSTEM_PASS
                if (HAS_PERM (PERM_SYSOP))
                {
                    if (!check_systempasswd ())
                    {
                        prints ("\n口令错误, 不得签入 ! !\n");
                        oflush ();
                        sleep (2);
                        exit (1);
                    }
                }
#endif
                bzero (passbuf, PASSLEN - 1);
                break;
            }
        }
    }
    multi_user_check ();
    if (!term_init (currentuser.termtype))
    {
        prints ("Bad terminal type.  Defaulting to 'vt100'\n");
        strcpy (currentuser.termtype, "vt100");
        term_init (currentuser.termtype);
    }
    check_tty_lines ();		/* 2000.03.14 */
    sethomepath (genbuf, currentuser.userid);
    mkdir (genbuf, 0755);
    login_start_time = time (0);
}

#ifdef REAL_NAME_EMAIL
int
check_email_id(const char *id)
/* invalid email id must be in range of 0-9a-zA-Z._ 
 * RETURN: 0 for valid, 1 for invalid
 */
{
	int i;
	for(i=0;id[i];i++)
		if(!(isalnum(id[i])||id[i]=='.'||id[i]=='_'||id[i]=='-'))
			return 1;
	return 0;
}
int
check_email_domain(const char *domain)
/* the valid email domian is *.edu.cn or *.univs.cn
 * but not include mail.edu.cn which is a free mail system
 * RETURN: 0 for valid, 1 for invalid
 */
{
	const char *eduDomain = ".edu.cn";
	const char *univsDomain = ".univs.cn";
	int i;
	for(i=0;domain[i];i++)
		if(!(isalnum(domain[i])||domain[i]=='.'||domain[i]=='-'))
			return 1;
	if(!strcmp(domain,"mail.edu.cn"))
		return 1;
	if(!strncmp(domain,"bbs.",4))
		return 1;
	if(!strcmp(domain,"univs.cn"))
		return 0;
	if(!strcmp(domain, "sjtu.org"))
		return 0;
	if(strlen(eduDomain) < strlen(domain))
		if(!strcmp( domain + strlen(domain) - strlen(eduDomain),eduDomain))
			return 0;
	if(strlen(univsDomain) < strlen(domain))
		if(!strcmp( domain + strlen(domain) - strlen(univsDomain),univsDomain))
			return 0;
	return 1;
}
int 
check_real_name_email(const char *email)
/* the valid email address is id@*.edu.cn or id@*.univs.cn
 * but not include mail.edu.cn which is a free mail system
 * RETURN: 0 for a valid email, 1 for a invalid one
 */
{
	char *pat,*domain,buf[REAL_NAME_EMAIL_LEN+1];
	strcpy(buf,email);
	pat = strchr(buf,'@');
	if(!pat)
		return 1;
	*pat = '\0';
	domain = pat + 1;
	if(check_email_id(buf)||check_email_domain(domain))
		return 1;
	else 
		return 0;
}
int
get_real_name_email(char *buf)
{
	char path[128];
	int fd;
	ssize_t readnum;
	sethomefile(path,currentuser.userid,REAL_NAME_FILE);
	if( (fd = open(path,O_RDONLY)) == -1)
	{
		buf[0] = '\0';
		return;
	}
	readnum = read(fd,buf,REAL_NAME_EMAIL_LEN);
	close(fd);
	buf[readnum] = '\0';
	return;
}
int
write_real_name_email(const char *email)
{
	char path[128];
	FILE *file;
	sethomefile(path,currentuser.userid,REAL_NAME_FILE);
	if( (file = fopen(path,"w")) == NULL)
	{
		return 1;
	}
	fprintf(file,email);
	fclose(file);
	return 0;
}
int
get_random(unsigned int buf[],int size)
{
	int fd;
	if( (fd=open("/dev/urandom",O_RDONLY)) != -1)
	{
		read(fd,buf,sizeof(unsigned int)*size);
		close(fd);
	}
	else
	{
		int i;
		for(i=0;i<size;i++)
			buf[i]=random();
	}
	return 0;
}
int
mailto_real_name_mail(char *userid)
{
	char email[REAL_NAME_EMAIL_LEN+1],fname[256],code_fname[256];
	const char title[] = BBSNAME"注册确认信";
	unsigned int rand[4],return_no;
	FILE *content,*code;
	get_real_name_email(email);
	if(!email[0])
		return 1;
	get_random(rand,4);
	sprintf(fname,"tmp/check_mail.%s",userid);
	content = fopen(fname,"w");
	if(content == NULL)
		return 2;

/*	fprintf(content,"\n\
    你好！这是%s(%s)注册确认信。\n\
    如果你曾在%s注册ID %s,请用这个的链接完成注册：\n\
    http://%s/bbsdochkmail?id=%s&code=%08x%08x%08x%08x\n\
    如果你没有注册过此ID，请删除本信。我们为此信的打扰感到歉意。\n\n\n\
                                                  %s站务组\n",
		BBSNAME,BBSHOST,BBSNAME,userid,BBSHOST,userid,rand[0],rand[1],rand[2],rand[3],BBSNAME);
*/
	fprintf(content,"\n\
    你好！这是 %s (%s) 注册确认信。\n\
	你的ID %s 的认证码是 %08x\
	请在下次登录 %s 时使用此认证码激活你的帐号。\
    如果你没有注册过此ID，请删除本信。我们为此信的打扰感到歉意。\n\n\n\
                                                  %s站务组\n",
		BBSNAME,BBSHOST,userid, rand[0], BBSNAME,BBSNAME);


	fclose(content);
	sethomefile(code_fname,userid,REAL_NAME_CHK_CODE);
	code = fopen(code_fname,"w");
	if(code == NULL)
	{
		unlink(fname);
		return 3;
	}
	fprintf(code,"%08x",rand[0]);
	fclose(code);
	return_no =  bbs_sendmail(fname,title,email,NA,NA);
	unlink(fname);
	return return_no;
}

int check_realmail_ns(char* buf)
{
	char filepath[STRLEN];
	char buffer[STRLEN];
	FILE* fp = NULL;
	int count = 0;
	sprintf(filepath, "realmail/%s", buf);
	fp = fopen(filepath, "r");
	if(fp == NULL)
		return 0;
	
	count = 0;
	while(fgets(buffer, STRLEN, fp)!= NULL)
	{
		count++;
	}
	return count;
}


verify_real_mail()
{
	char code_fname[STRLEN];
	char tempbuf[10] = {0};
	char realbuf[10] = {0};
	FILE* fp = NULL;
	int i=0;
	int id;
	FILE* fp2 = NULL;
	FILE* fp3=NULL;
	char mail_fname[STRLEN] = {0};
	char mail_buf[STRLEN] = {0};
	char mail_status_fname[STRLEN];
	FILE* fp4 = NULL;
	//FILE* fp3=NULL;
	char real_mail[STRLEN] = {0};
	//char realmail_status[STRLEN];
	int has_registered_ns = 0;
	char realmail_fname[STRLEN];

	
	if(HAS_PERM(PERM_REALNAME)) return;
	sethomefile(realmail_fname, currentuser.userid, REAL_NAME_FILE);

	/* real mail */
	sethomefile(code_fname,currentuser.userid, REAL_NAME_CHK_CODE);

	if(dashf(code_fname))
	{
		/* real_mail使用情况 */
		fp4= fopen(realmail_fname, "r");
		if(fp4!=NULL)
		{
			fgets(real_mail, STRLEN, fp4);
			fclose(fp4);
			has_registered_ns = check_realmail_ns(real_mail);
			if(has_registered_ns >= MAX_ID_FOR_REALMAIL)
			{
				unlink(code_fname);
				move(10, 0);
				prints("%s\n已经绑定3个或3个以上ID，不能继续绑定新的帐号! \n若有疑问请联系技术站长。", real_mail);
				pressreturn();
				return;
			}
		}
		fp = fopen(code_fname, "r");
		if(fp!= NULL)
		{
			fread(realbuf, 8, 1, fp);
			i=0;
			while(i<3)
			{
				clear();
				getdata(10, 0, "请输入认证码(8字符长度，字母全部小写): ", tempbuf, 9, DOECHO, YEA);
				if(!strncmp(realbuf, tempbuf, 8))
				{
					currentuser.userlevel |= PERM_REALNAME;
					if(!HAS_PERM(PERM_LOGINOK))
					{
						currentuser.userlevel |= (PERM_POST | PERM_PAGE | PERM_CHAT | PERM_MESSAGE | PERM_LOGINOK);
					}
					id = getuser (currentuser.userid);
					substitute_record (PASSFILE, &currentuser, sizeof (struct userec), id);
					prints("认证成功!");
					fclose(fp);
					unlink(code_fname);
					sethomefile(mail_fname,currentuser.userid, REAL_NAME_FILE);
					fp2 = fopen(mail_fname, "r");
					if(fp2 != NULL)
					{
						fgets(mail_buf, STRLEN, fp2);
						sprintf(mail_status_fname, "realmail/%s", mail_buf);
						fclose(fp2);
						fp3 = fopen(mail_status_fname, "a");
						if(fp3!= NULL)
						{
							fwrite(currentuser.userid, strlen(currentuser.userid), 1, fp3);
							fwrite("\n",1, 1, fp3);
							fclose(fp3);
						}
					}
					pressanykey();
					return;
				}
				i++;
			}
			move(12, 0);
			prints("认证失败，请重新绑定Email!");
			fclose(fp);
			unlink(code_fname);
			pressanykey();
		}
	}
	return;
}



void
ask_real_mail()
{
	int mail_has_used = 0;
	char email[REAL_NAME_EMAIL_LEN+1],genbuf[128],buf[REAL_NAME_EMAIL_LEN+1];
	if(HAS_PERM(PERM_REALNAME)) return;

/* 重新绑定Email */
	
	clear();
	move(0,0);
	prints("你的id尚未绑定邮箱，未绑定邮箱的账号不能发文。\n\
请绑定本站可接受的信箱。\n\
具体情况请到 BBSHelp 板查询，谢谢。\n");
	if(askyn("现在要绑定吗",YEA,YEA)==NA)
		return;
	get_real_name_email(email);
	buf[0] = '\0';
	sprintf(genbuf,"请输入邮箱[%s] :",email);
	while(check_real_name_email(buf))
	{
		clear();
		getdata(1,0,genbuf,buf,REAL_NAME_EMAIL_LEN,DOECHO,YEA);
		if( askyn("输入正确吗 ",YEA,NA))
		{
			if(strlen(buf)==0) 
				strcpy(buf,email);
		}
		else
			buf[0]='\0';
	}
	mail_has_used = check_realmail_ns(buf);
	if(mail_has_used>=MAX_ID_FOR_REALMAIL)
	{
		prints("此信箱已经绑定三个或三个以上ID，不能继续绑定！\n");
		prints("如有疑问请在SYSOP板面提出或者发信联系技术站长。");
		pressanykey();
		return;
	}
	write_real_name_email(buf);
	if(mailto_real_name_mail(currentuser.userid))
		prints("发送确认邮件失败");
	else
		prints("确认邮件已发送，请查收");
	pressanykey();
	return;
	
}
#endif
int
valid_ident (ident)
char *ident;
{
    static char *invalid[] = { "unknown@", "root@", "gopher@", "bbs@",
                               "guest@", "nobody@", "www@", NULL
                             };
    int i;

    if (ident[0] == '@')
        return 0;
    for (i = 0; invalid[i] != NULL; i++)
        if (strstr (ident, invalid[i]) != NULL)
            return 0;
    return 1;
}

void
write_defnotepad ()
{
    currentuser.notedate = time (0);
    set_safe_record ();
    substitute_record (PASSFILE, &currentuser, sizeof (currentuser), usernum);
    return;
}

void
notepad_init ()
{
    FILE *check;
    char notetitle[STRLEN];
    char tmp[STRLEN * 2];
    char *fname, *bname, *ntitle;
    long int maxsec;
    time_t now;

    maxsec = 86400;
    lastnote = 0;
    if ((check = fopen ("etc/checknotepad", "r")) != NULL)
    {
        fgets (tmp, sizeof (tmp), check);
        lastnote = atol (tmp);
        fclose (check);
    }
    now = time (0);
    if ((now - lastnote) >= maxsec)
    {
        move (t_lines - 1, 0);
        prints ("对不起，系统自动发信，请稍候.....");
        refresh ();
        check = fopen ("etc/checknotepad", "w");
        lastnote = now - (now % maxsec);
        fprintf (check, "%d", lastnote);
        fclose (check);
        if ((check = fopen ("etc/autopost", "r")) != NULL)
        {
            while (fgets (tmp, STRLEN, check) != NULL)
            {
                fname = strtok (tmp, " \n\t:@");
                bname = strtok (NULL, " \n\t:@");
                ntitle = strtok (NULL, " \n\t:@");
                if (fname == NULL || bname == NULL || ntitle == NULL)
                    continue;
                else
                {
                    getdatestring (now, NA);
                    sprintf (notetitle, "[%8.8s %6.6s] %s", datestring + 6,
                             datestring + 23, ntitle);
                    if (dashf (fname))
                    {
                        Postfile (fname, bname, notetitle, 1);
                        sprintf (tmp, "%s 自动张贴", ntitle);
                        report (tmp);
                    }
                }
            }
            fclose (check);
        }
        getdatestring (now, NA);
        sprintf (notetitle, "[%s] 留言板记录", datestring);
        if (dashf ("etc/notepad", "r"))
        {
            Postfile ("etc/notepad", "message", notetitle, 1);
            unlink ("etc/notepad");
        }
        report ("自动发信时间更改");
    }
    return;
}


void
user_login ()
{
    char fname[STRLEN];
    char *ruser;

    if (strcmp (currentuser.userid, "SYSOP") == 0)
    {
        currentuser.userlevel = ~0;	/* SYSOP gets all permission bits */
        substitute_record (PASSFILE, &currentuser, sizeof (currentuser),
                           usernum);
    }
    ruser = getenv ("REMOTEUSER");
    sprintf (genbuf, "%s@%s", ruser ? ruser : "?", fromhost);
    log_usies ("ENTER", genbuf);
    u_enter ();
    if (ruser != NULL)
    {
        sprintf (genbuf, "%s@%s", ruser, fromhost);
        if (valid_ident (genbuf))
        {
            strncpy (currentuser.ident, genbuf, NAMELEN);
            currentuser.ident[NAMELEN - 1] = '\0';
        }
    }
    report ("Enter");
    started = 1;
    if ((!HAS_PERM (PERM_MULTILOG | PERM_SYSOP))
            && count_user () > MULTI_LOGINS && strcmp (currentuser.userid, "guest"))
    {
        report ("kicked (multi-login)[漏网之鱼]");
        abort_bbs ();
    }
    initscr ();
#ifdef USE_NOTEPAD

    notepad_init ();
    if (strcmp (currentuser.userid, "guest") != 0)
    {
        if (DEFINE (DEF_NOTEPAD))
        {
            int noteln;

            if (lastnote > currentuser.notedate)
                currentuser.noteline = 0;
            noteln = countln ("etc/notepad");
            if (currentuser.noteline == 0)
            {
                shownotepad ();
            }
            else if ((noteln - currentuser.noteline) > 0)
            {
                move (0, 0);
                ansimore2 ("etc/notepad", NA, 0,
                           noteln - currentuser.noteline + 1);
                igetkey ();
                clear ();
            }
            currentuser.noteline = noteln;
            write_defnotepad ();
        }
    }
#endif
    if (show_statshm ("0Announce/bbslist/countusr", 0) && DEFINE (DEF_GRAPH))
    {
        refresh ();
        pressanykey ();
    }
    if ((vote_flag (NULL, '\0', 2 /* 检查读过新的Welcome 没 */ ) == 0))
    {
        //here change by alt@sjtu for vote when login
        {
            if (dashf ("Welcome"))
            {
                ansimore ("Welcome", YEA);
                vote_flag (NULL, 'R', 2 /* 写入读过新的Welcome */ );
            }
            //      strcpy(currboard,"sysop");
            //      b_vote();
            //       vote_flag(NULL, 'R', 2 /* 写入读过新的Welcome */ );
        }
    }
    else
    {
        if (fill_shmfile (3, "Welcome2", "WELCOME_SHMKEY"))
            show_welcomeshm ();
    }
    show_statshm ("etc/posts/day", 1);
    refresh ();
    move (t_lines - 2, 0);
    clrtoeol ();

    if (currentuser.numlogins < 1)
    {
        currentuser.numlogins = 0;
        getdatestring (time (0), NA);
        prints ("\033[1;36m☆ 这是您第 \033[33m1\033[36m 次拜访本站，请记住今天吧。\n");
        prints ("☆ 您第一次连入本站的时间为 \033[33m%s\033[m ", datestring);
    }
    else
    {
        getdatestring (currentuser.lastlogin, NA);
        prints
        ("\033[1;36m☆ 这是您第 \033[33m%d\033[36m 次拜访本站，上次您是从 \033[33m%s\033[36m 连往本站。\n",
         currentuser.numlogins + 1, currentuser.lasthost);
        prints ("☆ 上次连线时间为 \033[33m%s\033[m ", datestring);
    }
    igetkey ();
    WishNum = 9999;
    setuserfile (fname, BADLOGINFILE);
    if (ansimore (fname, NA) != -1)
    {
        if (askyn ("您要删除以上密码输入错误的记录吗", YEA, YEA) == YEA)
            unlink (fname);
    }

    set_safe_record ();
    check_uinfo (&currentuser, 0);
    strncpy (currentuser.lasthost, fromhost, 16);
    currentuser.lasthost[15] = '\0';	/* dumb mistake on my part */
    currentuser.lastlogin = time (0);

    if (HAS_PERM (PERM_SYSOP) || !strcmp (currentuser.userid, "guest"))
        currentuser.lastjustify = time (0);
    //next remove by alt@sjtu
    //取消每年1次的重新注册
    //{
    /*
       if (    HAS_PERM(PERM_LOGINOK) 
       && (abs(time(0)-currentuser.lastjustify)>=REG_EXPIRED * 86400)) {
       #ifdef MAILCHECK       
       currentuser.email[0]= '\0';
       #endif      
       currentuser.address[0]='\0';
       currentuser.userlevel &= ~(PERM_LOGINOK | PERM_PAGE | PERM_MESSAGE);
       mail_file("etc/expired", currentuser.userid, "更新个人资料说明。");
       }
     */
    //}
    currentuser.numlogins++;
#ifdef ALLOWGAME
   check_downline();
//   update_game_money();
#endif
    if (currentuser.firstlogin == 0)
    {
        currentuser.firstlogin = time (0) - 7 * 86400;
    }
    substitute_record (PASSFILE, &currentuser, sizeof (currentuser), usernum);
    check_register_info ();
}


void
set_numofsig ()
{
    int sigln;
    char signame[STRLEN];

    setuserfile (signame, "signatures");
    sigln = countln (signame);
    numofsig = sigln / MAXSIGLINES;
    if ((sigln % MAXSIGLINES) != 0)
        numofsig += 1;
}

int
check_maxmail ()
{
    extern char currmaildir[STRLEN];
    int maxmail, maxsize, mailsize, oldnum;
	int bmns = 0;	//担任板主数量

//    maxmail = (HAS_PERM (PERM_SYSOP)) ?
//              MAX_SYSOPMAIL_HOLD : (HAS_PERM (PERM_BOARDS)) ?
//              MAX_BMMAIL_HOLD : MAX_MAIL_HOLD;
	//modified by rwzmm @ sjtubbs, change the max mail hold of bms
	//depending on how many bms he gets.
	if(HAS_PERM (PERM_SYSOP))
	{
		maxmail = MAX_SYSOPMAIL_HOLD;
	}
	else if(HAS_PERM (PERM_BOARDS))
	{
		bmns = getbmns(currentuser.userid);
		if(bmns > 0)
			maxmail = MAX_BMMAIL_HOLD + MAX_MAIL_HOLD * (bmns-1);
		else
			maxmail = MAX_BMMAIL_HOLD;
	}
	else
	{
		maxmail = MAX_MAIL_HOLD;
	}

    set_safe_record ();
    oldnum = currentuser.nummails;
    currentuser.nummails =
        get_num_records (currmaildir, sizeof (struct fileheader));
    if (oldnum != currentuser.nummails)
        substitute_record (PASSFILE, &currentuser, sizeof (currentuser), usernum);
    if (HAS_PERM (PERM_MAILINFINITY))
        return 0;
    maxsize = getmailboxsize (currentuser.userid, currentuser.userlevel);
    mailsize = getmailsize (currentuser.userid);
    if (currentuser.nummails > maxmail || mailsize > maxsize)
    {
        mailXX = 1;
        clear ();
        move (4, 0);
        if (currentuser.nummails > maxmail)
            prints ("您的私人信件高达 %d 封, 您的邮件上限: %d 封\n",
                    currentuser.nummails, maxmail);
        if (mailsize > maxsize)
            prints ("您的信件容量高达 %d K，您的容量上限: %d K\n",
                    mailsize, maxsize);
        prints
        ("您的私人信件已经超限, 请整理信箱，否则无法使用本站的送信功能。\n");
        if (currentuser.nummails > maxmail + 100)
        {
            sprintf (genbuf, "私人信件过量: %d 封", currentuser.nummails);
            securityreport (genbuf);
        }
        if (mailsize > maxsize + 1000)
        {
            sprintf (genbuf, "私人信件过量: %d K", mailsize);
            securityreport (genbuf);
        }
    }
    else
    {
        mailXX = 0;
    }
    return mailXX;
}

#ifndef BBSD
int
main (argc, argv)
int argc;
char **argv;
#else
void
start_client ()
#endif
{

	
	struct stat kkk;
	char fname[80];//add by pax
	int fd;//add by pax	
	struct article n_arit;
	int size;
	int i;
    load_sysconf ();

#ifdef ALLOWSWITCHCODE

    resolve_GbBig5Files ();
#endif

#ifndef BBSD

    if (argc < 2 || *argv[1] != 'h')
    {
        printf ("You cannot execute this program directly.\n");
        exit (-1);
    }
    system_init (argc, argv);
#else

    system_init ();
#endif

    if (setjmp (byebye))
    {
        system_abort ();
    }
#ifndef BBSD
    get_tty ();
    init_tty ();
#endif

    login_query ();
    user_login ();
    m_init ();
    RMSG = NA;
    clear ();
    c_recover ();
#ifdef TALK_LOG

    tlog_recover ();		/* 990713.edwardc for talk_log recover */
#endif

    if (strcmp (currentuser.userid, "guest"))
    {
        if (HAS_PERM (PERM_ACCOUNTS) && dashf("register_sjtubbs"))
        {
			struct stat st = {0};
			stat("register_sjtubbs", &st);
			if(st.st_size >0)
            {
				prints ("[0;1m\n\n");
				prints ("[33m目前有新使用者等候您的审批。[37m\n\n");
				pressanykey ();
			}
        }
        if (check_maxmail ())
            pressanykey ();
        move (9, 0);
        clrtobot ();
        if (!DEFINE (DEF_NOLOGINSEND))
            if (!uinfo.invisible)
                apply_ulist (friend_login_wall);
        clear ();
        set_numofsig ();
/*pax#sjtubbs 初始化rpg参数*/
#ifdef ALLOWGAME


	   //sethomefile(fname, currentuser.userid, "rpg.result");
	   sethomefile(fname, currentuser.userid, PK_PERSON_FILE);
	   if(dashf(fname))
	   {
		   get_record (fname, &currentpker, sizeof (struct person), 1);
		   /*sethomefile(fname, currentuser.userid, "rpg.battle");
		   if(!dashf(fname))
		   {
			   fd=open(fname,O_CREAT | O_WRONLY | O_APPEND , 0644);
			   write (fd, &currentpker, sizeof (struct person));
			   close(fd);
			   sethomefile(fname, currentuser.userid, "pk.article");
			   if(dashf(fname))
			   {
				   sethomefile(fname, currentuser.userid, "pk.article");
				   if(	fd=open(fname,O_RDONLY)!=-1)
				   {
					   fd=open(fname,O_RDONLY);
					   stat(fname,&kkk);
					   size=kkk.st_size/sizeof(struct article);
					   close(fd);	
					   i=0;
					   while (i<size)
					   {
						   get_record (fname, &n_arit, sizeof (struct article), i+1);		
						   get_article(currentuser.userid,&n_arit);
						   i++;
					   }
				   }
			   }
		  } */

			   //sethomefile(fname, currentuser.userid, "rpg.battle");
			   //get_record (fname, &currentpker, sizeof (struct person), 1);
		   }

#endif
/*end by pax#sjtubbs*/
    }

    ActiveBoard_Init ();

    fill_date ();			/* 检查纪念日 */
    b_closepolls ();		/* 关闭投票 */

    num_alcounter ();

	//add by rwzmm @ sjtubbs
	//show global vote.
	show_global_vote();
#ifdef REAL_NAME_EMAIL
	verify_real_mail();
	ask_real_mail();
#endif
    if (count_friends > 0 && DEFINE (DEF_LOGFRIEND))
        t_friends ();

#ifdef BIRTHDAY_POST      
   if(is_birth(currentuser)&&currentuser.lastlogout/3600<time(0)/3600){
      char happybirthday[STRLEN];
	  int nBirthFile;
	  FILE* fpBthMail = NULL;
	  char* mailbuffer = NULL;
	  int bthmailsize = 0;
	  int id;

      sprintf(happybirthday,"%s, 生日快乐",currentuser.userid);
	  nBirthFile = (currentuser.numlogins % 7) + 1;

	  switch(nBirthFile)
	  {
	  case 1:
		  Postfile("etc/birthday_1","gift",happybirthday,1);
		  break;
	  case 2:
		  Postfile("etc/birthday_2","gift",happybirthday,1);
		  break;
	  case 3:
		  Postfile("etc/birthday_3","gift",happybirthday,1);
		  break;
	  case 4:
		  Postfile("etc/birthday_4","gift",happybirthday,1);
		  break;
	  case 5:
		  Postfile("etc/birthday_5","gift",happybirthday,1);
		  break;
	  case 6:
		  Postfile("etc/birthday_6","gift",happybirthday,1);
		  break;
	  case 7:
		  Postfile("etc/birthday_7","gift",happybirthday,1);
		  break;
	  default:
		  Postfile("etc/birthday_mail","gift",happybirthday,1);
		  break;
	  }
	
	  fpBthMail = fopen("etc/birthday_mail", "r");
	  fseek(fpBthMail, 0, SEEK_END);
	  bthmailsize = ftell(fpBthMail);
	  fclose(fpBthMail);
	  fpBthMail = NULL;

	  mailbuffer = (char*) malloc(bthmailsize);
	  fpBthMail = fopen("etc/birthday_mail", "r");
	  fread(mailbuffer, bthmailsize, 1, fpBthMail);
	  fclose(fpBthMail);
	  fpBthMail = NULL;
	  autoreport(happybirthday,mailbuffer,currentuser.userid, NULL);
	  free(mailbuffer);
	  mailbuffer = NULL;
	
	  id = getuser(currentuser.userid);
	  currentuser.money += 1000;
	  substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
   }
#endif   

    while (1)
    {
        if (DEFINE (DEF_NORMALSCR))
            domenu ("TOPMENU");
        else
            domenu ("TOPMENU2");
        Goodbye ();
    }
}

int refscreen = NA;

int
egetch ()
{
    int rval;

    check_calltime ();
    if (talkrequest)
    {
        talkreply ();
        refscreen = YEA;
        return -1;
    }
    while (1)
    {
        rval = igetkey ();
        if (talkrequest)
        {
            talkreply ();
            refscreen = YEA;
            return -1;
        }
        if (rval != Ctrl ('L'))
            break;
        redoscr ();
    }
    refscreen = NA;
    return rval;
}

char *
boardmargin ()
{
    static char buf[STRLEN];

    if (selboard)
        sprintf (buf, "讨论区 [%s]", currboard);
    else
    {
        brc_initial (DEFAULTBOARD);
        if (!getbnum (currboard))
            setoboard (currboard);
        selboard = 1;
        sprintf (buf, "讨论区 [%s]", currboard);
    }
    return buf;
}

void
update_endline ()
{
    char buf[255], fname[STRLEN], *ptr;
    time_t now;
    FILE *fp;
    int i, allstay, foo, foo2;

    move (t_lines - 1, 0);
    clrtoeol ();

    if (!DEFINE (DEF_ENDLINE))
        return;

    now = time (0);
    allstay = getdatestring (now, YEA);	// allstay 为当前秒数
    if (allstay == 0)
    {
nowishfile:
        resolve_boards ();
        strcpy (datestring, brdshm->date);
        allstay = 1;
    }
    if (allstay < 5)
    {
        allstay = (now - login_start_time) / 60;
        sprintf (buf, "[[36m%.12s[33m]", currentuser.userid);
        num_alcounter ();
        prints
        ("[1;44;33m[[36m%29s[33m][[36m%4d[33m人/[1;36m%3d[33m友][[36m%1s%1s%1s%1s%1s%1s[33m]帐号%-24s[[36m%3d[33m:[36m%2d[33m][m",
         datestring, count_users, count_friends,
         (uinfo.pager & ALL_PAGER) ? "P" : "p",
         (uinfo.pager & FRIEND_PAGER) ? "O" : "o",
         (uinfo.pager & ALLMSG_PAGER) ? "M" : "m",
         (uinfo.pager & FRIENDMSG_PAGER) ? "F" : "f",
         (DEFINE (DEF_MSGGETKEY)) ? "X" : "x",
         (uinfo.invisible == 1) ? "C" : "c", buf,
         (allstay / 60) % 1000, allstay % 60);
        return;
    }
    setuserfile (fname, "HaveNewWish");
    if (WishNum == 9999 || dashf (fname))
    {
        if (WishNum != 9999)
            unlink (fname);
        WishNum = 0;
        orderWish = 0;

        if (is_birth (currentuser))
        {
            strcpy (GoodWish[WishNum],
                    "                     啦啦～～，生日快乐!   记得要请客哟 :P                   ");
            WishNum++;
        }

        setuserfile (fname, "GoodWish");
        if ((fp = fopen (fname, "r")) != NULL)
        {
            for (; WishNum < 20;)
            {
                if (fgets (buf, 255, fp) == NULL)
                    break;
                buf[STRLEN - 4] = '\0';
                ptr = strtok (buf, "\n\r");
                if (ptr == NULL || ptr[0] == '#')
                    continue;
                strcpy (buf, ptr);
                for (ptr = buf; *ptr == ' ' && *ptr != 0; ptr++)
                    ;
                if (*ptr == 0 || ptr[0] == '#')
                    continue;
                for (i = strlen (ptr) - 1; i < 0; i--)
                    if (ptr[i] != ' ')
                        break;
                if (i < 0)
                    continue;
                foo = strlen (ptr);
                foo2 = (STRLEN - 3 - foo) / 2;
                strcpy (GoodWish[WishNum], "");
                for (i = 0; i < foo2; i++)
                    strcat (GoodWish[WishNum], " ");
                strcat (GoodWish[WishNum], ptr);
                for (i = 0; i < STRLEN - 3 - (foo + foo2); i++)
                    strcat (GoodWish[WishNum], " ");
                GoodWish[WishNum][STRLEN - 4] = '\0';
                WishNum++;
            }
            fclose (fp);
        }
    }
    if (WishNum == 0)
        goto nowishfile;
    if (orderWish >= WishNum * 2)
        orderWish = 0;
    prints ("[0;1;44;33m[[36m%s[33m][m", GoodWish[orderWish / 2]);
    orderWish++;
}

/*ReWrite by SmallPig*/
void
showtitle (title, mid)
char *title, *mid;
{
    char buf[STRLEN], *note;
    int spc1, spc2;

    note = boardmargin ();
    spc1 = 39 + num_ans_chr (title) - strlen (title) - strlen (mid) / 2;
    if (spc1 < 2)
        spc1 = 2;
    spc2 =
        79 - (strlen (title) - num_ans_chr (title) + spc1 + strlen (note) +
              strlen (mid));
    //if (spc1 < 1) spc1 = 1;
    if (spc2 < 1)
        spc2 = 1;
    move (0, 0);
    clrtoeol ();
    sprintf (buf, "%*s", spc1, "");
    if (!strcmp (mid, BoardName))
        prints ("[1;44;33m%s%s[37m%s[1;44m", title, buf, mid);
    else if (mid[0] == '[')
        prints ("[1;44;33m%s%s[5;36m%s[m[1;44m", title, buf, mid);
    else
        prints ("[1;44;33m%s%s[36m%s", title, buf, mid);
    sprintf (buf, "%*s", spc2, "");
    prints ("%s[33m%s[m\n", buf, note);
    update_endline ();
    move (1, 0);
}

void
firsttitle (title)
char *title;
{
    char middoc[30];

    if (chkmail ())
        //              strcpy(middoc, strstr(title,"讨论区列表")?"[您有信件，按 M 看新信]":"[您有信件]");
        strcpy (middoc,
                strstr (title,
                        "讨论区列表") ? "[您有信件，按 L 看新信]" : "[您有信件]");
    else if (mailXX == 1)
        strcpy (middoc, "[信件过量，请整理信件!]");
    else
        strcpy (middoc, BoardName);

    showtitle (title, middoc);
}

void
docmdtitle (title, prompt)
char *title, *prompt;
{
    firsttitle (title);
    move (1, 0);
    clrtoeol ();
    prints ("%s", prompt);
    clrtoeol ();
}

void
c_recover ()
{
    char fname[STRLEN], buf[STRLEN];
    int a;

    sprintf (fname, "home/%c/%s/%s.deadve", toupper (currentuser.userid[0]),
             currentuser.userid, currentuser.userid);
    if (!dashf (fname) || strcmp (currentuser.userid, "guest") == 0)
        return;
    clear ();
    strcpy (genbuf, "");
    getdata (0, 0,
             "[1;32m您有一个编辑作业不正常中断，(S) 写入暂存档 (M) 寄回信箱 (Q) 算了？[M]：[m",
             genbuf, 2, DOECHO, YEA);
    switch (genbuf[0])
    {
    case 'Q':
    case 'q':
        unlink (fname);
        break;
    case 'S':
    case 's':
        while (1)
        {
            strcpy (genbuf, "");
            getdata (2, 0, "[1;33m请选择暂存档 [0-7] [0]：[m", genbuf, 2,
                     DOECHO, YEA);
            if (genbuf[0] == '\0')
                a = 0;
            else
                a = atoi (genbuf);
            if (a >= 0 && a <= 7)
            {
                sprintf (buf, "home/%c/%s/clip_%d",
                         toupper (currentuser.userid[0]), currentuser.userid,
                         a);
                if (dashf (buf))
                {
                    getdata (3, 0,
                             "[1;31m暂存档已存在，覆盖或附加? (O)覆盖 (A)附加 [O]：[m",
                             genbuf, 2, DOECHO, YEA);
                    switch (genbuf[0])
                    {
                    case 'A':
                    case 'a':
                        f_cp (fname, buf, O_APPEND);
                        unlink (fname);
                        break;
                    default:
                        unlink (buf);
                        rename (fname, buf);
                        break;
                    }
                }
                else
                    rename (fname, buf);
                break;
            }
        }
        break;
    default:
        mail_file (fname, currentuser.userid, "不正常断线所保留的部份...");
        unlink (fname);
        break;
    }
}

#ifdef TALK_LOG
void
tlog_recover ()
{
    char buf[256];

    sprintf (buf, "home/%c/%s/talk_log", toupper (currentuser.userid[0]),
             currentuser.userid);

    if (strcasecmp (currentuser.userid, "guest") == 0 || !dashf (buf))
        return;

    clear ();
    strcpy (genbuf, "");
    getdata (0, 0,
             "[1;32m您有一个不正常断线所留下来的聊天记录, 您要 .. (M) 寄回信箱 (Q) 算了？[Q]：[m",
             genbuf, 2, DOECHO, YEA);

    if (genbuf[0] == 'M' || genbuf[0] == 'm')
    {
        mail_file (buf, currentuser.userid, "聊天记录");
    }
    unlink (buf);
    return;
}
#endif


