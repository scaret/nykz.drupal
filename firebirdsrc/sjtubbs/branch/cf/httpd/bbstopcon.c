#include "www.h"

static int no_re = 0;

int bbstopcon_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    /*char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];*/
    struct fileheader x;
    int num, total;
    int swap;

	silence = 1; /* trick to make init_all() not to print http header */
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    if (!has_read_perm(&currentuser, board))
    {
		cgi_head();
		html_head();
        http_fatal("�����������");
    }
	else
	{
		if(is_normal_board)
			cache_age = 30*60;
		cgi_head();
		html_head();
	}
    printf("<body>");
    printf("<center>\n");
    strcpy(board, getbcache(board)->filename);
    //u_info->board=getbnum(board);
    printf("%s -- �����Ķ� [������: %s]<hr color=green>", BBSNAME, board);
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("����Ĳ���2");
//    sprintf(dir, "boards/%s/.TOPFILE", board);
//    total = file_size(dir) / sizeof(x);
//    if (total <= 0)
//        http_fatal("�������������ڻ���Ϊ��");
    //{add by alt@sjtu for ���λ���վ����
    printf("[<a href=bbsdoc?board=%s>��������</a>]", board);
    printf
    ("<table width=610 style='BORDER: 2px solid; BORDER-COLOR: D0F0C0;'>\n");
    //printf("<tr><td style='font-size:14px'>\n<pre>");
    printf("<tr><td>\n<pre>");
/*  fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num < 0)
        num = 0;
    fseek(fp, sizeof(x) * (num), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    if (strcmp(x.filename, file) != 0)
        http_fatal("���Ĳ����ڻ��ѱ�ɾ��,���߼���ɾ���ڻ���վ��");
    fclose(fp);
    //}
    */
    sprintf(filename, "boards/%s/%s", board, file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
    while (1)
    {
        char *id, *s;

        if (fgets(buf, 512, fp) == 0)
            break;
        if (!strncmp(buf, "������: ", 8))
        {
            ptr = strdup(buf);
            id = strtok(ptr + 8, " ");
            s = strtok(0, "");
            if (id == 0)
                id = " ";
            if (s == 0)
                s = "\n";
            if (strlen(id) < 13 && getuser(id))
            {
                printf("������: %s", userid_str(id));
                hprintf("%s", s);
                free(ptr);
                continue;
            }
            free(ptr);
        }
        if (!strncmp(buf, "--", 2))
            pic_mod = 1;
        if (!strncmp(buf, ": ", 2))
        {
            printf("<font color=808080>");
            swap = pic_mod;
            pic_mod = 0;
        }
        hhprintf("%s", void1(buf));
        if (!strncmp(buf, ": ", 2))
        {
            printf("</font>");
            pic_mod = swap;
        }
    }
    fclose(fp);
    printf("</pre>\n</table><hr color=green>\n");
    printf("[<a href=bbsdoc?board=%s>��������</a>]", board);
    printf("</center>\n");
    http_quit();
}
