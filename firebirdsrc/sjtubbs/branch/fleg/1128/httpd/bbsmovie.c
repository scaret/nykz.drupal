#include "www.h"

int bbsmovie_main()
{
    FILE *fp;
    char buf[120][256], buf2[80];
    int total = 0, num, i;

    init_all();
    num = atoi(getparm("num"));
    fp = fopen("etc/movie", "r");
    if (fp == 0)
        http_fatal("��վ�������δ����");
    while (total < 120)
    {
        if (fgets(buf[total], 255, fp) == 0)
            break;
        total++;
    }
    fclose(fp);
    if (num > total / 5 - 1)
        num = total / 5 - 1;
    if (num < 0)
        num = 0;
    if (total >= 5)
    {
        sprintf(buf2, "bbsmovie?num=%d", (num + 1) % (total / 5));
        refreshto(buf2, 10);
    }
    printf("<center><font class=\"title\">%s - ����塡��</font>[��%d/%dҳ]<hr>\n", BBSNAME,
           num + 1, total / 5);
    printf("<table><tr><td><pre>");
    printf
    ("<font color=red>���������������������������� С �� �� �� �� �� �� ����������������������������</font>\n");
    for (i = num * 5; i <= num * 5 + 4; i++)
        hhprintf("%s", void1(buf[i]));
    printf
    ("<font color=red>������������������������������������������������������������������������������</font>\n");
    printf("</pre></table><hr>\n");
    printf("<div align=right>");
    printf("(10���Զ�ˢ��) ");
    for (i = 0; i < total / 5; i++)
    {
        if (i != num)
        {
            printf("<a href=bbsmovie?num=%d>[%d]</a>", i, i + 1);
        }
        else
        {
            printf("[%d]", i + 1);
        }
    }
    printf("</div>");
}