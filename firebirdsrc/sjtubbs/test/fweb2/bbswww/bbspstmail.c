#include "www.h"

int bbspstmail_main()
{
    FILE *fp;
    int i;
    char userid[80], buf[512], path[512], file[512], title[80] = "";

    init_all();
    if (!loginok)
        http_quit_print("�Ҵҹ��Ͳ���д�ţ����ȵ�¼");
    if (!HAS_PERM(PERM_LOGINOK))
        http_quit_print("��Ǹ, �㱻����˷���Ȩ��, �޷�����.");
    if (!HAS_PERM(PERM_POST) || !HAS_PERM(PERM_PERSONAL))
        http_quit_print("��Ǹ, �㱻����˷���Ȩ��, �޷�����.");
    strsncpy(file, getparm("file"), 20);
    strsncpy(title, nohtml(getparm("title")), 50);
    strsncpy(userid, getparm("userid"), 40);
    if (file[0] != 'M' && file[0])
        http_quit_print("������ļ���");
    if (strchr(file, '/'))
        http_quit_print("������ļ���");
    printf("<center>\n");
    printf
        ("<font class=\"title\">%s - �����Ÿ롡��</font>[ʹ����: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    printf("<table class=show_border bgcolor=f0f0f0><tr><td>\n");
    printf("<form method=post action=bbssndmail>\n");
    printf
        ("�ż�����: <input type=text name=title size=40 maxlength=100 value='%s'> ",
         title);
    printf("������: &nbsp;%s<br>\n", currentuser.userid);
    printf("������: &nbsp;&nbsp<input type=text name=userid value='%s'>",
           nohtml(userid));
    printf("  ʹ��ǩ���� ");
    printf("<input type=radio name=signature value=1 checked>1\n");
    printf("<input type=radio name=signature value=2>2\n");
    printf("<input type=radio name=signature value=3>3\n");
    printf("<input type=radio name=signature value=4>4\n");
    printf("<input type=radio name=signature value=5>5\n");
    printf("<input type=radio name=signature value=0>0\n");
    printf("����<input type=checkbox name=backup>\n");
    printf("<br>\n");
    printf("<textarea name=text rows=20 cols=80 wrap=physicle>\n\n");
    if (file[0])
    {
        int lines = 0;

        printf("�� �� %s ���������ᵽ: ��\n", userid);
        sprintf(path, "mail/%c/%c/%s/%s", toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid, file);
        fp = fopen(path, "r");
        if (fp)
        {
            for (i = 0; i < 4; i++)
                if (fgets(buf, 500, fp) == 0)
                    break;
            while (1)
            {
                if (fgets(buf, 500, fp) == 0)
                    break;
                if (!strncmp(buf, ": ��", 4))
                    continue;
                if (!strncmp(buf, ": : ", 4))
                    continue;
                if (!strncmp(buf, "--\n", 3))
                    break;
                if (buf[0] == '\n')
                    continue;
                ;
                if (!strcasestr(buf, "</textarea>"))
                    printf(": %s", buf);
                if (lines++ > 20)
                {
                    printf(": (��������ʡ�� ... ...)");
                    break;
                }
            }
            fprintf(fp, "\n");
            fclose(fp);
        }
    }
    printf("</textarea><br><br><div align=center>\n");
    printf("<input type=submit value=�����ż�> ");
    printf("<input type=reset value=���ݸ�ԭ></form>\n");
    printf("</div></table>");
    printf("</html>");
    return 0;
}
