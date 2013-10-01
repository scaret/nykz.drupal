
#include "../../include/bbs.h"
#include "sys/socket.h"
#include "netdb.h"
#include "netinet/in.h"
#include "stdarg.h"

#define PORT	10111
char fromhost[80];


/* ת���İ��� */

char *oboards[] = {
                      "installbbs",
                      "Unix",
                      "GNULinux",
                      "English",
                      "goabroad",
                      "HHU",
                      "test",
                      "XDU",
                      "Hongkong",
                      NULL
                  };


/* ������ʵĵ�ַ, '*' �������е�ַ */

char *allow_hosts[] = {
                          "*",
                          NULL
                      };


/* ����ĵ�ַ, �ݲ�֧��ͨ���, ��������IP��ַ */

char *deny_hosts[] = {
                         "12.34.56.78",
                         "10.0.0.1",
                         NULL
                     };

int valid_board(char *board)
{
    int i;

    for (i = 0; oboards[i] != NULL && i < 999; i++)
        if (!strcmp(oboards[i], board))
            return 1;
    return 0;
}

int valid_host(char *rhost)
{
    int i;

    for (i = 0; allow_hosts[i] != NULL && i < 999; i++)
        if (!strcmp(allow_hosts[i], rhost) || !strcmp(allow_hosts[i], "*"))
            goto N;
    return 0;
N:
    for (i = 0; deny_hosts[i] != NULL && i < 999; i++)
        if (!strcmp(deny_hosts[i], rhost))
            return 0;
    return 1;
}

int do_log(char *fmt, ...)
{
    FILE *fp;
    va_list ap;
    char cmd[256];
    time_t t = time(0);

    va_start(ap, fmt);
    vsnprintf(cmd, 255, fmt, ap);
    va_end(ap);
    fp = fopen("bbshome/reclog/bbsoutd.log", "a");
    fprintf(fp, "%24.24s %-24s %s\n", ctime(&t), fromhost, cmd);
    fclose(fp);
}

int main()
{
    static struct sockaddr_in xs;
    int fd, fd2, i;

    chdir(BBSHOME);
    if (fork())
        exit(0);
    for (i = 0; i <= getdtablesize(); i++)
        close(i);
    for (i = 1; i <= NSIG; i++)
        signal(i, SIG_IGN);
    xs.sin_family = AF_INET;
    xs.sin_port = htons(PORT);
    fd = socket(AF_INET, SOCK_STREAM, 0);
    i = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &i, sizeof(i));
    if (bind(fd, (struct sockaddr *) &xs, sizeof xs) < 0)
        exit(0);
    if (listen(fd, 4) < 0)
        exit(0);
    umask((mode_t) 022);
    i = sizeof xs;
    while (1)
        if ((fd2 = accept(fd, (struct sockaddr *) &xs, &i)) > 0)
        {
            if (fork() == 0)
                break;
            close(fd2);
        }
    dup2(fd2, 0);
    close(fd2);
    dup2(0, 1);
    strcpy(fromhost, (char *) inet_ntoa(xs.sin_addr));
    do_send(fromhost);
    do_log("LOGOUT");
    exit(0);
}

int do_send(char *rhost)
{
    FILE *fp, *fp2;
    struct fileheader x;
    int last, r, dt;
    char oboard[100], brk[100], dir[80], file[80], buf[256];

    srand(times(0) + getpid());
    do_log("LOGIN");
    if (!valid_host(rhost))
        return printf("access denied.\n");
    sprintf(brk, "=====random_break_line_%d=====\n", rand());
    printf("%s", brk);
    fflush(stdout);
    if (fgets(buf, 80, stdin) <= 0)
        return;
    do_log("CMD: %s", buf);
    r = sscanf(buf, "select * from %s where dt < %d", oboard, &dt);
    if (r < 2)
        return printf("Usage: select * from board where dt < time\n");
    if (!strcmp(oboard, "oboard_ctl"))
    {
        int i;

        for (i = 0; oboards[i] != 0 && i < 999; i++)
            printf("%s\n", oboards[i]);
        do_log("boards list");
        return;
    }

    if (!valid_board(oboard))
        return printf("bad boardname\n");
    if (dt > 86400 * 7)
        dt = 86400 * 7;         //��ת�������¡�
    last = time(0) - dt;
    sprintf(dir, "boards/%s/.DIR", oboard);
    fp = fopen(dir, "r");
    while (1)
    {
        int t, lines = 0;

        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        t = atoi(x.filename + 2);
        if (t <= last)
            continue;
        if (strstr(x.owner, "@"))
            continue;           //��վ���ż�����ת��
        printf("%s\n", x.title);
        printf("%s.bbs@%s\n", x.owner, BBSHOST);
        sprintf(file, "boards/%s/%s/%s", oboard, getfolderstr(x.filename), x.filename);
        fp2 = fopen(file, "r");
        while (1)
        {
            if (fgets(buf, 255, fp2) <= 0)
                break;
            if (lines == 0 && !strncmp(buf, "������", 6))
                sprintf(buf, "������: %s.bbs@%s, ԭ����: %s\n", x.owner,
                        BBSHOST, oboard);
            lines++;
            printf("%s", buf);
        }
        fclose(fp2);
        printf("\n%s", brk);
        fflush(stdout);
    }
    fclose(fp);
}
