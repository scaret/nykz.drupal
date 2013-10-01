/*
 * File showbm.c - ��������ͳ�Ʊ�
 */
/* $Id: showbm.c,v 1.2 2004/06/29 14:59:23 rwzmm Exp $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include "../../include/bbs.h"
#include "showbm.h"

//add by clist at sjtubbs
char* getfolderstr(char* filename)
{
	static char str[4] = {'\0','\0','\0','\0'};
	int len = strlen(filename);
	str[0] = filename[len-5]>'4'?filename[len-5]-5:filename[len-5];
	str[1] = filename[len-4];
	str[2] = filename[len-3];
	return str;
}

void fprintchar(FILE * fp0, int num, char ch)
{
    int i;

    for (i = 0; i < num; i++)
        fprintf(fp0, "%c", ch);
}

/*
 * getpath - ȡ�þ���������Ŀ¼
 * ����ֵ��Ϊ���ʾ�������������س���
 */
int getpath(char *filename, char *pathname)
{
    FILE *fp;
    char lpathname[256];        /* local pathname */

    sprintf(lpathname, "%s/0Announce/.Search", BBSHOME);
    fp = fopen(lpathname, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Can't open .Search file.\n");
        return 1;
    }
    do
    {
        fscanf(fp, "%s", filename);
        filename[strlen(filename) - 1] = '\0';
    }
    while (strcmp(filename, pathname) && !feof(fp));
    fscanf(fp, "%s", filename);
    fclose(fp);

    return 0;
}

static int articles0 = -1;      /* �������������� */
static int articles = -1;       /* ������������� */
static int lastNarticles = -1;  /* ��ȥN���б��������� */
static time_t lastmodified = 0; /* ���������һ������ʱ�� */
static time_t lastbmpost = 0;   /* �����ڱ������һ�η�������ʱ�� */
static time_t lastpost = 0;     /* �������һ�η�������ʱ�� */

/*
 * afilecount - ���������±��� 
 * ȡ�þ��������������;������������ʱ��
 * ����ֵ��Ϊ���ʾ���α�������
 */
int afilecount(char *pathname)
{
    DIR *dp;
    struct dirent *link;

    dp = opendir(pathname);
    if (dp == NULL)
    {
        fprintf(stderr, "Can't open dir %s.\n", pathname);
        return 1;
    }
    while ((link = readdir(dp)) != 0)
    {
        struct stat st;
        char filename[256];

        if (*link->d_name != '.')
            articles0++;
        else
            continue;
        sprintf(filename, "%s", pathname);
        strcat(filename, "/");
        strcat(filename, link->d_name);
        stat(filename, &st);
        if (st.st_mtime > lastmodified)
            lastmodified = st.st_mtime;

        if ((st.st_mode & S_IFMT) == S_IFDIR)
            afilecount(filename);
    }
    closedir(dp);

    return 0;
}

/*
 * lastbmpostime - ����������󷢱�����ʱ��
 * ��bmfilecount����
 */
void lastbmpostime(char *pathname, char *filename)
{
    char lpathname[256];
    struct stat st;

    sprintf(lpathname, "%s%s/", pathname, getfolderstr(filename));
    strcat(lpathname, filename);
    stat(lpathname, &st);
    if (st.st_mtime > lastbmpost)
        lastbmpost = st.st_mtime;
}

/*
 * lastpostime - ������󷢱�����ʱ��
 *             - ����ͳ�ƹ�ȥN���б���������
 * ��bmfilecount����
 */
void lastpostime(char *pathname, char *filename)
{
    char lpathname[256];
    struct stat st;

    sprintf(lpathname, "%s%s/", pathname, getfolderstr(filename));
    strcat(lpathname, filename);
    stat(lpathname, &st);
    if (st.st_mtime > lastpost)
        lastpost = st.st_mtime;
    /* ͳ�ƹ�ȥN���б��������� */
    if (st.st_mtime > time(0) - 60 * 60 * 24 * DAYSN)
        lastNarticles++;
}

/*
 * bmfilecount - ���������������
 * ����ֵ��Ϊ������һ�㲻�����, ��Ϊǰ����ѭ���й����
 */
int bmfilecount(char *pathname, char *bm)
{
    int fd, index, total;
    char lpathname[256];
    struct fileheader *buffer;
    struct stat st;

    sprintf(lpathname, "%s", pathname);
    lpathname[strlen(lpathname) - 4] = '\0';
    fd = open(pathname, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Can not open .DIR file!\n");
        return 1;
    }
    (void) fstat(fd, &st);
    total = st.st_size / sizeof(struct fileheader);
    buffer =
        (struct fileheader *) calloc(total, sizeof(struct fileheader));
    if (buffer == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(-1);
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        free(buffer);
        fprintf(stderr, "Can not read .DIR file!\n");
        return 1;
    }
    (void) close(fd);

    for (index = 0; index < total; index++)
    {
        lastpostime(lpathname, buffer[index].filename);
        if (!bm)
            continue;
        if (!strcmp(buffer[index].owner, bm))
        {
            articles++;
            lastbmpostime(lpathname, buffer[index].filename);
        }
    }
    free(buffer);

    return 0;
}

/*
 * ��.PASSWDȡ������������
 * ����ֵ��Ϊ���ʾ���ܶ�.PASSWDS, �����س���.
 */
int getbmrec(struct userec *bmrec, char *currbm)
{
    FILE *fp;
    char pathname[256];

    sprintf(pathname, "%s/.PASSWDS", BBSHOME);
    fp = fopen(pathname, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Error open .PASSWDS file.\n");
        return 1;
    }

    while (1)
    {
        if (fread(bmrec, sizeof(struct userec), 1, fp) <= 0)
            break;
        if (!strcmp(bmrec->userid, currbm))
            break;
    }
    fclose(fp);

    return 0;
}

/*
 * ��ؼ�����������
 */
void bubblesort(struct boardheader *buffer, int *bsort, int total)
{
    int change, i, j;

    for (i = total - 1, change = 1; i > 1 && change; --i)
    {
        change = 0;
        for (j = 0; j < i; ++j)
            if (strcmp(buffer[bsort[j]].filename,
                       buffer[bsort[j + 1]].filename) > 0)
            {
                int temp;

                temp = bsort[j];
                bsort[j] = bsort[j + 1];
                bsort[j + 1] = temp;
                change = 1;
            }
    }

    for (i = total - 1, change = 1; i > 1 && change; --i)
    {
        change = 0;
        for (j = 0; j < i; ++j)
            if (buffer[bsort[j]].title[0] > buffer[bsort[j + 1]].title[0])
            {
                int temp;

                temp = bsort[j];
                bsort[j] = bsort[j + 1];
                bsort[j + 1] = temp;
                change = 1;
            }
    }
}

int main(void)
{
    char pathname0[256];        /* ������bmstat·���� */
    char filename0[20];         /* bmstat/�ļ��� */
    FILE *fp0;                  /* �����Զ����ĵ������� */

    int fd, index, total;       /* ��ʹ������ѭ���� */
    char pathname[256];         /* ·���� */
    struct boardheader *buffer;
    struct stat st;
    struct userec bmrec;
    int idxsort, *bsort;        /* �������� */
    static int board_num, bm_num;       /* �ܼ� */

    sprintf(pathname, "%s/.BOARDS", BBSHOME);
    fd = open(pathname, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Error open .BOARDS file!\n");
        exit(-1);
    }
    fstat(fd, &st);
    total = st.st_size / sizeof(struct boardheader);
    buffer = (struct boardheader *)
             calloc((size_t) total, sizeof(struct boardheader));
    if (buffer == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(-1);
    }
    if (read(fd, buffer, (size_t) st.st_size) < (ssize_t) st.st_size)
    {
        fprintf(stderr, "Can not read .BOARDS file!\n");
        exit(-1);
    }
    close(fd);

    board_num = 0;
    bm_num = 0;

    /* �򿪾�������д�ļ� */
    sprintf(pathname0, "%s/0Announce/bmstat/", BBSHOME);
    sprintf(filename0, "M.%ld.A", time(0));
    strcat(pathname0, filename0);
#ifdef DEBUG

    printf("pathname = %s\n", pathname0);
#endif

    fp0 = fopen(pathname0, "w");
    if (fp0 == NULL)
    {
        fprintf(stderr, "Error open file %s for write.\n", filename0);
        exit(-1);
    }
#ifndef PRINTOFF
    fprintf(fp0, "%s", head);
#endif

    /* ���� */
    bsort = (int *) malloc(total * sizeof(int));
    for (index = 0; index < total; index++)
        bsort[index] = index;
    bubblesort(buffer, bsort, total);

    /* ��ѭ�� */
    for (idxsort = 0; idxsort < total; idxsort++, index = bsort[idxsort])
    {
        int count;              /* ������������ */
        char pathname[256];
        char filename[80];      /* pathname = pathname + filename */
        char boardtitles[STRLEN];       /* �������İ���Ԥ���� */
        char boardtitle[STRLEN];        /* ��ȡ���ض�̫�������İ��� */

        char bms[BM_LEN - 4];   /* ��strtok()�� */
        char *bm;               /* ��ǰ����(��)ָ�� */
        int bmflag = 0;         /* �帱��־, 0-�帱1 , 1-�帱2 */

        if (buffer[index].level > 0)
            continue;           /* Read/Post���� */
        if ((buffer[index].flag & ANONY_FLAG) == ANONY_FLAG)
            continue;           /* ������ */
        sprintf(pathname, "%s/boards/%s/.DIR", BBSHOME,
                buffer[index].filename);
        fd = open(pathname, O_RDONLY);
        if (fd == -1)
            continue;           /* ��ȥ�½���δ�������µİ� */
        (void) fstat(fd, &st);
        /* ������������ */
        count = st.st_size / sizeof(struct fileheader);
        (void) close(fd);

        /* ���İ��� */
        strcpy(boardtitles, buffer[index].title);
        {
            char *p, *q;

            boardtitles[STRLEN - 1] = '\0';
            boardtitle[0] = '\0';
            // ȡ���İ���: ���һ���ո��ַ�������ַ���
            p = q = boardtitles;
            while (*q != '\0')
            {
                q++;
                if (*q == ' ')
                    p = q + 1;
            }
            if (*p == '\0')
                continue;
            /*
               p = strtok (boardtitles, " ");
               p = strtok (NULL, " ");
               p = strtok (NULL, "\0");
               while (*p == ' ')
               p++;
             */
            strcat(boardtitle, p);
        }

        if (strlen(boardtitle) >= 35)
        {
            boardtitle[32] = '.';
            boardtitle[33] = '.';
            boardtitle[34] = '.';
            boardtitle[35] = ')';
            boardtitle[36] = '\0';
        }
        else
        {
            strcat(boardtitle, ")");
            boardtitle[35] = '\0';
        }

        /* ���������������� */
        lastpost = 0;

        /* �������������� */
        sprintf(pathname, "%s/0Announce/", BBSHOME);
        if (!getpath(filename, buffer[index].filename))
        {
            strcat(pathname, filename);
            articles0 = 0;      /* init */
            lastmodified = 0;
            if (afilecount(pathname))
                articles0 = -1; /* ���ξ������������� */
        }
        /* else �Ҳ�����������.Search file, ������������Ч. */

        sprintf(pathname, "%s/boards/", BBSHOME);
        strcat(pathname, buffer[index].filename);
        strcat(pathname, "/.DIR");

        /* ���� */
        sprintf(bms, "%s", buffer[index].BM);
        bm = strtok(bms, " ");  /* strtok()��һ���ֽ��ִ���ANSI���� */

        /* ���ϰ��������� */
        articles = 0;
        lastbmpost = 0;
        lastNarticles = 0;
        bmfilecount(pathname, bm);      /* ���� bm == NULL */

        if (bm)
        {
            if (getbmrec(&bmrec, bm))
                continue;

            /* ��ȥ����ΪSYSOP�İ� */
            if (!strcmp(bm, "SYSOP"))
                continue;

            bm_num++;

        }
        /* ��ȥ�ް����İ� */
        /*
           else
           continue;            
         */

#ifndef PRINTOFF
        fprintf(fp0, "%-17s", buffer[index].filename);
        /*
           fprintf (fp0, " %5d", count);
         */
        fprintf(fp0, " %5d", lastNarticles);
        fprintf(fp0, " ");
        if ((time(0) - lastpost) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - lastpost) / 60 / 60 / 24 > NOFILEDAYS)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - lastpost) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld", (time(0) - lastpost) / 60 / 60 / 24);
        fprintf(fp0, " ");
        fprintf(fp0, "%6d", articles0);
        fprintf(fp0, " ");
        if ((time(0) - lastmodified) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - lastmodified) / 60 / 60 / 24 > NO0FILEDAYS)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - lastmodified) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld", (time(0) - lastmodified) / 60 / 60 / 24);
        fprintf(fp0, " ");
#endif

#ifndef PRINTOFF

        if (bm == NULL)
        {
            fprintf(fp0, "%s%s%s\n", "[1;32m", "��", "[m");
            board_num++;
#ifdef DEBUG

            fprintf(fp0, "(%-43s\n", buffer[index].title);
#else

            fprintf(fp0, "(%-43s\n", boardtitle);
#endif

            continue;           /* �����ް����İ� */
        }
        fprintf(fp0, "%-12s", bm);
        fprintf(fp0, " ");

        /*
           fprintf (fp0, "%5d", bmrec.numposts);
           fprintf (fp0, " ");
         */
        fprintf(fp0, "%6d", articles);
        fprintf(fp0, " ");
        if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 > BMNOLOGIN)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - bmrec.lastlogin) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld",
                    (time(0) - bmrec.lastlogin) / 60 / 60 / 24);
        fprintf(fp0, " ");
        if ((time(0) - lastbmpost) / 60 / 60 / 24 > 999)
            fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
        else if ((time(0) - lastbmpost) / 60 / 60 / 24 > BMNOFILEDAYS)
            fprintf(fp0, "%s%6ld%s", "[1;31m",
                    (time(0) - lastbmpost) / 60 / 60 / 24, "[m");
        else
            fprintf(fp0, "%6ld", (time(0) - lastbmpost) / 60 / 60 / 24);

        fprintf(fp0, "\n");
#endif

#ifndef PRINTOFF
#ifdef DEBUG

        fprintf(fp0, "(%-43s", buffer[index].title);
#else

        fprintf(fp0, "(%-43s", boardtitle);
#endif
#endif

        /* �帱 */
        while ((bm = strtok(NULL, " ")) != NULL)
        {
            if (getbmrec(&bmrec, bm))
            {
                fprintf(fp0, "\n");
                continue;
            }
            /* ���ϰ帱������ */
            articles = 0;
            lastbmpost = 0;
            bmfilecount(pathname, bm);

#ifndef PRINTOFF

            if (bmflag)
                fprintchar(fp0, 44, ' ');
            fprintf(fp0, " ");
            fprintf(fp0, "%-12s", bm);
            fprintf(fp0, " ");

            /*
               fprintf (fp0, "%5d", bmrec.numposts);
               fprintf (fp0, " ");
             */
            fprintf(fp0, "%6d", articles);
            fprintf(fp0, " ");
            if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 > 999)
                fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
            else if ((time(0) - bmrec.lastlogin) / 60 / 60 / 24 >
                     BMNOLOGIN)
                fprintf(fp0, "%s%6ld%s", "[1;31m",
                        (time(0) - bmrec.lastlogin) / 60 / 60 / 24, "[m");
            else
                fprintf(fp0, "%6ld",
                        (time(0) - bmrec.lastlogin) / 60 / 60 / 24);
            fprintf(fp0, " ");
            if ((time(0) - lastbmpost) / 60 / 60 / 24 > 999)
                fprintf(fp0, "%s%6d%s", "[1;32m", 999, "[m");
            else if ((time(0) - lastbmpost) / 60 / 60 / 24 > BMNOFILEDAYS)
                fprintf(fp0, "%s%6ld%s", "[1;31m",
                        (time(0) - lastbmpost) / 60 / 60 / 24, "[m");
            else
                fprintf(fp0, "%6ld",
                        (time(0) - lastbmpost) / 60 / 60 / 24);

            fprintf(fp0, "\n");
#endif

            bmflag++;
            bm_num++;
        }

#ifndef PRINTOFF
        if (!bmflag)
            fprintf(fp0, "\n");
#endif

        board_num++;
    }

    free(buffer);
    free(bsort);

#ifndef PRINTOFF

    fprintf(fp0, "---------------------------------------\n");
    fprintf(fp0, "�ܹ� %d ����, %d ������(���ظ���)�ڱ���.\n",
            board_num, bm_num);
#endif

    fclose(fp0);

    /* д������0Announce/bmstat/.Names�ļ� */
    sprintf(pathname0, "%s/0Announce/bmstat/.Names", BBSHOME);
    fp0 = fopen(pathname0, "a");
    if (fp0 == NULL)
    {
        fprintf(stderr, "Error open file %s for write.\n", ".Names");
        exit(-1);
    }
    fprintf(fp0, "%-44s%s\n", "Name=��������ͳ��", "deliver");
    fprintf(fp0, "Path=~/%s\n", filename0);
    fprintf(fp0, "Numb=1\n#\n");
    fclose(fp0);

    return 0;
}
