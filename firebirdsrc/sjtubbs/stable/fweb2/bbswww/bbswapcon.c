#include "www.h"
#include "bbs.h"

int bbswapcon_main()
{

    char buf[512], board[80], file[80], filename[80];

    struct fileheader *pfh, *pre, *next;
    int num;
    FILE *fpcache;
    struct stat originfile, cachefile;
    char cachefilename[80];
    int cachestatus, no_re;

    no_re = 0;
    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("�����������");
    }
    else
    {
        if (is_normal_board)
            cache_age = 5 * 60;
        cgi_head();
        html_head();
        print_style();
    }
    printf("<body>");
    strcpy(board, getbcache(board)->filename);
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    printf
        ("<font class=\"title\">%s - �����Ķ�����</font>[������: %s]<hr>",
         BBSNAME, board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("����Ĳ���1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_quit_print("����Ĳ���2");
    if (map_board_dir(board, DOT_DIR) == 0)
        http_quit_print("�������������ڻ���Ϊ��");
    pfh = find_in_map_dir(file);
    num = get_offset_of_mapdir();
    if (pfh == NULL)
    {        
        unmap_board_dir();
        if (map_board_dir(board, RETURN_DIR) == 0)
            http_quit_print("���Ĳ�����");
        pfh = find_in_map_dir(file);
        if (pfh == NULL)
        {
            unmap_board_dir();
            http_quit_print("���Ĳ�����");
        }
        if (strcmp(pfh->owner, currentuser.userid) && !has_BM_perm(&currentuser, board))
        {
            unmap_board_dir();
            http_quit_print("���Ĳ�����");
        }
        num = get_offset_of_mapdir();
    }
	pre = pre_in_map_dir();
#ifdef STATIC_URL
    if (pre != NULL)
        printf("[<a href=bbswapcon,board,%s,file,%s.html>��һƪ</a>]",
               board, pre->filename);
    next = next_in_map_dir();
    if (NULL != next)
        printf("[<a href=bbswapcon,board,%s,file,%s.html>��һƪ</a>]",
               board, next->filename);
    printf("[<a href=\"bbswapdoc,board,%s,page,%d.html\">����������</a>]<br>",
           board, num / PAGE_SIZE);

#else
    if (pre != NULL)
        printf("[<a href=bbswapcon?board=%s&file=%s>��һƪ</a>]",
               board, pre->filename);
    next = next_in_map_dir();
    if (NULL != next)
        printf("[<a href=bbswapcon?board=%s&file=%s>��һƪ</a>]",
               board, next->filename);
    printf("[<a href=\"bbswapdoc?board=%s&page=%d\">����������</a>]<br>", board,
           num / PAGE_SIZE);

#endif
    brc_init(currentuser.userid, board);
    brc_add_read(pfh->filename);
    brc_update(currentuser.userid, board);

    if (!no_re)
        printf("[<a href='bbswappst?board=%s&file=%s&reid=%d'>������</a>]",
               board, file, pfh->reid);
#ifdef STATIC_URL
    printf("[<a href='bbswaptcon,board,%s,reid,%d.html'>ͬ�����Ķ�</a>]",
           board, pfh->reid);
    printf
        ("[<a href='bbswaptcon,board,%s,reid,%d,file,%s.html'>������չ��</a>]",
         board, pfh->reid, pfh->filename);
#else
    printf("[<a href='bbswaptcon?board=%s&reid=%d'>ͬ�����Ķ�</a>]", board,
           pfh->reid);
    printf("[<a href='bbswaptcon?board=%s&reid=%d&file=%s'>������չ��</a>]",
           board, pfh->reid, pfh->filename);
#endif

    printf("<pre>");
    sprintf(filename, "boards/%s/%s/%s", board, getfolderstr(file), file);
    sprintf(cachefilename, "cache/%s/%s/%s", board, getfolderstr(file),
            file);
    if (stat(filename, &originfile) == -1)
    {
        unmap_board_dir();
        http_quit_print("���Ĳ����ڻ����ѱ�ɾ��");
    }
    if (stat(cachefilename, &cachefile) == -1)
        cachestatus = 0;
    else
    {
        if (cachefile.st_mtime > originfile.st_mtime)
            cachestatus = 1;
        else
            cachestatus = 0;
    }

    if (cachestatus == 0)
    {
        do_cache(filename, cachefilename);
        cachestatus = 1;
    }
    fpcache = fopen(cachefilename, "r");

    while (1)
    {
        int r;

        r = fread(buf, 1, 512, fpcache);
        if (r <= 0)
            break;
        fwrite(buf, 1, r, stdout);
    }
    fclose(fpcache);

    printf("</pre><hr>\n");
#ifdef STATIC_URL
    if (NULL != pre)
        printf("[<a href=bbswapcon,board,%s,file,%s.html>��һƪ</a>]",
               board, pre->filename);

    if (NULL != next)
        printf("[<a href=bbswapcon,board,%s,file,%s.html>��һƪ</a>]",
               board, next->filename);
    printf("[<a href=\"bbswapdoc,board,%s,page,%d.html\">����������</a>]<br>",
        board, get_offset_of_mapdir() / PAGE_SIZE);

    if (!no_re)
        printf("[<a href='bbswappst?board=%s&file=%s&reid=%d'>������</a>]",
               board, file, pfh->reid);
    printf("[<a href='bbswaptcon,board,%s,reid,%d.html'>ͬ�����Ķ�</a>]",
           board, pfh->reid);
    printf
        ("[<a href='bbswaptcon,board,%s,reid,%d,file,%s.html'>������չ��</a>]",
         board, pfh->reid, pfh->filename);
#else
    if (NULL != pre)
        printf("[<a href=bbswapcon?board=%s&file=%s>��һƪ</a>]",
               board, pre->filename);
    if (NULL != next)
        printf("[<a href=bbswapcon?board=%s&file=%s>��һƪ</a>]",
               board, next->filename);
    printf("[<a href=\"bbswapdoc?board=%s&page=%d\">����������</a>]<br>", board,
           get_offset_of_mapdir() / PAGE_SIZE);

    if (!no_re)
        printf("[<a href='bbswappst?board=%s&file=%s&reid=%d'>������</a>]",
               board, file, pfh->reid);
    printf("[<a href='bbswaptcon?board=%s&reid=%d'>ͬ�����Ķ�</a>]", board,
           pfh->reid);
    printf("[<a href='bbswaptcon?board=%s&reid=%d&file=%s'>������չ��</a>]",
           board, pfh->reid, pfh->filename);
#endif

    unmap_board_dir();
    printf("\n</html>");
    return 0;

}