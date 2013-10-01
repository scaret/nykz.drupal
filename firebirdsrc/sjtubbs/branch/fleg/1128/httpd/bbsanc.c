#include "www.h"

int bbsanc_main()
{
    FILE *fp;
    char *board, path[512], buf[512];

    init_all();
    strsncpy(path, getparm("path"), 511);
    board = getbfroma(path);
    buf[0] = 0;
    if (board[0] && getbcache(board)
            && !has_read_perm(&currentuser, board))
        http_fatal("������ļ���");
    if (board[0])
        sprintf(buf, "%s��", board);
    printf("<center><font class=\"title\">%s - %s�����������Ķ�</font><hr>\n", BBSNAME, board);
    if (strstr(path, ".Search") || strstr(path, ".Names")
            || strstr(path, "..") || strstr(path, "./"))
        http_fatal("������ļ���");
    sprintf(buf, "0Announce%s", path);
    printf
	    ("<table class=\"show_border\" width=610>");
    //printf("<tr><td><pre style='font-size:14px'>");
    printf("<tr><td><pre>");
    fp = fopen(buf, "r");
    if (fp == 0)
        http_fatal("������ļ���");
    while (1)
    {
        if (fgets(buf, 256, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre></table><hr>\n");
    printf("[<a href='javascript:history.go(-1)'>������һҳ</a>] ");
    if (board[0])
    {
        if (!strncasecmp(board, "p_", 2))
            printf("[<a href=bbs0an?path=%s>%s�ļ���ҳ</a>]",
                   anno_path_of(board), board + 2);
        else
            printf("[<a href=bbsdoc?board=%s>��������</a>]", board);
    }
    printf("</center>\n");
    http_quit();
}
