#include "www.h"

int bbsfdel_main()
{
    FILE *fp;
    int i, total = 0;
    char path[80], userid[80];
    struct override f[200];

    init_all();
    if (!loginok)
        http_quit_print("����δ��¼�����ȵ�¼");
    sprintf(path, "home/%c/%c/%s/friends", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    printf
        ("<center><font class=\"title\">%s - ������������</font>[ʹ����: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    strsncpy(userid, getparm("userid"), 13);
    if (userid[0] == 0)
    {
        printf("<form action=bbsfdel>\n");
        printf("��������ɾ���ĺ����ʺ�: <input type=text><br>\n");
        printf("<input type=submit>\n");
        printf("</form>");
        http_quit();
    }
    loadfriend(currentuser.userid);
    if (friendnum <= 0)
        http_quit_print("��û���趨�κκ���");
    if (!isfriend(userid))
        http_quit_print("���˱����Ͳ�����ĺ���������");
    for (i = 0; i < friendnum; i++)
    {
        if (strcasecmp(fff[i].id, userid))
        {
            memcpy(&f[total], &fff[i], sizeof(struct override));
            total++;
        }
    }
    fp = fopen(path, "w");
    fwrite(f, sizeof(struct override), total, fp);
    fclose(fp);
    printf
        ("[%s]�Ѵ����ĺ���������ɾ��.<br>\n <a href=bbsfall>���غ�������</a>",
         userid);
    printf("</html>");
    return 0;
}
