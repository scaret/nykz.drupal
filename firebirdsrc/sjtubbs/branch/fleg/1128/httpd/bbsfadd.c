#include "www.h"

int bbsfadd_main()
{
    FILE *fp;
    char path[80], userid[80], exp[80];

    init_all();
    if (!loginok)
        http_fatal("����δ��¼�����ȵ�¼");
    sprintf(path, "home/%c/%s/friends", toupper(currentuser.userid[0]),
            currentuser.userid);
    printf("<center><font class=\"title\">%s - ������������</font>[ʹ����: %s]<hr>\n",
           BBSNAME, currentuser.userid);
    strsncpy(userid, getparm("userid"), 13);
    strsncpy(exp, getparm("exp"), 32);
    loadfriend(currentuser.userid);
    if (userid[0] == 0 || exp[0] == 0)
    {
        if (userid[0])
            printf("<font color=red>���������˵��</font>");
        printf("<form action=bbsfadd>\n");
        printf
        ("������������ĺ����ʺ�: <input type=text name=userid value='%s'><br>\n",
         userid);
        printf("�������������ѵ�˵��: <input type=text name=exp>\n",
               exp);
        printf("<br><input type=submit value=ȷ��></form>\n");
        http_quit();
    }
    if (!getuser(userid))
        http_fatal("�����ʹ�����ʺ�");
    if (friendnum >= 199)
        http_fatal("���ĺ��������Ѵﵽ����, ���������µĺ���");
    if (isfriend(userid))
        http_fatal("�����Ѿ�����ĺ�����������");
    strcpy(fff[friendnum].id, getuser(userid)->userid);
    strcpy(fff[friendnum].exp, exp);
    friendnum++;
    fp = fopen(path, "w");
    fwrite(fff, sizeof(struct override), friendnum, fp);
    fclose(fp);
    printf
    ("[%s]�Ѽ������ĺ�������.<br>\n <a href=bbsfall>���غ�������</a>",
     userid);
    http_quit();
}