#include "www.h"

int bbsnick_main()
{
    int i;
    char nick[80];

    init_all();
    if (!loginok)
        http_quit_print("�Ҵҹ����޷��ı��ǳ�");
    strsncpy(nick, getparm("nick"), 30);
    if (nick[0] == 0)
    {
        printf
            ("<center><font class=\"title\">%s - ��ʱ�ı��ǳ�(�����ķ���Ч)����</font>[ʹ����: %s]</center><hr>\n",
             BBSNAME, currentuser.userid);
        printf
            ("<form action=bbsnick>���ǳ�<input name=nick size=24 maxlength=24 type=text value='%s'> \n",
             void1(u_info->username));
        printf("<input type=submit value=ȷ��>");
        printf("</form>");
        http_quit();
    }
    for (i = 0; nick[i]; i++)
        if ((nick[i] > 0 && nick[i] < '\x20') || nick[i] == '\xFF')
            nick[i] = ' ';
    strsncpy(u_info->username, nick, 32);
    printf("��ʱ����ǳƳɹ�");
    printf("</html>");
    return 0;
}
