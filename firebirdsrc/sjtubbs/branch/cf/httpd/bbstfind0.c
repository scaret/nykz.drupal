#include "www.h"
#define TOPIC_MAX 1000
int bbstfind0_main()
{
    char  board[80], reid_str[16];
    struct fileheader *fh, *fhs[1000];
    int num = 0, offset,no_re = 0;
	unsigned int reid = 0;
	
	silence = 1;
	init_all();
	strsncpy(board, getparm("board"), 32);
	strsncpy(reid_str, getparm("reid"), 16);
	if (!has_read_perm(&currentuser, board))
	{
		cgi_head();
		html_head();
		http_fatal("�����������");
	}
	else
	{
		if(is_normal_board)
			cache_age = 3*60;
		cgi_head();
		html_head();
	}
	if(strlen(reid_str)!=0)
		reid = atoi(reid_str);
	else
		http_fatal("�������reid");	
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
	fh=(struct fileheader *)map_board_dir(board, ".DIR");
	if(fh==NULL)
        http_fatal("Ŀ¼����");
	offset = get_size_of_mapdir()-1;
	for(;offset>=0;--offset)
		if(fh[offset].reid == reid)
		{
			fhs[num++] = &fh[offset];
			if(fh[offset].id == reid||num==TOPIC_MAX)
				break;
		}
	if(num == 0)
		http_fatal("�����ⲻ����");

    printf
    ("<center>%s -- ͬ������� [������: %s] <hr color=green>\n",
     BBSNAME, board);
    printf
    ("<table width=610><tr bgcolor=e8e8e8><td>���<td>����<td>����<td>����");
    for (offset=num-1;offset>=0;--offset)
    {
		struct fileheader *p;
		p=fhs[offset];
        printf("<tr><td>%d", num-offset);
        printf("<td>%s", userid_str(p->owner));
        printf("<td>%6.6s", Ctime(atoi(p->filename + 2)) + 4);
        printf("<td><a href=bbscon?board=%s&file=%s>%s</a>",
                   board, p->filename, void1(p->title));
    }
    printf("</table><hr color=green>\n���ҵ� %d ƪ\n", num);
    printf("<a href=bbsdoc?board=%s>��������</a> ", board);
    printf("<a href=bbstcon?board=%s&reid=%d>������ȫ��չ��</a> ",
               board,fhs[0]->reid);
    http_quit();
}
