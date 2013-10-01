#include "www.h"
#include <sys/sendfile.h>
static int no_re = 0;

int do_log2()
{
    int i;
    FILE *fp;

    fp = fopen("wwwlog3.log", "a");
    for (i = 0; i < 100 && environ[i]; i++)
    {
        fprintf(fp, "%s\n", environ[i]);
    }
    fclose(fp);
}
int do_cache(char *filename,char *cachefilename)
{
	FILE *fp,*fpcache;
	char buf[512],*ptr;
	int swap;	
	fp = fopen(filename, "r");
    fpcache = fopen(cachefilename,"w");

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
                    fprintf(fpcache,"������: %s", userid_str(id));
                    hfprintf(fpcache,"%s", s);
                    free(ptr);
                    continue;
                }
                free(ptr);
            }
            if (!strncmp(buf, "--", 2))
                pic_mod = 1;
            if (!strncmp(buf, ": ", 2))
            {
                fprintf(fpcache,"<font color=808080>");
                swap = pic_mod;
                pic_mod = 0;
            }
            hhfprintf(fpcache,"%s", void1(buf));
            if (!strncmp(buf, ": ", 2))
            {
                fprintf(fpcache,"</font>");
                pic_mod = swap;
            }
        }
    fclose(fp);
    fclose(fpcache);
}
int bbscon_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80],*ptr;
    char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];

    struct fileheader x;
    int num, total;
    unsigned int jsreid;
    int swap;

    //added by cf,caching web content
    FILE *fpcache;
    struct stat originfile,cachefile;
    char cachefilename[80];
    int cachestatus;
    off_t offset=0;
    //added by hongliang
    int	start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);

    //added by hongliang
    start	= atoi(getparm("start"));

    num = atoi(getparm("num"));
    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    //u_info->board=getbnum(board);
    printf("<font class=\"title\">%s - �����Ķ�����</font>[������: %s]<hr>", BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("����Ĳ���1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("����Ĳ���2");
    sprintf(dir, "boards/%s/.DIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("�������������ڻ���Ϊ��");
    //{add by alt@sjtu for ���λ���վ����

    printf("[<a href=bbsfwd?board=%s&file=%s>ת��/�Ƽ�</a>]", board, file);
    printf("[<a href=bbsccc?board=%s&file=%s>ת��</a>]", board, file);
    printf
    ("[<a onclick='return confirm(\"�����Ҫɾ��������?\")' href=bbsdel?board=%s&file=%s>ɾ��</a>]",
     board, file);
    printf("[<a href=bbsedit?board=%s&file=%s>�޸�</a>]", board, file);
    printf("[<a href=bbsdocnore?board=%s&file=%s&start=%d&num=%d>���ÿ�RE����</a>]", board, file, start, num);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>��һƪ</a>]",
               board, prefilename, num - 1, start);
    }

    //added by hongliang
    printf("[<a href=\"bbsdoc?board=%s&start=%d&currentpoint=%d\">����������</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>��������ĩҳ</a>]", board);
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>��һƪ</a>]",
               board, nextfilename, num + 1, start);
    }
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        (*(int *) (x.title + 73))++;
        if (*(int *) (x.title + 73) > 1000000)
            (*(int *) (x.title + 73)) = 0;
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
        brc_init(currentuser.userid, board);
        brc_add_read(x.filename);
        brc_update(currentuser.userid, board);
    }
    fclose(fp);
    strcpy(title, x.title);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    ptr[60] = 0;
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>������</a>]",
               board, file, x.reid);
    printf("[<a href='bbstfind0?board=%s&title=%s'>ͬ�����б�</a>]", board,
           nohtml5(void3(void1(ptr))));
    printf("[<a href='bbstfind?board=%s&title=%s'>ͬ�����Ķ�</a>]", board,
           nohtml5(void3(void1(ptr))));


    //added by hongliang for javascript
    printf("<script language=\"javascript\" src=\""UPLOAD_SERVER_HEADER"/key_hl_viewforum.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", num - start);
    printf("var board = \"%s\";\n", board);
    printf("var start = %d;\n", start);
    printf("var filename = \"%s\";\n", file);
    printf("var title= \"%s\";\n", nohtml5(void3(void1(ptr))));
    printf("var prefilename = \"%s\";\n", prefilename);
    printf("var nextfilename= \"%s\";\n", nextfilename);
    printf("var jsreid = %d;\n", x.reid);
    printf("\n-->\n</script>\n");


    printf
	    ("<table class=\"show_border\" width=610>\n");
    //printf("<tr><td style='font-size:14px'>\n<pre>");
    printf("<tr><td>\n<pre>");
    fp = fopen(dir, "r+");
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
    sprintf(filename, "boards/%s/%s", board, file);
    //add by cf,cache web content.2004.08.10
    sprintf(cachefilename,"cache/%s",board);
    if(stat(cachefilename,&cachefile) == -1)
        mkdir(cachefilename,0755);
    sprintf(cachefilename,"cache/%s/%s",board,file);
    if(stat(filename,&originfile) == -1)
        http_fatal("���Ĳ����ڻ����ѱ�ɾ��");
    if(stat(cachefilename,&cachefile) == -1)
        cachestatus=0;
    else
    {
        if(cachefile.st_mtime > originfile.st_mtime)
            cachestatus=1;
        else
            cachestatus=0;
    }

    if(cachestatus == 0)
    {
	    do_cache(filename,cachefilename);
        cachestatus = 1;
    }
    if(cachestatus == 1)
    {
        fpcache=fopen(cachefilename,"r");

        while(1)
        {
            /*	if(sendfile(fileno(stdout),fileno(fpcache),&offset,512) == -1)
            		break;
            */
            int r;
            r = fread(buf, 1, 512, fpcache);
            if (r <= 0)
                break;
            fwrite(buf, 1, r, stdout);

        }
        fclose(fpcache);
    }

    printf("</pre>\n</table><hr>\n");
    printf("[<a href=bbsfwd?board=%s&file=%s>ת��/�Ƽ�</a>]", board, file);
    printf("[<a href=bbsccc?board=%s&file=%s>ת��</a>]", board, file);
    printf
    ("[<a onclick='return confirm(\"�����Ҫɾ��������?\")' href=bbsdel?board=%s&file=%s>ɾ��</a>]",
     board, file);
    printf("[<a href=bbsedit?board=%s&file=%s>�޸�</a>]", board, file);
    printf("[<a href=bbsdocnore?board=%s&file=%s&start=%d&num=%d>���ÿ�RE����</a>]", board, file, start, num);
    if (num > 0)
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>��һƪ</a>]",
               board, prefilename, num - 1, start);

    //added by hongliang
    printf("[<a href=\"bbsdoc?board=%s&start=%d&currentpoint=%d\">����������</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>��������ĩҳ</a>]", board);
    if (num < total - 1)
        printf("[<a href=bbscon?board=%s&file=%s&num=%d&start=%d>��һƪ</a>]",
               board, nextfilename, num + 1, start);
    if (!no_re)
        printf("[<a href='bbspst?board=%s&file=%s&reid=%d'>������</a>]",
               board, file, x.reid);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    printf("[<a href='bbstfind0?board=%s&title=%s'>ͬ�����б�</a>]", board,
           nohtml5(void3(void1(ptr))));
    printf("[<a href='bbstfind?board=%s&title=%s'>ͬ�����Ķ�</a>]", board,
           nohtml5(void3(void1(ptr))));
    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    http_quit();
}


int bbscondel_main()
{
    FILE *fp;
    char buf[512], board[80], dir[80], file[80], filename[80], *ptr;
    char prefilename[STRLEN], nextfilename[STRLEN], title[STRLEN];
    struct fileheader x;
    int num, total;
    int swap;

    //added by hongliang
    int	start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);

    //added by hongliang
    start	= atoi(getparm("start"));

    num = atoi(getparm("num"));
    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    //u_info->board=getbnum(board);
    printf("<font class=\"title\">%s - �ѱ�ɾ�������Ķ� [����������վ: %s]</font><hr>", BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("����Ĳ���1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("����Ĳ���2");

    //added by hongliang
    if (!has_BM_perm(&currentuser, board))
        http_fatal("�Բ�����û�лָ����µ�Ȩ��");

    sprintf(dir, "boards/%s/.RETURNDIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("�������������ڻ���Ϊ��");

    printf("[<a href=bbsdelreturn?board=%s&file=%s>�ָ�O</a>]", board, file);
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num > 0)
    {
        fseek(fp, sizeof(x) * (num - 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(prefilename, x.filename);
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>��һƪK</a>]",
               board, prefilename, num - 1, start);
    }

    //added by hongliang
    printf("[<a href=\"bbsdocdel?board=%s&start=%d&currentpoint=%d\">��������������վ��</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>��������</a>]", board);
    if (num < total - 1)
    {
        fseek(fp, sizeof(x) * (num + 1), SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        strcpy(nextfilename, x.filename);
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>��һƪJ</a>]",
               board, nextfilename, num + 1, start);
    }
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        (*(int *) (x.title + 73))++;
        if (*(int *) (x.title + 73) > 1000000)
            (*(int *) (x.title + 73)) = 0;
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
        brc_init(currentuser.userid, board);
        brc_add_read(x.filename);
        brc_update(currentuser.userid, board);
    }
    fclose(fp);
    strcpy(title, x.title);
    ptr = title;
    if (!strncmp(ptr, "Re: ", 4))
        ptr += 4;
    ptr[60] = 0;

    //added by hongliang for javascript
    printf("<script language=\"javascript\" src=\""UPLOAD_SERVER_HEADER"/key_hl_viewdelforum.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", num - start);
    printf("var board = \"%s\";\n", board);
    printf("var start = %d;\n", start);
    printf("var filename = \"%s\";\n", file);
    printf("var title= \"%s\";\n", nohtml5(void3(void1(ptr))));
    printf("var prefilename = \"%s\";\n", prefilename);
    printf("var nextfilename= \"%s\";\n", nextfilename);
    printf("var jsreid = %d;\n", x.reid);
    printf("\n-->\n</script>\n");

    printf
    ("<table width=610 style='BORDER: 2px solid; BORDER-COLOR: D0F0C0;'>\n");
    //printf("<tr><td style='font-size:14px'>\n<pre>");
    printf("<tr><td>\n<pre>");
    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num < 0)
        num = 0;
    fseek(fp, sizeof(x) * (num), SEEK_SET);
    fread(&x, sizeof(x), 1, fp);
    if (strcmp(x.filename, file) != 0)
        http_fatal("���Ĳ����ڻ��ѱ��ָ�");
    fclose(fp);

    sprintf(filename, "boards/%s/%s", board, file);
    fp = fopen(filename, "r");
    if (fp == 0)
        http_fatal("���Ĳ����ڻ����ѱ���ȫɾ��");
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

    printf("[<a href=bbsdelreturn?board=%s&file=%s>�ָ�O</a>]", board, file);
    if (num > 0)
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>��һƪK</a>]",
               board, prefilename, num - 1, start);

    //added by hongliang
    printf("[<a href=\"bbsdocdel?board=%s&start=%d&currentpoint=%d\">��������������վ��</a>]", board, start, num-start);

    printf("[<a href=bbsdoc?board=%s>��������</a>]", board);
    if (num < total - 1)
        printf("[<a href=bbscondel?board=%s&file=%s&num=%d&start=%d>��һƪJ</a>]",
               board, nextfilename, num + 1, start);
    printf("</center>\n");
    if (has_BM_perm(&currentuser, board))
        printf("<p>");
    http_quit();
}


int bbsdelreturn_main()
                {
                    FILE *fp;
                    struct shortfile *brd;
                    struct fileheader f;
                    struct userec *u;
                    char buf[80], dir[80], path[80], board[80], file[80], *id, *p, UTitle[128];
                    int num = 0;
                    int i;

                    init_all();
                    if (!loginok)
                        http_fatal("���ȵ�¼");
                    id = currentuser.userid;
                    strsncpy(board, getparm("board"), 60);
                    strsncpy(file, getparm("file"), 20);
                    if (!strcmp(board, "deleted"))
                        http_fatal("���岻�ָܻ���������");
                    brd = getbcache(board);
                    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
                        http_fatal("����Ĳ���");
                    if (strstr(file, ".."))
                        http_fatal("����Ĳ���");
                    if (brd == 0)
                        http_fatal("�������");
                    if (!has_post_perm(&currentuser, board))
                        http_fatal("�����������");
#ifdef	POST_AFTER_ONE_DAY
                    if(too_young(&currentuser, board))
                        http_fatal("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ��巢������.������������� BBSHelp ���� sysop �巢����ѯ��");
#endif
                    sprintf(dir, "boards/%s/.RETURNDIR", board);
                    sprintf(path, "boards/%s/%s", board, file);
                    fp = fopen(dir, "r");
                    if (fp == 0)
                        http_fatal("����Ĳ���");
                    while (1)
                    {
                        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
                            break;
                        if (!strcmp(f.filename, file))
                        {
                            if (strcasecmp(id, f.owner)
                                    && !has_BM_perm(&currentuser, board))
                                http_fatal("����Ȩ�ָ�����");

                            strcpy(UTitle, f.title);
                            if (p = strrchr(UTitle, '-'))
                            { /* create default article title */
                                *p = 0;
                                for (i = strlen(UTitle) - 1; i >= 0; i --)
                                {
                                    if (UTitle[i] != ' ')
                                        break;
                                    else
                                        UTitle[i] = 0;
                                }
                            }
                            strncpy(f.title,UTitle,STRLEN);
                            f.accessed[0] = 0;

                            return_del_article(dir, sizeof(struct fileheader), num);
                            post_article3(board, f.title, file, f.owner, f.id, f.reid);

                            printf
                            ("�ָ��ɹ�.<br><a href='bbsdocdel?board=%s'>���ر�����������վ</a>",
                             board);
                            u = getuser(f.owner);
                            if (!junkboard(brd->filename) && u)
                            {
                                if (u->numposts > 0)
                                    u->numposts++;
                                save_user_data(u);
                            }
                            sprintf(buf, "%s %-12s bbsdel %s\n", Ctime(time(0)) + 4, id,
                                    board);
                            f_append("trace", buf);
                            {
                                char buf[256];

                                sprintf(buf, "%24.24s %s %s Do_Web_D_RETURN\n", Ctime(time(0)),
                                        currentuser.userid, board);
                                f_append("bm2.log", buf);
                            }
                            http_quit();
                        }
                        num++;
                    }
                    fclose(fp);
                    printf("�ļ�������, �ָ�ʧ��.<br>\n");
                    printf("<a href='bbsdocdel?board=%s'>���ر�����������վ</a>", board);
                    http_quit();
                }

                void post_article3(char *board, char *title, char *file, char *id,
                                   unsigned int currid, unsigned int reid)
                {
                    FILE *fp;
                    char buf[1024];
                    struct fileheader header;

                    bzero(&header, sizeof(header));
                    strcpy(header.owner, id);
                    header.id = currid;
                    header.reid = reid;
                    sprintf(header.filename, "%s", file);
                    strsncpy(header.title, title, 60);
                    header.accessed[0] = 0;
                    sprintf(buf, "boards/%s/.DIR", board);
                    fp = fopen(buf, "a");
                    fwrite(&header, sizeof(header), 1, fp);
                    fclose(fp);
                }
