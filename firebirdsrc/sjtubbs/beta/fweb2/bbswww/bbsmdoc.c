#include "www.h"

int
bbsmdoc_main()
{
	FILE *fp;
	char board[80], dir[80];
	struct shortfile *x1;
	struct fileheader x;
	int i, start, total;

	init_all();
	strsncpy(board, getparm("board"), 32);
	x1 = getbcache(board);
	if (x1 == 0)
		http_quit_print("�����������");
	strcpy(board, x1->filename);
	if (!has_read_perm(&currentuser, board))
		http_quit_print("�����������");
	if (!has_BM_perm(&currentuser, board))
		http_quit_print("��û��Ȩ�޷��ʱ�ҳ");
	sprintf(dir, "boards/%s/.DIR", board);
	fp = fopen(dir, "r");
	if (fp == 0)
		http_quit_print("�����������Ŀ¼");
	total = file_size(dir) / sizeof(struct fileheader);
	start = atoi(getparm("start"));
	if (start == 0)
	{
		char buf[256];

		sprintf(buf, "%24.24s %s %s W_MDoc\n", Ctime(time(0)),
			currentuser.userid, board);
		f_append("bm2.log", buf);
	}
	if (strlen(getparm("start")) == 0 || start > total - 20)
		start = total - 20;
	if (start < 0)
		start = 0;
	printf("<nobr><center>\n");
	printf("%s -- [������: %s] ����[%s] ������[%d]<hr color=green>\n",
	       BBSNAME, board, userid_str(x1->BM), total);
	if (total <= 0)
	{
		fclose(fp);
		http_quit_print("��������Ŀǰû������");
	}
	printf("<form name=form1 method=post action=bbsman>\n");
	printf("<table width=613>\n");
	printf("<tr><td>���<td>����<td>״̬<td>����<td>����<td>����\n");
	fseek(fp, start * sizeof(struct fileheader), SEEK_SET);
	for (i = 0; i < 20; i++)
	{
		char filename[80];

		if (fread(&x, sizeof(x), 1, fp) <= 0)
			break;
		sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(x.filename), x.filename);
		printf("<tr><td>%d", start + i + 1);
		printf("<td><input style='height:14px' name=box%s type=checkbox>", x.filename);
		printf("<td>%s<td>%s", flag_str(x.accessed[0]),
		       userid_str(x.owner));
		printf("<td>%12.12s", Ctime(atoi(x.filename + 2)) + 4);
		printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%s%36.36s </a>", board, x.filename, start + i, strncmp(x.title, "Re: ", 4) ? "�� " : "", void1(nohtml(x.title)));
	}
	printf("</table>\n");
	printf("<input type=hidden name=mode value=''>\n");
	printf("<input type=hidden name=start value='%d'>\n", start);
	printf("<input type=hidden name=board value='%s'>\n", board);
	printf("<input type=button value=ɾ�� onclick='document.form1.mode.value=1; document.form1.submit();'>\n");
	printf("<input type=button value=��M onclick='document.form1.mode.value=2; document.form1.submit();'>\n");
	printf("<input type=button value=��G onclick='document.form1.mode.value=3; document.form1.submit();'>\n");
	printf("<input type=button value=����Re onclick='document.form1.mode.value=4; document.form1.submit();'>\n");
	printf("<input type=button value=���MG onclick='document.form1.mode.value=5; document.form1.submit();'>\n");
	printf("</form>\n");
	if (start > 0)
	{
		printf("<a href=bbsmdoc?board=%s&start=%d>��һҳ</a> ",
		       board, start < 20 ? 0 : start - 20);
	}
	if (start < total - 20)
	{
		printf("<a href=bbsmdoc?board=%s&start=%d>��һҳ</a> ",
		       board, start + 20);
	}
	printf("<a href=bbsdoc?board=%s>һ��ģʽ</a> ", board);
	printf("<a href=bbsdenyall?board=%s>��������</a> ", board);
	printf("<a href=bbsmnote?board=%s>�༭���廭��</a> ", board);
	printf("<a href=bbsmnote3?board=%s>�༭��������</a> ", board);

	//added by hongliang for recylce bin on Nov. 21th, 2003 21:12
	printf("<a href=bbsdocdel?board=%s>����վ</a> ", board);

	fclose(fp);
	printf("<form action=bbsmdoc method=post onsubmit='this.start.value-=1'>\n");
	printf("<input type=hidden name=board value=%s>\n",board);
	printf("<input type=submit value=��ת��> �� <input type=text name=start size=4> ƪ");
	printf("</form>\n");
	printf("</html>");
	return  0;

}
