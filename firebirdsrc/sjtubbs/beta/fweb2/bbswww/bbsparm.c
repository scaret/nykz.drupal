#include "www.h"

char *defines[] = {
	"�������ر�ʱ���ú��Ѻ���",	/* DEF_FRIENDCALL */
	"���������˵�ѶϢ",	/* DEF_ALLMSG */
	"���ܺ��ѵ�ѶϢ",	/* DEF_FRIENDMSG */
	"�յ�ѶϢ��������",	/* DEF_SOUNDMSG */
	"ʹ�ò�ɫ",		/* DEF_COLOR */
	"��ʾ�����",		/* DEF_ACBOARD */
	"��ʾѡ����ѶϢ��",	/* DEF_ENDLINE */
	"�༭ʱ��ʾ״̬��",	/* DEF_EDITMSG */
	"ѶϢ������һ��/����ģʽ",	/* DEF_NOTMSGFRIEND */
	"ѡ������һ��/����ģʽ",	/* DEF_NORMALSCR */
	"������������ New ��ʾ",	/* DEF_NEWPOST */
	"�Ķ������Ƿ�ʹ���ƾ�ѡ��",	/* DEF_CIRCLE */
	"�Ķ������α�ͣ춵�һƪδ��",	/* DEF_FIRSTNEW */
	"��վʱ��ʾ��������",	/* DEF_LOGFRIEND */
	"������վ֪ͨ",		/* DEF_LOGINFROM */
	"�ۿ����԰�",		/* DEF_NOTEPAD */
	"��Ҫ�ͳ���վ֪ͨ������",	/* DEF_NOLOGINSEND */
	"����ʽ����",		/* DEF_THESIS */
	"�յ�ѶϢ�Ⱥ��Ӧ�����",	/* DEF_MSGGETKEY */
	"��վʱ�ۿ���վ�˴�ͼ",	/* DEF_GRAPH */
	"��վʱ�ۿ�ʮ�����а�",	/* DEF_TOP10 */
	"ʹ������ǩ����",	/* DEF_RANDSIGN */
	"��ʾ����",		/* DEF_S_HOROSCOPE */
	"����ʹ����ɫ����ʾ�Ա�",	/* DEF_COLOREDSEX */
	"ȱʡת������Ϊվ������",	/* DEF_FMAIL */
	"ʹ��\'+\'���δ������",	/* DEF_NOT_N_MASK */
	"��������ɾ��",		/* DEF_DELDBLCHAR */
	"�Զ��Ű���Ԥ��Ϊ 78 ��",	/* DEF_AUTOWRAP */
	"ʹ��GB���Ķ�",		/* DEF_USEGB KCN 99.09.03 */
	NULL
};
static int
read_form()
{
	int i, perm = 1, def = 0;
	char var[100];

	for (i = 0; i < 32; i++)
	{
		sprintf(var, "perm%d", i);
		if (strlen(getparm(var)) == 2)
			def += perm;
		perm = perm * 2;
	}
	currentuser.userdefine = def;
	save_user_data(&currentuser);
	printf("���˲������óɹ�.<br><a href=bbsparm>���ظ��˲�������ѡ��</a>");
	return 0;
}
int
bbsparm_main()
{
	int i, perm = 1, type;

	init_all();
	type = atoi(getparm("type"));
	printf("<center><font class=\"title\">%s - �޸ĸ��˲�������</font>[ʹ����: %s]<hr>\n", BBSNAME, currentuser.userid);
	if (!loginok)
		http_quit_print("�Ҵҹ��Ͳ����趨����");
	if (type)
		return read_form();
	printf("<form action=bbsparm method=post>\n");
	printf("<input type=hidden name=type value=1>\n");
	printf("<table width=610>\n");
	for (i = 0; defines[i]; i++)
	{
		char *ptr = "";

		if (i % 2 == 0)
			printf("<tr>\n");
		if (currentuser.userdefine & perm)
			ptr = " checked";
		printf("<td><input type=checkbox name=perm%d%s><td>%s", i,
		       ptr, defines[i]);
		perm = perm * 2;
	}
	printf("</table><br><hr>");
	printf("<input type=submit value=ȷ��></form>���ϲ���������telnet��ʽ�²�������\n");
	printf("</html>");
	return 0;
}


