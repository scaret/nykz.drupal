#include "www.h"

int bbsfall_main()
{
    int i;

    init_all();
    if (!loginok)
        http_quit_print("����δ��¼, ���ȵ�¼");
    loadfriend(currentuser.userid);
    printf("<center>\n");
    printf
        ("<font class=\"title\">%s - ������������</font>[ʹ����: %s]<hr><br>\n",
         BBSNAME, currentuser.userid);
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
    printf("</table><hr>\n");
    printf("[<a href=bbsfadd>����µĺ���</a>]</center>\n");
    printf("\n</html>");
    return 0;
}
