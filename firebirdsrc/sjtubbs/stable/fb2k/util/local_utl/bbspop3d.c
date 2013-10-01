/*
    POP3D server for Firebird BBS.
    Revision [29 Oct 1997] By Peng-Piaw Foong, ppfoong@csie.ncu.edu.tw
    
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
   ʹ�÷����� ./bbspop3d 1110
   ��ʾ bbs pop3 �������� 1110 �˿��ϡ�( �޲�����Ϊȱʡ�� 110 �˿�)
   Ҫʹ�� 1024 ���µĶ˿ڣ������� root �������ִ�У����ң��ö˿�û�б�ռ��
*/

/* $Id: bbspop3d.c,v 1.2 2004/06/29 14:59:22 rwzmm Exp $ */

#include <sys/ioctl.h>
#include "bbs.h"

#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

/* Edit these lines to fit the configuration of your server */

#define POP3D_PORT  110
#define BUFSIZE     1024
#define BBSPASSWDS  BBSHOME"/.PASSWDS"
#define BADLOGINFILE "logins.bad"
#define THEBBSNAME ".bbs@"BBSHOST

/*  You needn't modify any lines below unless you know what you're doing */

struct fileheader currentmail;
struct userec currentuser;

char LowUserid[20];
char genbuf[BUFSIZE];
char hostname[80];

#define QLEN            5

#define S_CONNECT       1
#define S_LOGIN         2


#define STRN_CPY(d,s,l) { strncpy((d),(s),(l)); (d)[(l)-1] = 0; }
#define ANY_PORT        0
#define RFC931_PORT     113
#define RFC931_TIMEOUT  5
#define POP3_TIMEOUT    60

static jmp_buf timebuf;

int State;
int msock, sock;                /* master server socket */
static int reaper();
char fromhost[STRLEN];
char inbuf[BUFSIZE];
char remote_userid[STRLEN];
FILE *cfp;
char *msg, *cmd;
int fd;
struct fileheader *fcache;
int totalnum, totalbyte, markdel, idletime;
int *postlen;

void log_usies();
int Quit(), User(), Pass(), Noop(), Stat(), List(), Retr(), Rset();
int Last(), Dele(), Uidl();
int Top();                      /* Leeward adds, 98.01.21 */

struct commandlist
{
    char *name;
    int (*fptr) ();
}
cmdlists[] = {
                 "retr", Retr, "dele", Dele, "user", User, "pass", Pass, "stat", Stat, "list", List, "uidl", Uidl, "quit", Quit, "rset", Rset, "last", Last, "noop", Noop, "top", Top,       /* Leeward adds, 98.01.21 */
                 NULL, NULL};

char *crypt();

/* Added by deardragon 1999.12.2 */
char datestring[30];

void getdatestring(time_t now)
{
    struct tm *tm;
    char weeknum[7][3] = { "��", "һ", "��", "��", "��", "��", "��" };

    tm = localtime(&now);
    sprintf(datestring, "%4d��%02d��%02d��%02d:%02d:%02d ����%2s",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec, weeknum[tm->tm_wday]);
}

/* Added End. */

void logattempt(char *uid, char *frm)       /* Leeward 98.07.25 */
{
    char fname[STRLEN];
    int fd, len;
    time_t now = time(0);

    getdatestring(now);
    sprintf(genbuf, "%-12.12s  %-30s %-20s pop3\n", uid, datestring, frm);
    len = strlen(genbuf);
    if ((fd = open(BADLOGINFILE, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
    {
        write(fd, genbuf, len);
        close(fd);
    }
    sprintf(fname, "home/%c/%c/%s/%s", toupper(uid[0]), toupper(uid[1]), uid, BADLOGINFILE);
    if ((fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644)) > 0)
    {
        write(fd, genbuf, len);
        close(fd);
    }
}

int checkpasswd(char *passwd, char *test)
{
    static char pwbuf[14];
    char *pw;

    strncpy(pwbuf, test, 14);
    pw = crypt(pwbuf, passwd);
    return (!strcmp(pw, passwd));
}

static int abort_server()
{
    log_usies("ABORT SERVER");
    close(msock);
    close(sock);
    exit(1);
}

int dokill()
{
    return kill(0, SIGKILL);
}

static FILE *fsocket(int domain, int type, int protocol)
{
    int s;
    FILE *fp;

    if ((s = socket(domain, type, protocol)) < 0)
    {
        return (0);
    }
    else
    {
        if ((fp = fdopen(s, "r+")) == 0)
        {
            close(s);
        }
        return (fp);
    }
}

void outs(char *str)
{
    char sendbuf[BUFSIZE];

    (void) bzero(sendbuf, sizeof(sendbuf));
    (void) snprintf(sendbuf, BUFSIZE, "%s\r\n", str);
    (void) write(sock, sendbuf, strlen(sendbuf));
}

void outfile(char *filename, int linenum)
{
    FILE *fp;
    char linebuf[256];

    if (linenum && (fp = fopen(filename, "r")) != NULL)
    {
        while (fgets(linebuf, 256, fp) != NULL && linenum > 0)
        {
            linebuf[strlen(linebuf) - 1] = '\0';
            if (strcmp(linebuf, ".") == 0)
                outs("..");
            else
                outs(linebuf);
            linenum--;
        }
        fclose(fp);
    }
    outs(".");
}


/* timeout - handle timeouts */
static void timeout(int sig)
{
    longjmp(timebuf, sig);
}


void rfc931(struct sockaddr_in *rmt_sin, struct sockaddr_in *our_sin, char *dest)
{
#if 0
    unsigned rmt_port;
    unsigned our_port;
    struct sockaddr_in rmt_query_sin;
    struct sockaddr_in our_query_sin;
    char user[256];
    char buffer[512];
    char *cp;
    FILE *fp;
    char *result = "unknown";
    struct hostent *hp;


    /*
     * Use one unbuffered stdio stream for writing to and for reading from
     * the RFC931 etc. server. This is done because of a bug in the SunOS 
     * 4.1.x stdio library. The bug may live in other stdio implementations,
     * too. When we use a single, buffered, bidirectional stdio stream ("r+"
     * or "w+" mode) we read our own output. Such behaviour would make sense
     * with resources that support random-access operations, but not with   
     * sockets.
     */
    if ((fp = fsocket(AF_INET, SOCK_STREAM, 0)) != 0)
    {
        setbuf(fp, (char *) 0);

        /*
         * Set up a timer so we won't get stuck while waiting for the server.
         */

        if (setjmp(timebuf) == 0)
        {
            signal(SIGALRM, (void *) timeout);
            alarm(RFC931_TIMEOUT);

            /*
             * Bind the local and remote ends of the query socket to the same
             * IP addresses as the connection under investigation. We go
             * through all this trouble because the local or remote system
             * might have more than one network address. The RFC931 etc.  
             * client sends only port numbers; the server takes the IP    
             * addresses from the query socket.
             */

            our_query_sin = *our_sin;
            our_query_sin.sin_port = htons(ANY_PORT);
            rmt_query_sin = *rmt_sin;
            rmt_query_sin.sin_port = htons(RFC931_PORT);

            if (bind(fileno(fp), (struct sockaddr *) &our_query_sin,
                     sizeof(our_query_sin)) >= 0 &&
                    connect(fileno(fp), (struct sockaddr *) &rmt_query_sin,
                            sizeof(rmt_query_sin)) >= 0)
            {

                /*
                 * Send query to server. Neglect the risk that a 13-byte
                 * write would have to be fragmented by the local system and
                 * cause trouble with buggy System V stdio libraries.
                 */

                fprintf(fp, "%u,%u\r\n",
                        ntohs(rmt_sin->sin_port),
                        ntohs(our_sin->sin_port));
                fflush(fp);

                /*
                 * Read response from server. Use fgets()/sscanf() so we can
                 * work around System V stdio libraries that incorrectly
                 * assume EOF when a read from a socket returns less than
                 * requested.
                 */

                if (fgets(buffer, sizeof(buffer), fp) != 0
                        && ferror(fp) == 0 && feof(fp) == 0
                        && sscanf(buffer, "%u , %u : USERID :%*[^:]:%255s",
                                  &rmt_port, &our_port, user) == 3
                        && ntohs(rmt_sin->sin_port) == rmt_port
                        && ntohs(our_sin->sin_port) == our_port)
                {

                    /*
                     * Strip trailing carriage return. It is part of the
                     * protocol, not part of the data.
                     */

                    if (cp = strchr(user, '\r'))
                        *cp = 0;
                    result = user;
                }
            }
            alarm(0);
        }
        fclose(fp);
    }
    STRN_CPY(dest, result, 60);

    if (strcmp(dest, "unknown") == 0)
        strcpy(dest, "");
    else
        strcat(dest, "@");

    hp = gethostbyaddr((char *) &rmt_sin->sin_addr, sizeof(struct in_addr),
                       rmt_sin->sin_family);
    if (hp)
        strcat(dest, hp->h_name);
    else
#endif
        strcpy(dest, (char *) inet_ntoa(rmt_sin->sin_addr));

}

int Isspace(char ch)
{
    return (ch == ' ' || ch == '\t' || ch == 10 || ch == 13);
}

char *nextwordlower(char **str)
{
    char *p;

    while (Isspace(**str))
        (*str)++;
    p = (*str);

    while (**str && !Isspace(**str))
    {
        **str = tolower(**str);
        (*str)++;
    }

    if (**str)
    {
        **str = '\0';
        (*str)++;
    }
    return p;
}

char *nextword(char **str)
{
    char *p;

    while (Isspace(**str))
        (*str)++;
    p = (*str);

    while (**str && !Isspace(**str))
        (*str)++;

    if (**str)
    {
        **str = '\0';
        (*str)++;
    }
    return p;
}

char *nextwordX(char **str)            /* Leeward: 97.12.27: enable password include space(s) */
{
    char *p;

    while (Isspace(**str))
        (*str)++;
    p = (*str);

    while (**str && '\t' != **str && 10 != **str && 13 != **str)
        (*str)++;

    if (**str)
    {
        **str = '\0';
        (*str)++;
    }
    return p;
}

void Init()
{
    State = S_CONNECT;
    LowUserid[0] = '\0';
    markdel = 0;
    idletime = 0;
    //(void)gethostname(hostname, 80);
}

void Login_init()
{
    int fd, i;
    char *ptr;
    struct stat st;

    totalnum = totalbyte = 0;
    sprintf(genbuf, "mail/%c/%c/%s/.DIR", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid);
    if (stat(genbuf, &st) == -1 || st.st_size == 0)
    {
        return;
    }
    totalnum = st.st_size / sizeof(struct fileheader);
    fcache = (struct fileheader *) malloc(st.st_size);
    postlen = (int *) malloc(sizeof(int) * totalnum);
    fd = open(genbuf, O_RDONLY);
    read(fd, fcache, st.st_size);
    close(fd);

    for (i = 0; i < totalnum; i++)
    {
        if (index(fcache[i].owner, '@') == NULL)
        {
            if (ptr = strchr(fcache[i].owner, ' '))
                *ptr = '\0';
            strcat(fcache[i].owner, THEBBSNAME);
        }
        sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid, 
                fcache[i].filename);
        if (stat(genbuf, &st) == -1)
            st.st_size = 0;
        postlen[i] =
            st.st_size + strlen(fcache[i].owner) + 10 +
            strlen(fcache[i].title) + 10 + 40;
        totalbyte += postlen[i];
        if (fcache[i].accessed[0] & FILE_MARKED)        /* Leeward 99.01.28 */
            fcache[i].accessed[1] = 'M';
        fcache[i].accessed[0] = ' ';
    }
}

void pop3_timeout()
{
#if 0
    idletime++;
    if (idletime > 5)
#endif 
    {
        log_usies("ABORT - TIMEOUT");
        fclose(cfp);
        close(sock);
        exit(1);
    }
    alarm(POP3_TIMEOUT);
}


int main(int argc, char **argv)
{

    struct sockaddr_in fsin, our;
    int on, alen, len, i, n;
    char *str;
    int portnum = POP3D_PORT;
    int childpid;

    if (2 == argc)
    {
        if(strcmp(argv[1],"--tcp4")==0)
        {
            sock = 0;
            alen = sizeof(fsin);
            if(0!=getpeername(sock, (struct sockaddr*)&fsin, &alen))
                exit(1);
            goto HANDLE_INETD;
        }
        else
        {
        portnum = atoi(argv[1]);
        }
    }
    if (0 == portnum)
        portnum = POP3D_PORT;

    if (fork())
        exit(0);
    for (n = 0; n < 10; n++)
        close(n);
    open("/dev/null", O_RDONLY);
    dup2(0, 1);
    dup2(0, 2);
    if ((n = open("/dev/tty", O_RDWR)) > 0)
    {
        ioctl(n, TIOCNOTTY, 0);
        close(n);
    }

    if ((msock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        exit(1);
    }
    setsockopt(msock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
    bzero((char *) &fsin, sizeof(fsin));
    fsin.sin_family = AF_INET;
    fsin.sin_addr.s_addr = htonl(INADDR_ANY);
    fsin.sin_port = htons(portnum);

    if (bind(msock, (struct sockaddr *) &fsin, sizeof(fsin)) < 0)
    {
        exit(1);
    }

    signal(SIGHUP, (void *) abort_server);
    signal(SIGCHLD, (void *) reaper);
    signal(SIGINT, (void *) dokill);
    signal(SIGTERM, (void *) dokill);

    listen(msock, QLEN);

    while (1)
    {

        alen = sizeof(fsin);
        sock = accept(msock, (struct sockaddr *) &fsin, &alen);
        if ((sock < 0) && (errno == EINTR))
            continue;

        if ((childpid = fork()) < 0)
        {
            exit(1);
        }

        switch (childpid)
        {
        case 0:                /* child process */
            close(msock);
HANDLE_INETD:
            setgid(BBSGID);
            setuid(BBSUID);

            strcpy(fromhost, (char *) inet_ntoa(fsin.sin_addr));
            len = sizeof our;
            getsockname(sock, (struct sockaddr *) &our, &len);

            Init();

            rfc931(&fsin, &our, remote_userid);

            cfp = fdopen(sock, "r+");
            setbuf(cfp, (char *) 0);

            sprintf(genbuf,
                    "+OK InternetBBS Pop3 server at %s starting.[FB2000]",
                    strchr(THEBBSNAME, '@') + 1);
            outs(genbuf);

            chdir(BBSHOME);

            log_usies("CONNECT");
            alarm(0);
            signal(SIGALRM, (void *) pop3_timeout);
            alarm(POP3_TIMEOUT);

            while (fgets(inbuf, sizeof(inbuf), cfp) != 0)
            {

                idletime = 0;

                msg = inbuf;

                inbuf[strlen(inbuf) - 1] = '\0';
                if (inbuf[strlen(inbuf) - 1] == '\r')
                    inbuf[strlen(inbuf) - 1] = '\0';
                cmd = nextwordlower(&msg);

                if (*cmd == 0)
                    continue;

                i = 0;
                while ((str = cmdlists[i].name) != NULL)
                {
                    if (strcmp(cmd, str) == 0)
                        break;
                    i++;
                }

                if (str == NULL)
                {
                    sprintf(genbuf, "-ERR Unknown command: \"%s\".", cmd);
                    outs(genbuf);
                }
                else
                    (*cmdlists[i].fptr) ();

                alarm(POP3_TIMEOUT);
            }

            if (State == S_LOGIN)
            {
                free(fcache);
                free(postlen);
            }
            log_usies("ABORT");
            fclose(cfp);
            close(sock);
            exit(0);
            break;
        default:               /* parent process */
            close(sock);
            break;
        }
    }
	return 0;
}

static int reaper()
{
    int state, pid;

    signal(SIGCHLD, (void *) reaper);
    signal(SIGINT, (void *) dokill);
    signal(SIGTERM, (void *) dokill);

    while ((pid = waitpid(-1, &state, WNOHANG | WUNTRACED)) > 0)
        ;
    return 0;
}

int Noop()
{
    outs("+OK");
    return 0;
}

int get_userdata(char *user)
{
    FILE *rec;
    int found = 0;

    if ((rec = fopen(BBSPASSWDS, "rb")) == NULL)
        return -1;
    while (1)
    {
        if (fread(&currentuser, sizeof(currentuser), 1, rec) <= 0)
            break;
        if (currentuser.numlogins <= 0)
            continue;
        if (strcasecmp(user, currentuser.userid))
            continue;
        else
        {
            found = 1;
            strcpy(user, currentuser.userid);
            break;
        }
    }
    fclose(rec);
    if (!found)
        return -1;
    else
        return 1;

}

int User()
{
    char *ptr;

    if (State == S_LOGIN)
    {
        outs("-ERR Unknown command: \"user\".");
        return -1;
    }

    cmd = nextwordlower(&msg);
    if (*cmd == 0)
    {
        outs("-ERR Too few arguments for the user command.");
        return -1;
    }
    if (strstr(cmd, ".bbs") == NULL)
    {
        sprintf(genbuf, "-ERR Unknown user: \"%s\".", cmd);
        outs(genbuf);
        return -1;
    }

    ptr = strchr(cmd, '.');
    *ptr = '\0';
    if (get_userdata(cmd) == 1)
    {
        strcpy(LowUserid, cmd);
        sprintf(genbuf, "+OK Password required for %s.bbs.", cmd);
        outs(genbuf);
        return 0;
    }
    else
    {
        sprintf(genbuf, "-ERR Unknown user: \"%s\".", cmd);
        outs(genbuf);
        return -1;
    }
}

void log_usies(char *buf)
{
    FILE *fp;

    if ((fp = fopen("reclog/pop3d.log", "a")) != NULL)
    {
        time_t now;
        struct tm *p;

        time(&now);
        p = localtime(&now);
        fprintf(fp, "%02d/%02d/%02d %02d:%02d:%02d [%s](%s) %s\n",
                p->tm_year, p->tm_mon + 1, p->tm_mday, p->tm_hour,
                p->tm_min, p->tm_sec,
                currentuser.userid ? currentuser.userid : "",
                remote_userid ? remote_userid : "", buf);
        fflush(fp);
        fclose(fp);
    }
}
char * Rfc2043_encode(char *inbuf, char *outbuf, int size)
{
	char *inptr = inbuf;
	char *outptr;
	if(strlen(inbuf)*3+15>size)
	{
		strncpy(outbuf,"unknown",size-1);
		outbuf[size-1] = '\0';
		return outbuf;
	}
	outptr = outbuf+ sprintf(outbuf, "=?GB18030?Q?");
	while(*inptr!='\0')
	{
		outptr += sprintf(outptr, "=%02X", (unsigned char) *inptr);
		inptr++;
	}
	outptr += sprintf(outptr, "?=");
	*outptr = '\0';
	return outbuf;
	
}
int Retr()
{
    int num;
	time_t mail_time;
    char *ptr;
    struct stat st;
	char outbuf[BUFSIZE];

    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"retr\".");
        return -1;
    }

    cmd = nextword(&msg);

    if (*cmd == 0)
    {
        outs("-ERR Too few arguments for the retr command.");
        return -1;
    }

    num = atoi(cmd);
    if (num <= 0 || totalnum < num)
    {
        sprintf(genbuf, "-ERR Message %d does not exist.", num);
        outs(genbuf);
        return -1;
    }
    else if (fcache[num - 1].accessed[0] == 'X')
    {
        sprintf(genbuf, "-ERR Message %d has been deleted.", num);
        outs(genbuf);
        return -1;
    }
    num--;
    sprintf(genbuf, "+OK %d octets", postlen[num]);
    outs(genbuf);
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid, 
            fcache[num].filename);
	mail_time = (time_t)atoi(fcache[num].filename+2);
	if(mail_time==0)
	{
		stat(genbuf, &st); 
		mail_time = st.st_mtime;
	}
	ptr = ctime(&mail_time);
    /*      Wed Jan 21 17:42:14 1998            -- ctime returns
            012345678901234567890123            -- offsets
      Date: Wed, 21 Jan 1998 17:54:33 +0800     -- RFC wants     */
    sprintf(genbuf, "Date: %3.3s, %2.2s %3.3s %4.4s %8.8s +0800",
            ptr + 0, ptr + 8, ptr + 4, ptr + 20, ptr + 11);
    outs(genbuf);
    sprintf(genbuf, "From: %s", fcache[num].owner);
    outs(genbuf);
    sprintf(genbuf, "To: %s%s", currentuser.userid, THEBBSNAME);
    outs(genbuf);
	sprintf(genbuf, "MIME-Version: 1.0");
	outs(genbuf);
	sprintf(genbuf, "Content-Type: text/plain; charset=gb18030");
	outs(genbuf);
	sprintf(genbuf, "Content-Transfer-Encoding: 8bit");
	outs(genbuf);
    sprintf(genbuf, "Subject: %s", Rfc2043_encode(fcache[num].title, outbuf, BUFSIZE));
    outs(genbuf);
    outs("");
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid, 
            fcache[num].filename);
    outfile(genbuf, 2147483647/*INT_MAX*/);
    return 0;
}


int Stat()
{
    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"stat\".");
        return -1;
    }
    sprintf(genbuf, "+OK %d %d", totalnum, totalbyte);
    outs(genbuf);
    return 0;
}


int Rset()
{
    int i;

    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"rset\".");
        return -1;
    }

    for (i = 0; i < totalnum; i++)
    {
        fcache[i].accessed[0] = ' ';
    }
    markdel = 0;
    sprintf(genbuf, "+OK Maildrop has %d messages (%d octets)", totalnum,
            totalbyte);
    outs(genbuf);
    return 0;
}

int List()
{
    int i;

    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"list\".");
        return -1;
    }

    cmd = nextword(&msg);

    if (*cmd == 0)
    {
        sprintf(genbuf, "+OK %d messages (%d octets)", totalnum,
                totalbyte);
        outs(genbuf);
        for (i = 0; i < totalnum; i++)
        {
            if (fcache[i].accessed[0] == ' ')
            {
                sprintf(genbuf, "%d %d", i + 1, postlen[i]);
                outs(genbuf);
            }
        }
        outs(".");
    }
    else
    {
        i = atoi(cmd);
        if (i <= 0 || totalnum < i)
        {
            sprintf(genbuf, "-ERR Message %d does not exist.", i);
            outs(genbuf);
            return -1;
        }
        else if (fcache[i - 1].accessed[0] == 'X')
        {
            sprintf(genbuf, "-ERR Message %d has been deleted.", i);
            outs(genbuf);
            return -1;
        }
        sprintf(genbuf, "+OK %d %d", i, postlen[i - 1]);
        outs(genbuf);
    }
    return 0;
}

int Top()
{                               /* Leeward adds, 98.01.21 */
    int num;
    int ln;

    struct stat st;

    char *ptr;
	char outbuf[BUFSIZE];


    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"top\".");
        return -1;
    }

    cmd = nextword(&msg);

    if (*cmd == 0)
    {
        outs("-ERR Too few arguments for the top command.");
        return -1;
    }
    num = atoi(cmd);
    if (num <= 0 || totalnum < num)
    {
        sprintf(genbuf, "-ERR Message %d does not exist.", num);
        outs(genbuf);
        return -1;
    }
    else if (fcache[num - 1].accessed[0] == 'X')
    {
        sprintf(genbuf, "-ERR Message %d has been deleted.", num);
        outs(genbuf);
        return -1;
    }

    cmd = nextword(&msg);

    if (*cmd == 0)
    {
        outs("-ERR Too few arguments for the top command.");
        return -1;
    }

    ln = atoi(cmd);
    if (ln < 0)
    {
        sprintf(genbuf, "-ERR Line %d does not exist.", ln);
        outs(genbuf);
        return -1;
    }


    num--;
    sprintf(genbuf, "+OK %d octets", postlen[num]);
    outs(genbuf);
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid, 
            fcache[num].filename);
    stat(genbuf, &st);
    ptr = ctime(&st.st_mtime);
    /*      Wed Jan 21 17:42:14 1998            -- ctime returns
            012345678901234567890123            -- offsets
      Date: Wed, 21 Jan 1998 17:54:33 +0800     -- RFC wants     */
    sprintf(genbuf, "Date: %3.3s, %2.2s %3.3s %4.4s %8.8s +0800",
            ptr + 0, ptr + 8, ptr + 4, ptr + 20, ptr + 11);
    outs(genbuf);
    sprintf(genbuf, "From: %s", fcache[num].owner);
    outs(genbuf);
    sprintf(genbuf, "To: %s%s", currentuser.userid, THEBBSNAME);
    outs(genbuf);
	sprintf(genbuf, "MIME-Version: 1.0");
	outs(genbuf);
	sprintf(genbuf, "Content-Type: text/plain; charset=gb18030");
	outs(genbuf);
	sprintf(genbuf, "Content-Transfer-Encoding: 8bit");
	outs(genbuf);
    sprintf(genbuf, "Subject: %s", Rfc2043_encode(fcache[num].title, outbuf, BUFSIZE));
    outs(genbuf);
    outs("");
    sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid, 
            fcache[num].filename);
    outfile(genbuf, ln);
	return 0;
}

int Uidl()
{
    int i;

    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"uidl\".");
        return -1;
    }

    cmd = nextword(&msg);

    if (*cmd == 0)
    {
        outs("+OK");
        for (i = 0; i < totalnum; i++)
        {
            if (fcache[i].accessed[0] == ' ')
            {
                sprintf(genbuf, "%d %s", i + 1, fcache[i].filename);
                outs(genbuf);
            }
        }
        outs(".");
    }
    else
    {
        i = atoi(cmd);
        if (i <= 0 || totalnum < i)
        {
            sprintf(genbuf, "-ERR Message %d does not exist.", i);
            outs(genbuf);
            return -1;
        }
        else if (fcache[i - 1].accessed[0] == 'X')
        {
            sprintf(genbuf, "-ERR Message %d has been deleted.", i);
            outs(genbuf);
            return -1;
        }
        sprintf(genbuf, "+OK %d %s", i, fcache[i - 1].filename);
        outs(genbuf);
    }
    return 0;
}


int Pass()
{
    if (State == S_LOGIN)
    {
        outs("-ERR Unknown command: \"pass\".");
        return -1;
    }

    /* Leeward: 97.12.27: enable password include space(s) */
    /*cmd = nextword(&msg); */
    cmd = nextwordX(&msg);

    if (*cmd == 0)
    {
        outs("-ERR Too few arguments for the pass command.");
        return -1;
    }

    if (LowUserid[0] == '\0')
    {
        outs("-ERR need a USER");
        return -1;
    }

    if (!checkpasswd(currentuser.passwd, cmd))
    {
        sprintf(genbuf,
                "-ERR Password supplied for \"%s.bbs\" is incorrect.",
                LowUserid);
        outs(genbuf);
        LowUserid[0] = '\0';
        log_usies("ERROR PASSWD");
        if (remote_userid)
            logattempt(currentuser.userid, remote_userid);      /* Leeward 98.07.25 */
        return -1;
    }

    if (State == S_CONNECT)
    {
        log_usies("ENTER");
        State = S_LOGIN;
    }

    Login_init();
    sprintf(genbuf, "+OK %s has %d message(s) (%d octets).", LowUserid,
            totalnum, totalbyte);
    outs(genbuf);
    return 0;
}

int Last()
{
    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"last\".");
        return -1;
    }

    sprintf(genbuf, "+OK %d is the last message seen.", totalnum);
    outs(genbuf);
    return 0;
}

int Dele()
{
    int num;

    if (State != S_LOGIN)
    {
        outs("-ERR Unknown command: \"dele\".");
        return -1;
    }

    cmd = nextword(&msg);

    if (*cmd == 0)
    {
        outs("-ERR Too few arguments for the dele command.");
        return -1;
    }

    num = atoi(cmd);
    if (num <= 0 || totalnum < num)
    {
        sprintf(genbuf, "-ERR Message %d does not exist.", num);
        outs(genbuf);
        return -1;
    }
    else if (fcache[num - 1].accessed[0] == 'X')
    {
        sprintf(genbuf, "-ERR Message %d has already been deleted.", num);
        outs(genbuf);
        return -1;
    }
    else if ('M' == fcache[num - 1].accessed[1])
    {
        sprintf(genbuf, "+OK Message %d has already been marked", num);
        outs(genbuf);           /* Leeward 99.01.28 */
        return -1;
    }
    num--;

    fcache[num].accessed[0] = 'X';
    markdel++;
    sprintf(genbuf, "+OK Message %d has been deleted.", num);
    outs(genbuf);
    return 0;
}

int do_delete()
{
    int i, fdr, fdw, count;
    char fpath[80], fnew[80];

    sprintf(fpath, "mail/%c/%c/%s/.DIR", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid);
    sprintf(fnew, "mail/%c/%c/%s/pop3.DIR", toupper(*LowUserid), toupper(LowUserid[1]), LowUserid);
    if ((fdr = open(fpath, O_RDONLY)) == -1)
        return -1;
    if ((fdw = open(fnew, O_RDWR | O_CREAT, 0644)) == -1)
        return -1;
    i = count = 0;
    while (read(fdr, &currentmail, sizeof(currentmail)))
    {
        if (i >= totalnum || fcache[i].accessed[0] == ' ')
        {
            //|| fcache[i].accessed[0] & FILE_MARKED) {
            write(fdw, &currentmail, sizeof(currentmail));
            count++;
        }
        else
        {
            sprintf(genbuf, "mail/%c/%c/%s/%s", toupper(*LowUserid), toupper(LowUserid[1]), 
                    LowUserid, currentmail.filename);
            unlink(genbuf);
        }
        i++;
    }
    close(fdr);
    close(fdw);
    unlink(fpath);
    if (count)
    {
        rename(fnew, fpath);
    }
    else
        unlink(fnew);
    return 0;
}

int Quit()
{
    if (State == S_LOGIN)
    {
        free(fcache);
        free(postlen);
        if (markdel)
            do_delete();
    }
    log_usies("EXIT");
    sprintf(genbuf,
            "+OK InternetBBS Pop3 server at %s signing off.[FB2000]",
            strchr(THEBBSNAME, '@') + 1);
    outs(genbuf);
    fclose(cfp);
    close(sock);
    exit(0);
}
