#include "www.h"

static int check_info()
{
    unsigned int m;
    char buf[256];

    strsncpy(buf, getparm("nick"), 30);
    for (m = 0; m < strlen(buf); m++)
        if ((buf[m] < 32 && buf[m] > 0) || buf[m] == -1)
            buf[m] = ' ';
    if (strlen(buf) > 1)
    {
        strcpy(currentuser.username, buf);
    }
    else
    {
        printf("����: �ǳ�̫��!<br>\n");
    }
    strsncpy(buf, getparm("realname"), 9);
    if (strlen(buf) > 1)
    {
        strcpy(currentuser.realname, buf);
    }
    else
    {
        printf("����: ��ʵ����̫��!<br>\n");
    }
    strsncpy(buf, getparm("address"), 40);
    if (strlen(buf) > 8)
    {
        strcpy(currentuser.address, buf);
    }
    else
    {
        printf("����: ��ס��ַ̫��!<br>\n");
    }
    strsncpy(buf, getparm("email"), 32);
    if (strlen(buf) > 8 && strchr(buf, '@'))
    {
        strcpy(currentuser.email, buf);
    }
    else
    {
        printf("����: email��ַ���Ϸ�!<br>\n");
    }
    strsncpy(buf, getparm("year"), 5);
    if (atoi(buf) > 1910 && atoi(buf) < 1998)
    {
        currentuser.birthyear = atoi(buf) - 1900;
    }
    else
    {
        printf("����: ����ĳ������!<br>\n");
    }
    strsncpy(buf, getparm("month"), 3);
    if (atoi(buf) > 0 && atoi(buf) <= 12)
    {
        currentuser.birthmonth = atoi(buf);
    }
    else
    {
        printf("����: ����ĳ����·�!<br>\n");
    }
    strsncpy(buf, getparm("day"), 3);
    if (atoi(buf) > 0 && atoi(buf) <= 31)
    {
        currentuser.birthday = atoi(buf);
    }
    else
    {
        printf("����: ����ĳ�������!<br>\n");
    }
    strsncpy(buf, getparm("gender"), 2);
    if (!strcasecmp(buf, "F"))
        currentuser.gender = 'F';
    if (!strcasecmp(buf, "M"))
        currentuser.gender = 'M';
    strsncpy(buf, getparm("exptype"), 3);
    if (!strcasecmp(buf, "E1"))
        currentuser.user_exp_type = 0;
    if (!strcasecmp(buf, "E2"))
        currentuser.user_exp_type = 1;
    if (!strcasecmp(buf, "E3"))
        currentuser.user_exp_type = 2;
    if (!strcasecmp(buf, "E4"))
        currentuser.user_exp_type = 3;
    save_user_data(&currentuser);
    printf("[%s] ���������޸ĳɹ�.", currentuser.userid);
    return 0;
}

int bbsinfo_main()
{
    int type;

    init_all();
    if (!loginok)
        http_quit_print("����δ��¼");
    type = atoi(getparm("type"));
    printf
        ("<center><font class=\"title\">%s - ���ø������ϡ���</font>[ʹ����: %s]<hr>\n",
         BBSNAME, currentuser.userid);

    if (type != 0)
    {
        check_info();
        http_quit();
    }
    printf("<form action=bbsinfo method=post>");
    printf("<input type=hidden name=type value=1>\n");
    printf("<table>\n");
    printf("<tr><td>�����ʺţ� <td>%s\n", currentuser.userid);
    printf
        ("<tr><td>�����ǳƣ� <td><input type=text name=nick value='%s' size=24 maxlength=30>\n",
         currentuser.username);
    printf("<tr><td>��������� <td>%d ƪ<br>\n", currentuser.numposts);
    printf("<tr><td>�ż������� <td>%d ��<br>\n", currentuser.nummails);
    printf("<tr><td>��վ������ <td>%d ��<br>\n", currentuser.numlogins);
    printf("<tr><td>��վʱ�䣺 <td>%ld ����<br>\n", currentuser.stay / 60);
    printf
        ("<tr><td>��ʵ������ <td><input type=text name=realname value='%s' size=16 maxlength=16>\n",
         currentuser.realname);
    printf
        ("<tr><td>��ס��ַ�� <td><input type=text name=address value='%s' size=40 maxlength=40>\n",
         currentuser.address);
    printf("<tr><td>�ʺŽ����� <td>%s\n", Ctime(currentuser.firstlogin));
    printf("<tr><td>������٣� <td>%s\n", Ctime(currentuser.lastlogin));
    printf("<tr><td>��Դ��ַ�� <td>%s\n", currentuser.lasthost);
    printf
        ("<tr><td>�����ʼ��� <td><input type=text name=email value='%s' size=32 maxlength=32>\n",
         currentuser.email);
    printf
        ("<tr><td>�������ڣ� <td><input type=text name=year value=%d size=4 maxlength=4>��",
         currentuser.birthyear + 1900);
    printf("<input type=text name=month value=%d size=2 maxlength=2>��",
           currentuser.birthmonth);
    printf("<input type=text name=day value=%d size=2 maxlength=2>��\n",
           currentuser.birthday);
    printf("<tr><td>�����Ա� ");
    printf("<td>��<input type=radio value=M name=gender %s>, ",
           currentuser.gender == 'M' ? "checked" : "");
    printf("Ů<input type=radio value=F name=gender %s>\n",
           currentuser.gender == 'F' ? "checked" : "");

    printf("</table><hr>\n");
    printf
        ("<input type=submit value=ȷ��> <input type=reset value=��ԭ>\n");
    printf("</form>");
    printf("</center>\n");
    printf("</html>");
    return 0;
}
