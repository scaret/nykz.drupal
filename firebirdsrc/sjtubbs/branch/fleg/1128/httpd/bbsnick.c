#include "www.h"

int bbsnick_main()
{
    int i;
    unsigned char nick[80];

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ����޷��ı��ǳ�");
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
        if (nick[i] < 32 || nick[i] == 255)
            nick[i] = ' ';
    strsncpy(u_info->username, nick, 32);
    printf("��ʱ����ǳƳɹ�");
}
