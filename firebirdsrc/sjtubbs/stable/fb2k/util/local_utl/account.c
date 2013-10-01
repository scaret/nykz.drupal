/* account.c  -- count for no. of logins */
/* $Id: account.c,v 1.2 2004/06/29 14:59:22 rwzmm Exp $ */

#include <time.h>
#include <string.h>
#include <stdio.h>
#include "bbs.h"
#define MAX_LINE        (15)
char datestring[30];

struct
{
    int no[24];                 /* ���� */
    int sum[24];                /* �ܺ� */
}
st;

/* Added by deardragon 1999.12.2 */
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

int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    char buf[256], *p;
    char date[80];
    int now;
    int hour, max = 0, item, total = 0;
    int totaltime = 0;
    int i, j;
	int use_stdio = 0;
    char *blk[10] = {
                        "��", "��", "�x", "�y", "�z",
                        "�{", "�|", "�}", "�~", "��",
                    };
	if( argc > 1)
	{
		if(2 == argc && strlen(argv[1])==1 && argv[1][0] == '-')
		{
			use_stdio = 1;
		}
		else
		{
			fprintf(stderr,"\
unknown parameters.\n\
usage: %s [-]\n\
       -    get data from stdin and output to stdout\n",argv[0]);
			exit(1);
		}
	}
	if(use_stdio)
	{
		fp = stdin;
	}
	else
	{
	    sprintf(buf, "%s/usies", BBSHOME);
	    if ((fp = fopen(buf, "r")) == NULL)
	    {
	        fprintf(stderr,"can't open usies\n");
	        return 1;
	    }
	}
    now = time(0);
    getdatestring(now);
    sprintf(date, "%14.14s", datestring);
    while (fgets(buf, 256, fp))
    {
        hour = atoi(buf + 14);
        if (hour < 0 || hour > 23)
        {
            fprintf(stderr,"%s", buf);
            continue;
        }
        if (strncmp(buf, date, 14))
            continue;
        if (!strncmp(buf + 30, "ENTER", 5))
        {
            st.no[hour]++;
            continue;
        }
        if (p = (char *) strstr(buf + 49, "Stay:"))
        {
            st.sum[hour] += atoi(p + 6);
            continue;
        }
    }
    fclose(fp);

    for (i = 0; i < 24; i++)
    {
        total += st.no[i];
        totaltime += st.sum[i];
        if (st.no[i] > max)
            max = st.no[i];
    }

    if (max != 0)
        item = max / MAX_LINE + 1;
    else
    {
        /*		fprintf(stderr,"max is 0.\n"); */
        exit(0);
    }
	if(use_stdio)
	{
		fp = stdout;
	}
	else
	{
	    sprintf(buf, "%s/0Announce/bbslist/countusr", BBSHOME);
	    if ((fp = fopen(buf, "w")) == NULL)
	    {
	        fprintf(stderr,"Cann't open countusr\n");
	        return 1;
	    }
	}
    fprintf(fp,
            "\n[1;36m    ����������������������������������������������������������������������������\n");
    for (i = MAX_LINE; i >= 0; i--)
    {
        fprintf(fp, "[1;37m%4d[36m��[33m", (i + 1) * item);
        for (j = 0; j < 24; j++)
        {
            if ((item * (i) > st.no[j]) && (item * (i - 1) <= st.no[j])
                    && st.no[j])
            {
                fprintf(fp, "[35m%-3d[33m", (st.no[j] % 1000));
                continue;
            }
            if (st.no[j] - item * i < item && item * i < st.no[j])
                fprintf(fp, "%s ",
                        blk[((st.no[j] - item * i) * 10) / item]);
            else if (st.no[j] - item * i >= item)
                fprintf(fp, "%s ", blk[9]);
            else
                fprintf(fp, "   ");
        }
        fprintf(fp, "[1;36m��\n");
    }

    fprintf(fp,
            "   [37m0[36m������ [37m%-12.12sÿСʱ�����˴�ͳ��[36m����[37m%s[36m������\n"
            "    [;36m  00 01 02 03 04 05 06 07 08 09 10 11 [1;31m12 13 14 15 16 17 18 19 20 21 22 23\n\n"
            "                 [32m1 [33m�~ [32m= [37m%-5d [32m�ܹ���վ�˴Σ�[37m%-9d[32mƽ��ʹ��ʱ�䣺[37m%d[m\n",
            BBSNAME, datestring, item, total,
            (totaltime == 0) ? 0 : totaltime / total + 1);
    fclose(fp);
}
