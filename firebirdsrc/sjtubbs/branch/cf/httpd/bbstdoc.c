#include "www.h"

int bbstdoc_main()
{
	char	board[80], page_str[10],filename[80];
	struct shortfile *x1;
	struct fileheader *fh, **fhs;
	int  page=-1, num=0, from, to, i, num2=0;
	
    silence = 1; /* trick to make init_all() not to print http header */
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(page_str, getparm("page"), 10);
	if(strlen(page_str)!=0)
		page = atoi(page_str);
    if (!has_read_perm(&currentuser, board))
    {
		cgi_head();
		html_head();
        http_fatal("�����������");
    }
	else
	{
		if(is_normal_board)
			cache_age = 10*60;
		cgi_head();
		html_head();
	}
	x1 = getbcache(board);
    strcpy(board, x1->filename);
	fh=(struct fileheader *)map_board_dir(board, ".DIR");
	if(fh==NULL)
        http_fatal("Ŀ¼����");
	num = get_size_of_mapdir();
	fhs = (struct fileheader **)malloc(sizeof(struct fileheader *)*num);
	for(i=0;i<num;++i)
	{
		if(fh[i].id == fh[i].reid)
		{
			fhs[num2++] = &fh[i];
		}
	}
	from = page*PAGE_SIZE;
	to = from+PAGE_SIZE;
	if(from >= num2||from < 0)
	{
		from = num2-PAGE_SIZE;
		to = num2;
	}
	else if(to > num2)
	{
		from -= (to - num2);
		to = num2;
	}	
	if(from<0)from =0;
    if (num2 == 0)
        http_fatal("��������Ŀǰû������");
	
    x1 = getbcache(board);
    strcpy(board, x1->filename);	
    printf("<body>\n");
    printf("<nobr><center>\n");
    printf
    ("%s -- �����Ķ�: [������: %s] ����[%s] ����%d, ����%d��<hr color=green>\n",
     BBSNAME, board, userid_str(x1->BM), num, num2);
    printf("<table width=670>\n");
    printf
    ("<tr bgcolor=e8e8e8><td>���<td>״̬<td>����<td width=100>����<td>����\n");
    for (i = from; i < to; i++)
    {
        printf("<tr><td>%d<td>%s<td>%s",
               i+1, flag_str(fhs[i]->accessed[0]),
               userid_str(fhs[i]->owner));
        printf("<td>%12.12s", Ctime(atoi(fhs[i]->filename + 2)) + 4);
        printf("<td><a href=bbstcon?board=%s&reid=%d>�� %s </a>",
               board, fhs[i]->reid,
               void1(nohtml2(fhs[i]->title)));
    }
    printf("</table><hr color=green>\n");    
	if (from != 0)
			printf("<a href=bbstdoc?board=%s&page=%d>��һҳ</a>,", board,
				   (from%PAGE_SIZE == 0)?(from/PAGE_SIZE - 1):(from/PAGE_SIZE));
	if (to != num2)
		printf("<a href=bbstdoc?board=%s&page=%d>��һҳ</a>,", board,
				   to/PAGE_SIZE);
    printf("<a href=bbsnot?board=%s>���廭��</a> ", board);
    printf("<a href=bbsdoc?board=%s>һ��ģʽ</a> ", board);
    printf("<a href=bbsgdoc?board=%s>��ժ��</a> ", board);
    printf("<a href=bbs0an?path=%s>������</a> ", anno_path_of(board));
    printf("<a href=bbspst?board=%s>��������</a> <br>\n", board);
    free(fhs);
    printf
    ("<form action=bbstdoc method=get onsubmit='this.page.value-=1;return true'><input type=hidden name=board value='%s'>\n",
     board);
    printf
    ("<input type=submit value=��ת��> �� <input type=text name=page size=4> ҳ");
    printf("</form>\n");
    http_quit();
}

char *stat1(struct fileheader *data, int from, int total)
{
    static char buf[256];
    char *ptr = data[from].title;
    int i, re = 0, click = *(int *) (data[from].title + 73);

    for (i = from; i < total; i++)
    {
        if (!strncmp(ptr, data[i].title + 4, 40))
        {
            re++;
            data[i].title[0] = 0;
            click += *(int *) (data[i].title + 73);
        }
    }
    sprintf(buf, "<font color=%s>%d</font>/<font color=%s>%d</font>",
            re > 9 ? "red" : "black", re, click > 499 ? "red" : "black",
            click);
    return buf;
}
