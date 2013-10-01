#include "www.h"
#define TOPIC_MAX 2000
int bbstfind0_main()
{
    char board[80], reid_str[16];
    struct fileheader *fh, *fhs[TOPIC_MAX];
    int num = 0, offset, no_re = 0;
    unsigned int reid = 0;

    silence = 1;
    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(reid_str, getparm("reid"), 16);
    if (!has_read_perm(&currentuser, board))
    {
        cgi_head();
        html_head();
        http_quit_print("�����������");
    }
    else
    {
        if (is_normal_board)
            cache_age = 3 * 60;
        cgi_head();
        html_head();
        print_style();
    }
    if (strlen(reid_str) != 0)
        reid = atoi(reid_str);
    else
        http_quit_print("�������reid");
    if (getbcache(board)->flag & NOREPLY_FLAG)
        no_re = 1;
    fh = (struct fileheader *) map_board_dir(board, ".DIR");
    if (fh == NULL)
        http_quit_print("Ŀ¼����");
    offset = get_size_of_mapdir() - 1;
    for (; offset >= 0; --offset)
        if (fh[offset].reid == reid)
        {
            fhs[num++] = &fh[offset];
            if (fh[offset].id == reid || num == TOPIC_MAX)
                break;
        }
    if (num == 0)
    {
        unmap_board_dir();
        http_quit_print("�����ⲻ����");
    }

    printf
        ("<center><font class=\"title\">%s - ͬ������ҡ���</font>[������: %s] [���� '%s']<hr>\n",
         BBSNAME, board, void1(nohtml2(fhs[num - 1]->title)));
    printf
        ("<table width=610><tr bgcolor=e8e8e8><td>���<td>����<td>����<td>����");
    for (offset = num - 1; offset >= 0; --offset)
    {
        struct fileheader *p;

        p = fhs[offset];
        printf("<tr><td>%d", num - offset);
        printf("<td>%s", userid_str(p->owner));
        printf("<td>%6.6s", Ctime(atoi(p->filename + 2)) + 4);
        printf("<td><a href=bbscon?board=%s&file=%s>%s</a>",
               board, p->filename, void1(nohtml2(p->title)));
    }
    printf("</table><hr>\n���ҵ� %d ƪ\n", num);
    printf("<a href=bbsdoc?board=%s>��������</a> ", board);
    printf("<a href=bbstcon?board=%s&reid=%d>������ȫ��չ��</a> ",
           board, fhs[0]->reid);
    unmap_board_dir();
    printf("</html>");
    return 0;
}
