#include "www.h"

int bbssec_main()
{
    int i;

    init_all();
    printf("<link rel=stylesheet type=text/css href='http://bbs.sjtu.edu.cn:8000/TechGroup/1100423301201530.css'>\n");
    printf("<body topmargin=0 leftmargin=0>");
    printf
    ("<style type=text/css>A {font-size:12x; color: #0000f0}</style>");
    printf("<center><br>\n");
    printf("<font class=\"title\">%s - �����������б�</font><hr>\n", BBSNAME);
    printf("<table>\n");
    printf("<tr bgcolor='e8e8e8'><td>����<td>���<td>����\n");
    for (i = 0; i < SECNUM; i++)
    {
        printf("<tr><td>%c<td><a href=bbsboa?sec=%d>%s</a>",
               i < 10 ? i + '0' : i + 'A' - 10, i, secname[i][0]);
        printf("<td><a href=bbsboa?sec=%d>%s</a>\n", i, secname[i][1]);
    }
    printf("</table>\n");
    printf("<hr>\n");
    printf("[<a href=bbsall>ȫ��������</a>]");
    printf("[<a href=bbs0an>����������</a>]");
    printf("[<a href=bbsusr>ʹ�����б�</a>]");
    printf("</center></td>\n");
    printf("</center>\n");

    http_quit();
}
