#include "www.h"

extern int loadregistdetail();

extern struct registdetail alldetail[MAX_REGIST_NUM];

int bbsregist_main()
{
    struct registdetail detail;
    int type;
    int i = 0;
    int isopen = 0, max_allow_num = 0;
    int mynum = -1;

    init_all();
    loadregistdetail(&isopen, &max_allow_num);

    if (!isopen)
    {
        printf("<body>");
        printf("��ǰ������δ�������Ѿ��رա�<br/>");
        printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
        printf("</body>");
    }
    else if (!loginok)
    {
        printf("<body>");
        printf("�Ҵҹ��Ͳ��ܱ��������ȵ�¼��<br/>");
        printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
        printf("</body>");
    }
    else if (!(currentuser.userlevel & PERM_LOGINOK))
    {
        printf("<body>");
        printf("����δͨ��ע�ᣬ���ܱ�����<br/>");
        printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
        printf("</body>");
    }
    else
    {
        type = atoi(getparm("type"));
        if (type == 2)
        {
            for (i = 1; i <= max_allow_num; i++)
            {
                if (strcmp(alldetail[i].userid, currentuser.userid) == 0)
                {
                    detail.bus = alldetail[i].bus;
                    strcpy(detail.email, alldetail[i].email);
                    detail.gender = alldetail[i].gender;
                    strcpy(detail.userid, alldetail[i].userid);
                    mynum = i;
                    break;
                }
            }
        }
        else if (type == 1)
        {
            detail.userid[0] = '\0';
            mynum = -1;
        }
        printf("<body>");
        printf
            ("<form action='bbsdoregist' method=get enctype='multipart/form-data'>\n");

        printf("<input type=\"hidden\" name=\"type\" value=\"%d\"/>",
               mynum == -1 ? 1 : 2);
        printf("<table>");
        printf("<tr>");
        printf("<td>");
        printf("��ѡ��ĺ���:");
        printf("</td>");
        printf("<td>");

        if (mynum == -1)
        {
            for (i = 1; i <= max_allow_num; i++)
                if (alldetail[i].userid[0] == '\0')
                    break;

            if (i == max_allow_num + 1)
            {
                printf("<body>");
                printf("�ܱ�Ǹ��Ŀǰ������ȫ�����ꡣ<br/>");
                printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
                printf("</body>");
                return 0;
            }
        }

        printf("<input name='number' maxlength='4' size='4' value='%d'/>",
               mynum == -1 ? i : mynum);

        printf("</td>");
        printf("</tr>");
        printf("<tr>");
        printf("<td>");
        printf("�Ƿ����У��:");
        printf("</td>");
        printf("<td>");
        printf
            ("<input type=radio name=\"bus\" value=\"0\"%s/>������У��<br/>",
             detail.bus == 0 ? " checked" : "");
        printf
            ("<input type=radio name=\"bus\" value=\"1\"%s/>��У����<br/>",
             detail.bus == 1 ? " checked" : "");
        printf
            ("<input type=radio name=\"bus\" value=\"2\"%s/>��У����<br/>",
             detail.bus == 2 ? " checked" : "");
        printf
            ("<input type=radio name=\"bus\" value=\"3\"%s/>������У��<br/>",
             detail.bus == 3 ? " checked" : "");
        printf("</td>");
        printf("</tr>");
        printf("</table>");
        printf
            ("<input type=\"button\" value='�ύ' onclick=\"verify();\"/>");
        printf("&nbsp;&nbsp;");
        printf
            ("<input type=\"button\" value='ȡ��' onclick=\"confirmback();\"/>");
        printf("</form>");
        printf("</body>");
        printf("<script language=\"javascript\">\n");
        printf("function verify()\n");
        printf("{");
        printf("var num = document.getElementsByName('number')[0].value;");
        printf(" if ( isNaN(num) ) { \n");
        printf("alert('�������Ϊ����');return false;}\n");
        printf("else if(num <= 0 || num > %d) { \n", max_allow_num);
        printf("alert('���������1-%d֮��');return false;}\n", max_allow_num);
        printf("else if(!(parseInt(num) == num)) {\n");
        printf("alert('�������Ϊ����');return false;}\n");
        printf("if(confirm(\"ȷ���ύ��?\"))");
        printf("document.forms[0].submit();}");
        printf("function confirmback(){");
        printf("if(confirm('ȷ��Ҫȡ����?'))");
        printf("window.open('bbsregistview','_self');}");
        printf("</script>\n");
    }
    return 0;
}
