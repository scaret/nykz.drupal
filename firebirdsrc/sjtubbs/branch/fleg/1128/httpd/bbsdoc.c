#include "www.h"

int is_my_brd(char *board)
{
    //deleted by hongliang on Nov.20, 2003 7:20 because these variables are not in use
    //    FILE *fp;
    int i, mybrdnum = 0;
    char /*buf[256],*/ mybrd[GOOD_BRC_NUM][80];

    if (!loginok)
        return 0;
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    for (i = 0; i < mybrdnum; i++)
        if (!strcasecmp(board, mybrd[i]))
        {
            return 1;
        }
    return 0;
}

int bbsdoc_main()
{
    FILE	*fp,*top_fp;
    char	board[80], dir[80];
    struct shortfile *x1;
    struct fileheader x;
    int	i, start, total, width = 720;
    int	top_total;
    //added by hongliang
    int	currentpoint;

    init_all();
    if (my_font_mode == 0)
        width = 670;
    strsncpy(board, getparm("board"), 32);

    //added by hongliang
    currentpoint	= atoi(getparm("currentpoint"));
    if ( currentpoint < 0 )
    {
        currentpoint = 0;
    }

    x1 = getbcache(board);
    if (x1 == 0)
        http_fatal("�����������");
    strcpy(board, x1->filename);
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    //u_info->board=getbnum(board);

    printf("<body>");
    printf
    ("<form name=form1 action=bbsdoc?board=%s method=post onsubmit='this.start.value-=1'>\n",
     board);
    printf("<center>\n");
    printf("<table><tr><td>");
    printf
    ("<td><font class=\"title\">%s(%s)</font></table>\n",
     x1->filename, x1->title + 11);
    sprintf(dir, "boards/%s/.DIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("�����������Ŀ¼");
    total = file_size(dir) / sizeof(struct fileheader);
    if (total <= 0)
        http_fatal("��������Ŀǰû������");
    sprintf(dir, "boards/%s/.TOPFILE", board);
    top_fp = fopen(dir, "r");
    top_total = 0;
    if (top_fp != 0)
        top_total = file_size(dir) / sizeof(struct fileheader);

    start = atoi(getparm("start"));
    if (my_t_lines < 10 || my_t_lines > 40)
        my_t_lines = 20;
    my_t_lines = my_t_lines - top_total;

    //added by hongliang
    if ( currentpoint < 0)
    {
        currentpoint	= my_t_lines + start;
    }
    if ( currentpoint >= my_t_lines )
    {
        currentpoint	= my_t_lines - 1;
    }

    if (strlen(getparm("start")) == 0 || start > total - my_t_lines)
        start = total - my_t_lines;
    if (start < 0)
        start = 0;
    if (strlen(getparm("start")) == 0)
        b_usage(board, 33);
    brc_init(currentuser.userid, board);
    printf("<nobr>\n");
    //      printf("<table width=%d><tr style='height:14px'><td align=left> ����: %s, ��������: %d��.",
    //              width, userid_str(x1->BM), count_on_board(board));

    //added by hongliang
    //write the global variables of javascript
    printf("<script language=\"javascript\" src=\""UPLOAD_SERVER_HEADER"/key_hl_forumlist.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", currentpoint);
    printf("var totalid = %d;\n", top_total + my_t_lines - 1);
    printf("var toptotal = %d;\n", top_total);
    printf("var arrayid = new Array(totalid);\n");
    printf("var board = \"%s\";\n", board);
    printf("var boardpath= \"%s\";\n", anno_path_of(board));
    printf("var start = %d;\n", start);
    printf("var total = %d;\n", total);
    printf("if ( total < totalid ) \n\ttotalid = total - 1;\n");
    printf("\n-->\n</script>\n");

    printf
    ("<table width=%d><tr style='height:14px'><td align=left width=50%> ����: %s",
     width, userid_str(x1->BM));
    printf("<td align=right width=25%>");
    if (start > 0)
        printf("<a href=bbsdoc?board=%s&start=%d>��</a>,", board,
               start - my_t_lines);
    if (start < total - my_t_lines)
        printf("<a href=bbsdoc?board=%s&start=%d>��</a>,", board,
               start + my_t_lines);
    if (!is_my_brd(board))
    {
        printf
        ("<a onclick='return confirm(\"����ȷҪ����������Ԥ����������\")' href=bbsbrdadd?board=%s>���ı���</a>\n",
         board);
    }
    else
    {
        printf
        ("<a onclick='return confirm(\"����ȷҪ��������Ԥ����������ɾ����\")' href=bbsbrddel?board=%s>ȡ������</a>\n",
         board);
    }
	printf(",<a href=\"bbsrss?board=%s\"> <img border=0 src=\" http://bbs.sjtu.edu.cn:8000/bbs/rss.gif\"></a>",board);
    printf("<td align=right width=25%>");
    printf
    ("<input type=submit value=��ת��> �� <input type=text onkeydown=\"return true;\" name=start size=4> ƪ");
    printf("</table>\n");
    show_bm_words(board, width);
    printf("\n<hr>\n");
    printf("<table width=%d>\n", width);
    //printf("<tr bgcolor='e8e8e8'><td>���<td>״̬<td>����<td width=72>����<td>����<td>����\n");

    //add a column to display '>' by hongliang on Nov. 20th, 2003 6:50
    printf
    ("<tr bgcolor='e8e8e8'><td>���<td>״̬<td>����<td width=100>����<td width=12>&nbsp;<td>����<td>����\n");
    fseek(fp, start * sizeof(struct fileheader), SEEK_SET);
    for (i = 0; i < my_t_lines; i++)
    {
        char filename[80];
        char *ptr, *font1 = "", *font2 = "";

        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        ptr = flag_str2(x.accessed[0], brc_has_read(x.filename));
        if (ptr[0] == 'N')
        {
            font1 = "<font color=#909090>";
            font2 = "</font>";
        }
        sprintf(filename, "boards/%s/%s", board, x.filename);
        printf("<tr><td>%d<td>%s%s%s<td>%s",
               start + i + 1, font1, ptr, font2, userid_str(x.owner));
        printf("<td>%12.12s", Ctime(atoi(x.filename + 2)) + 4);

        //added by hongliang
        printf("<td><span id=\"arrow%d\">", i);
        if ( i == currentpoint )
        {
            printf("&gt;");	//Display the '>'
        }
        printf("</span>");

        /*printf("<td><a href=bbscon?board=%s&file=%s&num=%d>%s%s </a>%s",
                      board, x.filename, start + i,
                      void1(nohtml2(x.title)), eff_size(filename));*/
        //modified by acdsee,add x flag
        printf("<td><a href=bbscon?board=%s&file=%s&num=%d&start=%d>%s%s%s </a>%s",
               board, x.filename, start + i, start,
               (x.accessed[0]&FILE_NOREPLY) ? "<font color = blue><u>x</u></font>" : "&nbsp;&nbsp;",
               strncmp(x.title, "Re: ", 4) ? "�� " : "",
               void1(nohtml2(x.title)), eff_size(filename));

        //added by hongliang for the url of viewing forum
        printf("<script language=\"javascript\">\n");
        printf("arrayid[%d] = \"board=%s&file=%s&num=%d&start=%d\";\n", i, board, x.filename, start + i, start);
        printf("</script>\n");

        printf("<td><font color=%s>%d</font>\n",
               *(int *) (x.title + 73) > 99 ? "red" : "black",
               *(int *) (x.title + 73));
    }
    if (top_fp != 0)
    {
        fseek(top_fp, 0, SEEK_SET);
        for (i = 0; i < top_total; i++)
        {
            char filename[80];
            char *ptr, *font1 = "", *font2 = "";

            if (fread(&x, sizeof(x), 1, top_fp) <= 0)
                break;
            ptr = flag_str2(x.accessed[0], brc_has_read(x.filename));
            if (ptr[0] == 'N')
            {
                font1 = "<font color=#909090>";
                font2 = "</font>";
            }
            sprintf(filename, "boards/%s/%s", board, x.filename);
            switch (ptr[0])
            {
            case 'm':
            case 'M':
                printf("<tr><td><font color=red>%s</font><td><td>%s","[��ʾ]", userid_str(x.owner));
                break;
            case 'g':
            case 'G':
                printf("<tr><td><font color=red>%s</font><td><td>%s","[֪ͨ]", userid_str(x.owner));
                break;
            case 'b':
            case 'B':
                printf("<tr><td><font color=red>%s</font><td><td>%s","[���]", userid_str(x.owner));
                break;
            default:
                printf("<tr><td><font color=red>%s</font><td><td>%s","[�õ�]", userid_str(x.owner));
            }
            printf("<td>%12.12s", Ctime(atoi(x.filename + 2)) + 4);

            //added by hongliang
            printf("<td><span id=\"arrow%d\">", i + my_t_lines);
            if ( (i+my_t_lines) == currentpoint )
            {
                printf("&gt;");	//Display the '>'
            }
            printf("</span>");


            printf("<td><a href=bbstopcon?board=%s&file=%s&num=%d&start=%d&lines=%d>&nbsp;&nbsp;%s%s </a>%s",
                   board, x.filename, i, start, my_t_lines,
                   strncmp(x.title, "Re: ", 4) ? "�� " : "",
                   void1(nohtml2(x.title)), eff_size(filename));

            //added by hongliang for the url of viewing forum
            printf("<script language=\"javascript\">\n");
            printf("arrayid[%d] = \"board=%s&file=%s&num=%d&start=%d&lines=%d\";\n",
                   i + my_t_lines, board, x.filename, i, start, my_t_lines);
            printf("</script>\n");

            printf("<td><font color=%s>%d</font>\n",
                   *(int *) (x.title + 73) > 99 ? "red" : "black",
                   *(int *) (x.title + 73));
        }
        fclose(top_fp);
    }
    printf("</table><hr>\n");
    /*
       printf("<script>\n");
       printf("if(top==self) document.writeln('<a href=url?url=bbsdoc?board=%s@start=%d>��ӱ߿�</a>');", board, start);
       printf("</script>\n");
     */
    printf("<a href=bbsfdoc?board=%s>��������</a> ", board);
    printf("<a href=bbspst?board=%s>��������</a> \n", board);
    printf("<a href='javascript:location=location'>ˢ��</a> ");
    if (start > 0)
    {
        printf("<a href=bbsdoc?board=%s&start=0>��ҳ</a> ", board);
        printf("<a href=bbsdoc?board=%s&start=%d>��һҳ</a> ", board,
               start - my_t_lines );
    }
    if (start < total - my_t_lines)
    {
        printf("<a href=bbsdoc?board=%s&start=%d>��һҳ</a> ", board,
               start + my_t_lines );
        printf("<a href=bbsdoc?board=%s>ĩҳ</a> ", board);
    }
    if (loginok)
        printf("<a href=bbsclear?board=%s&start=%d>���δ��</a>\n", board,start);
    printf("<a href=bbstdoc?board=%s>����ģʽ</a> ", board);
    printf("<a href=bbsbfind?board=%s>���ڲ�ѯ</a><br> \n", board);
	printf("<a href=bbsfdoc2?board=%s>��������</a> ", board);
    printf("<a href=\"javascript: showhelp();\">���̰���[H]</a> ");
    printf("<a href=bbsnot?board=%s>���廭��</a> ", board);
    printf("<a href=bbsgdoc?board=%s>��ժ��</a> ", board);
    printf("<a href=bbs0an?path=%s>������</a> ", anno_path_of(board));
    //      printf("<a href=bbsadl2?board=%s>���ؾ�����</a> ", board);
    //      printf("<a href=/an/%s.tar.gz>���ؾ�����</a> ", board);

    if (has_BM_perm(&currentuser, board))
        printf("<a href=bbsmdoc?board=%s>����ģʽ</a> ", board);
    //      printf("<a href=bbsafind?board=%s>��������ѯ </a>\n", board);

    printf("<a href=http://bbs.sjtu.edu.cn:8080/%s.tar.gz>���ؾ�����</a> ", board);
    fclose(fp);

    printf("</form>\n");
    printf("</body>\n");
    http_quit();
}


int b_usage(char *board, int stay)
{
    char buf[256];

    sprintf(buf, "%s USE %-20.20s Stay: %5ld (%s)\n", Ctime(time(0)) + 4,
            board, stay, currentuser.userid);
    f_append("use_board", buf);
    sprintf(buf, "%24.24s %s %s W_Enter\n", Ctime(time(0)),
            currentuser.userid, board);
    if (has_BM_perm(&currentuser, board))
        f_append("bm2.log", buf);
}

int show_bm_words(char *board, int width)
{
    FILE *fp;
    char buf[256], buf2[256] = "";

    sprintf(buf, "vote/%s/note2", board);
    fp = fopen(buf, "r");
    if (!fp)
        return;
    fgets(buf2, 80, fp);
    buf2[80] = 0;
    fclose(fp);
    if (strlen(buf2) == 0)
        return;
    if (loginok && has_BM_perm(&currentuser, board))
    {
        printf
        ("<table width=%d><tr style='height:14px'><td><b><a href=bbsmnote3?board=%s>��������</a></b>: <font color='#308080'>%s</font></table>",
         width, board, nohtml(buf2));
    }
    else
    {
        printf
        ("<table width=%d><tr style='height:14px'><td><b>��������</b>: <font color='#308080'>%s</font></table>",
         width, nohtml(buf2));
    }
}

int bbsdocdel_main()
{
    FILE	*fp;
    char	board[80], dir[80];
    struct shortfile *x1;
    struct fileheader x;
    int	i, start, total, width = 720;
    int	currentpoint;

    init_all();
    if (my_font_mode == 0)
        width = 670;
    strsncpy(board, getparm("board"), 32);

    //added by hongliang
    currentpoint	= atoi(getparm("currentpoint"));
    if ( currentpoint < 0 )
    {
        currentpoint = 0;
    }

    x1 = getbcache(board);
    if (x1 == 0)
        http_fatal("�����������");
    strcpy(board, x1->filename);
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    //u_info->board=getbnum(board);

    printf("<body>");
    printf
    ("<form name=form1 action=bbsdocdel?board=%s method=post onsubmit='this.start.value-=1'>\n",
     board);
    printf("<center>\n");
    printf("<table><tr><td>");
    printf
    ("<td><font style=\"font-size:18px;\"><b>%s(%s)</b></font></table>\n",
     x1->filename, x1->title + 11);
    sprintf(dir, "boards/%s/.RETURNDIR", board);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_fatal("�����������Ŀ¼");
    total = file_size(dir) / sizeof(struct fileheader);


    start = atoi(getparm("start"));
    if (my_t_lines < 10 || my_t_lines > 40)
        my_t_lines = 20;

    //added by hongliang
    if ( currentpoint < 0)
    {
        currentpoint	= my_t_lines + start;
    }
    if ( currentpoint >= my_t_lines )
    {
        currentpoint	= my_t_lines - 1;
    }

    if (strlen(getparm("start")) == 0 || start > total - my_t_lines)
        start = total - my_t_lines;
    if (start < 0)
        start = 0;
    if (strlen(getparm("start")) == 0)
        b_usage(board, 33);
    brc_init(currentuser.userid, board);
    printf("<nobr>\n");
    //      printf("<table width=%d><tr style='height:14px'><td align=left> ����: %s, ��������: %d��.",
    //              width, userid_str(x1->BM), count_on_board(board));

    //added by hongliang
    //write the global variables of javascript
    printf("<script language=\"javascript\" src=\"/key_hl_delforumlist.js\"></script>\n");
    printf("<script language=\"javascript\">\n<!--\n");
    printf("var currentid = %d;\n", currentpoint);
    printf("var totalid = %d;\n", my_t_lines - 1);
    printf("var toptotal = 0;\n");
    printf("var arrayid = new Array(totalid);\n");
    printf("var board = \"%s\";\n", board);
    printf("var start = %d;\n", start);
    printf("var total = %d;\n", total);
    printf("if ( total < totalid ) \n\ttotalid = total - 1;\n");
    printf("\n-->\n</script>\n");

    printf
    ("<table width=%d><tr style='height:14px'><td align=left width=50%> ����: %s",
     width, userid_str(x1->BM));
    printf("<td align=right width=25%>");
    if (start > 0)
        printf("<a href=bbsdocdel?board=%s&start=%d>��</a>,", board,
               start - my_t_lines);
    if (start < total - my_t_lines)
        printf("<a href=bbsdocdel?board=%s&start=%d>��</a>,", board,
               start + my_t_lines);

    printf("<td align=right width=25%>");
    printf
    ("<input type=submit value=��ת��> �� <input type=text name=start size=4> ƪ");
    printf("</table>\n");
    show_bm_words(board, width);
    printf("<hr>\n");
    if (total <= 0)
        http_fatal("������������վĿǰû������");
    printf("<table width=%d>\n", width);
    //printf("<tr bgcolor='e8e8e8'><td>���<td>״̬<td>����<td width=72>����<td>����<td>����\n");

    //add a column to display '>' by hongliang on Nov. 20th, 2003 6:50
    printf
    ("<tr bgcolor='e8e8e8'><td>���<td>״̬<td>����<td width=100>����<td width=12>&nbsp;<td>����<td>����\n");
    fseek(fp, start * sizeof(struct fileheader), SEEK_SET);
    for (i = 0; i < my_t_lines; i++)
    {
        char filename[80];
        char *ptr, *font1 = "", *font2 = "";

        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        ptr = flag_str2(x.accessed[0], brc_has_read(x.filename));
        if (ptr[0] == 'N')
        {
            font1 = "<font color=#909090>";
            font2 = "</font>";
        }
        sprintf(filename, "boards/%s/%s", board, x.filename);
        printf("<tr><td>%d<td>%s%s%s<td>%s",
               start + i + 1, font1, ptr, font2, userid_str(x.owner));
        printf("<td>%12.12s", Ctime(atoi(x.filename + 2)) + 4);

        //added by hongliang
        printf("<td><span id=\"arrow%d\">", i);
        if ( i == currentpoint )
        {
            printf("&gt;");	//Display the '>'
        }
        printf("</span>");

        //modified by acdsee,add x flag
        printf("<td><a href=bbscondel?board=%s&file=%s&num=%d&start=%d>%s%s%s </a>%s",
               board, x.filename, start + i, start,
               (x.accessed[0]&FILE_NOREPLY) ? "<font color = red><u>x</u></font>" : "",
               strncmp(x.title, "Re: ", 4) ? "�� " : "",
               void1(nohtml2(x.title)), eff_size(filename));

        //added by hongliang for the url of viewing forum
        printf("<script language=\"javascript\">\n");
        printf("arrayid[%d] = \"board=%s&file=%s&num=%d&start=%d\";\n", i, board, x.filename, start + i, start);
        printf("</script>\n");

        printf("<td><font color=%s>%d</font>\n",
               *(int *) (x.title + 73) > 99 ? "red" : "black",
               *(int *) (x.title + 73));
    }

    printf("</table><hr>\n");

    printf("<a href='bbsmdoc?board=%s'>���ع���ģʽ</a> ", board);
    printf("<a href='javascript:location=location'>ˢ��</a> ");
    if (start > 0)
    {
        printf("<a href=bbsdocdel?board=%s&start=0>��ҳ[Home]</a> ", board);
        printf("<a href=bbsdocdel?board=%s&start=%d>��һҳ[PageUp]</a> ", board,
               start - my_t_lines );
    }
    if (start < total - my_t_lines)
    {
        printf("<a href=bbsdocdel?board=%s&start=%d>��һҳ[PageDown]</a> ", board,
               start + my_t_lines );
        printf("<a href=bbsdocdel?board=%s>ĩҳ[End]</a> ", board);
    }
    fclose(fp);

    //added by hongliang for displaying the keyboard operation help
    printf("<br>\n");
    printf("���̲���: [K] ����/�Ϸ�ҳ &nbsp; [J] ����/�·�ҳ &nbsp; ");
    printf("[��]/[Enter] ������ &nbsp; ");
    printf("[PageUp] �Ϸ�ҳ &nbsp; [PageDown] �·�ҳ ");
    printf("[O]�ָ����� &nbsp; ");

    printf("</form>\n");
    printf("</body>\n");
    http_quit();
}


//this function is for un-re
int bbsdocnore_main()
{
    FILE *fp;
    char board[80], dir[80], file[80];
    struct fileheader x, *p;
    int num, total;
    int start;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(file, getparm("file"), 32);
    start	= atoi(getparm("start"));
    num	= atoi(getparm("num"));

    p	= get_file_ent(board, file);
    if (p == 0)
        http_fatal("����Ĳ���");

    printf("<body>");
    printf("<center>\n");
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    if (strcmp(p->owner, currentuser.userid) && !has_BM_perm(&currentuser, board))
        http_fatal("����Ȩ���ô����²���re");

    strcpy(board, getbcache(board)->filename);

    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_fatal("����Ĳ���1");
    if (strstr(file, "..") || strstr(file, "/"))
        http_fatal("����Ĳ���2");
    sprintf(dir, "boards/%s/.DIR", board);
    total = file_size(dir) / sizeof(x);
    if (total <= 0)
        http_fatal("�������������ڻ���Ϊ��");

    fp = fopen(dir, "r+");
    if (fp == 0)
        http_fatal("dir error2");
    if (num >= 0 && num < total)
    {
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fread(&x, sizeof(x), 1, fp);
        //Set it to no-re or re
        if ( x.accessed[0] & FILE_NOREPLY )
        {
            x.accessed[0] &= ~FILE_NOREPLY;
        }
        else
        {
            x.accessed[0] |= FILE_NOREPLY;
        }
        fseek(fp, sizeof(x) * num, SEEK_SET);
        fwrite(&x, sizeof(x), 1, fp);
    }
    fclose(fp);

    printf("���óɹ�.<br><a href=\"bbsdoc?board=%s&start=%d&currentpoint=%d\">���ر�������</a>",
           board, start, num-start);

    http_quit();
}
