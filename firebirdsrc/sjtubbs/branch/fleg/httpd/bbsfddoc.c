#include "www.h"

/*show detailed uploading file,only for bm*/
int bbsfddoc_main()
{
    FILE *fp;
    char *ptr, board[30],dirbuf[256];
	int dirsize,n_read,i,start;
    struct dir x[20];
	struct shortfile *brd;
	
	
    init_all();
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_fatal("错误的讨论区");
    ptr = getparm("start");
	sprintf(dirbuf,UPLOAD_PATH2"/%s",board);
	dirsize= file_size(dirbuf) / sizeof(struct dir);
	if(0 == dirsize)
		http_fatal("上载区为空");
	
	if (ptr[0] == 0)
    {
        start = dirsize-20;
    }
    else
    {
        start = atoi(ptr);
		if(start >= dirsize)
			start = dirsize-20;
    }
	if(start<0)
		start=0;
	brd = getbcache(board);
    if (brd == 0)
    	http_fatal("内部错误10002");
    if (!has_BM_perm(&currentuser, board))
    {
         http_fatal("你无权进入管理页面");
    }
		
    fp = fopen(dirbuf, "r");
    if (fp == 0)
        http_fatal("没有找到文件");
    fseek(fp,start*sizeof(struct dir),SEEK_SET);
	n_read = fread(x,sizeof(struct dir),20,fp);
	
    printf("<center> %s -- 板内软件下载 [讨论区: %s]<hr color=green>\n",
           BBSNAME, board);
	printf("<form method=post action=bbsurdel?board=%s>",board);
    printf
    ("<table width=800 border=1><tr><td>序号<td>删<td>文件<td>上载者<td>时间<td>大小<td>说明<td>备注\n");
    for (i = 0; i < n_read ; i++)
    {
        printf("<tr><td>%d", start+1+i);
		printf("<td><input style='height:14px' type=checkbox name=del%06d%s>",start+i,x[i].showname);
		printf("<td><a href=http://"BBSHOST":"UPLOAD_SERVER_PORT"/%s/%s  target=_blank>",x[i].board,x[i].showname);
        if(isimage(x[i].showname))
			printf("<img src=http://"BBSHOST":"UPLOAD_SERVER_PORT"/%s/%s alt=%s onload=\"if(this.width>150) this.width = 150; if(this.height>150) this.height = 150\" ></a>",
					x[i].board,x[i].showname,x[i].showname);
		else	
	        printf(" %s </a>",x[i].showname);
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td><a href=bbsfexp2?fid=%d&board=%s",start+i,x[i].board);
        printf(">more </a>\n");
        printf("<td width='33%'>%s ",nohtml(x[i].exp));
    }
    printf("</table><br>\n");
	printf("<input type=submit value=\"删除\" onclick='return confirm(\"删除选择附件吗�?\")'><br>");
	printf("</form>");
    printf
    ("[<a href=bbsfddoc?board=%s&start=%d>刷新</a>] [<a href=bbsdoc?board=%s>本讨论区</a>] [<a href=bbsupload?board=%s>上载文件</a>]",
     board,start, board, board);
    printf("[<a href=bbsfddoc?board=%s&start=0>首页</a>] ", board);
    if ( 0 != start)
        printf("[<a href=bbsfddoc?board=%s&start=%d>上一页</a>] ", board,
               ((start-20)<0)?0:(start-20));
    if (start+20 < dirsize )
        printf("[<a href=bbsfddoc?board=%s&start=%d>下一页</a>] ", board,
               start+20);
    printf("[<a href=bbsfddoc?board=%s>末页</a>] ", board);
    printf
    ("<form name=form1 action=bbsfddoc?board=%s method=post onsubmit='this.start.value-=1' >\n",
     board);
    printf
    ("<input type=submit value=跳转到> 第 <input type=text name=start size=4 onkeydown=\"return true;\" >条 ");
    printf("</form>\n");
}

int isimage(char *name)
{
	char *ptr;

	ptr = strrchr(name,'.');
	if( ptr != NULL )
	{
		ptr++;
		if( ! strcasecmp(ptr,"jpg" ))
			return 1;
		else if( ! strcasecmp(ptr,"gif"))
			return 1;
		else if( ! strcasecmp(ptr,"bmp"))
			return 1;
		else if( ! strcasecmp(ptr,"png"))
			return 1;
		else if( ! strcasecmp(ptr,"jpeg"))
			return 1;
	}
	return 0;

}


