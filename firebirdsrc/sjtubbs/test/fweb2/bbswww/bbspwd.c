#include "www.h"

int bbspwd_main()
{
    int type;
    char pw1[20], pw2[20], pw3[20];

    init_all();
    if (!loginok)
        http_quit_print("����δ��¼, ���ȵ�¼");
    type = atoi(getparm("type"));
    if (type == 0)
    {
        printf
            ("<center><font class=\"title\">%s - �޸����롡��</font>[ʹ����: %s]<hr>\n",
             BBSNAME, currentuser.userid);
        printf("<form action=bbspwd method=post>\n");
        printf("<input type=hidden name=type value=1>\n");
        printf("<table>\n");
        printf("<tr><td>ʹ�����ʺ�: <td>%s\n", currentuser.userid);
        printf
            ("<tr><td>��ľ�����: <td><input maxlength=12 size=12 type=password name=pw1>\n");
        printf
            ("<tr><td>���������: <td><input maxlength=12 size=12 type=password name=pw2>\n");
        printf
            ("<tr><td>������һ��: <td><input maxlength=12 size=12 type=password name=pw3>\n");
        printf("</table><hr>\n");
        printf("<input type=submit value=ȷ��>\n");
        http_quit();
    }
    strsncpy(pw1, getparm("pw1"), 13);
    strsncpy(pw2, getparm("pw2"), 13);
    strsncpy(pw3, getparm("pw3"), 13);
    if (strcmp(pw2, pw3))
        http_quit_print("�������벻��ͬ");
    if (strlen(pw2) < 2)
        http_quit_print("������̫��");
    if (!checkpasswd(currentuser.passwd, pw1))
        http_quit_print("���벻��ȷ");
    strsncpy(currentuser.passwd, crypt_passwd(pw2), 14);
    save_user_data(&currentuser);
    printf("[%s] �����޸ĳɹ�.", currentuser.userid);
    printf("</html>");
    return 0;
}
