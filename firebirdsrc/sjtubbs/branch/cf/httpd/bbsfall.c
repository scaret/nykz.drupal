#include "www.h"

int bbsfall_main()
{
    int i;

    init_all();
    if (!loginok)
        http_fatal("����δ��¼, ���ȵ�¼");
    loadfriend(currentuser.userid);
    printf("<center>\n");
    printf("%s -- �������� [ʹ����: %s]<hr color=green><br>\n", BBSNAME,
           currentuser.userid);
    printf("�����趨�� %d λ����<br>", friendnum);
    printf
    ("<table border=1><tr><td>���<td>���Ѵ���<td>����˵��<td>ɾ������");
    for (i = 0; i < friendnum; i++)
    {
        printf("<tr><td>%d", i + 1);
        printf("<td><a href=bbsqry?userid=%s>%s</a>", fff[i].id,
               fff[i].id);
        printf("<td>%s\n", nohtml(fff[i].exp));
        printf
        ("<td>[<a onclick='return confirm(\"ȷʵɾ����?\")' href=bbsfdel?userid=%s>ɾ��</a>]",
         fff[i].id);
    }
    printf("</table><hr color=green>\n");
    printf("[<a href=bbsfadd>����µĺ���</a>]</center>\n");
    http_quit();
}
