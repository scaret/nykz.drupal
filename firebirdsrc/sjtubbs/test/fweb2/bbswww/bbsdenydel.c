#include "www.h"
int bbsdenydel_main()
{
    int i;
    char board[80], *userid;

    bzero(denyuser, sizeof(denyuser));
    denynum = 0;
    init_all();
    if (!loginok)
        http_quit_print("����δ��¼, ���ȵ�¼");
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_quit_print("�����������");
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("����Ȩ���б�����");
    loaddenyuser(board);
    userid = getparm("userid");
    for (i = 0; i < denynum; i++)
    {
        if (!strcasecmp(denyuser[i].id, userid))
        {
            denyuser[i].id[0] = 0;
            savedenyuser(board);
            printf("�Ѿ��� %s ���. <br>\n", userid);
            inform(board, userid);
            printf("[<a href=bbsdenyall?board=%s>���ر�������</a>]",
                   board);
            http_quit();
        }
    }
    http_quit_print("����û����ڱ���������");
    return 0;
}
