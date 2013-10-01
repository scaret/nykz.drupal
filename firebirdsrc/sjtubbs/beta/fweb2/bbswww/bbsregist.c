#include "www.h"

#define MAXREGIST (2500)

extern int loadregistdetail();

struct registdetail
{
	char id[IDLEN + 1];
	char email[REAL_NAME_EMAIL_LEN];
	int num;
	int gender;
	int bus;	// 0: ��; 1: ��, 2: ��; 3: both
};

extern struct registdetail alldetail[MAXREGIST];

int
bbsregist_main()
{
    FILE *stream, *fp;
    char bm_file_path[36],email[REAL_NAME_EMAIL_LEN];
	struct registdetail detail;
	int type;
	int i = 0;
	char *status_file = "vote/regist_status";
	int regIsOpen = 0;

	init_all();

	if ((fp = fopen(status_file, "r")) != NULL){
		fscanf(FCGI_ToFILE(fp), "%d", &regIsOpen);
		fclose(fp);
	}

	if (!regIsOpen){
		printf("<body>");
		printf("��ǰ������δ�������Ѿ��رա�<br/>");
		printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
		printf("</body>");
	}
	else if (!loginok)	// guest ���ܱ���
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
		loadregistdetail();
		type = atoi(getparm("type"));
		if (type == 2)	// �޸�
		{
			for (i = 0; i < MAXREGIST; i++)
			{
				if (strcmp(alldetail[i].id, currentuser.userid) == 0)
				{
					detail.bus = alldetail[i].bus;
					strcpy(detail.email, alldetail[i].email);
					detail.gender = alldetail[i].gender;
					strcpy(detail.id, alldetail[i].id);
					detail.num = alldetail[i].num;
					break;
				}
			}
		}
		else if (type == 1)	// ����
		{
			detail.id[0] = '\0';
			detail.num = 0;
		}
		sprintf(bm_file_path,"home/%c/%c/%s/real_email",toupper(currentuser.userid[0]),toupper (currentuser.userid[1]), currentuser.userid);
		if ((stream=fopen(bm_file_path,"r"))==NULL)
			sprintf(email, "NO_REAL_EMAIL");
		else
		{
			fgets(email,32,stream);
			fclose(stream);
		}
		printf("<body>");
		printf("<form action='bbsdoregist' method=post enctype='multipart/form-data'>\n");
		//type: 1: ��; 2: ��; 3: ɾ
		printf("<input type=\"hidden\" name=\"type\" value=\"%d\"/>", detail.num > 0 ? 2 : 1);
		printf("<input type=\"hidden\" name=\"email\" value=\"%s\"/>", email);
		printf("<input type=\"hidden\" name=\"id\" value=\"%s\"/>", currentuser.userid);
		printf("<input type=\"hidden\" name=\"gender\" value=\"%c\"/>", currentuser.gender);
		printf("<table>");
		printf("<tr>");
		printf("<td>");
		printf("��ѡ��ĺ���:");
		printf("</td>");
		printf("<td>");

		if (detail.num == 0)
		{
			for (i = 0; i < MAXREGIST; i++)
				if (alldetail[i].id[0] == '\0')
					break;
			
			if (i == MAXREGIST)
			{
				printf("<body>");
				printf("�ܱ�Ǹ��Ŀǰ������ȫ�����ꡣ<br/>");
				printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
				printf("</body>");
				return 0;
			}
		}

		printf("<input name='number' maxlength='4' size='4' value='%d'/>", detail.num == 0? (i+1) : detail.num);

		printf("</td>");
		printf("</tr>");
		printf("<tr>");
		printf("<td>");
		printf("�Ƿ����У��:");
		printf("</td>");
		printf("<td>");
		printf("<input type=radio name=\"bus\" value=\"0\"%s/>������У��<br/>", detail.bus == 0 ? " checked" : "");
		printf("<input type=radio name=\"bus\" value=\"1\"%s/>��У����<br/>", detail.bus == 1 ? " checked" : "");
		printf("<input type=radio name=\"bus\" value=\"2\"%s/>��У����<br/>", detail.bus == 2 ? " checked" : "");
		printf("<input type=radio name=\"bus\" value=\"3\"%s/>������У��<br/>", detail.bus == 3 ? " checked" : "");
		printf("</td>");
		printf("</tr>");
		printf("</table>");
		printf("<input type=\"button\" value='�ύ' onclick=\"verify();\"/>");
		printf("&nbsp;&nbsp;");
		printf("<input type=\"button\" value='ȡ��' onclick=\"confirmback();\"/>");
		printf("</form>");
		printf("</body>");
		printf("<script language=\"javascript\">\n");
		printf("function verify()\n");
		printf("{");
		printf("var num = document.getElementsByName('number')[0].value;");
		printf(" if ( isNaN(num) ) { \n");
		printf("alert('�������Ϊ����');return false;}\n");
		printf("else if(num <= 0 || num > 2500) { \n");
		printf("alert('���������1-2500֮��');return false;}\n");
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

