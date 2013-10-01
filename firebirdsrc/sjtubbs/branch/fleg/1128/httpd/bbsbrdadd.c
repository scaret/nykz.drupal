#include "www.h"

static char mybrd[GOOD_BRC_NUM][80];
static int mybrdnum = 0;

static int ismybrd(char *board)
{
    int n;

    for (n = 0; n < mybrdnum; n++)
        if (!strcasecmp(mybrd[n], board))
            return n;
    return -1;
}

int bbsbrdadd_main()
{
    /*FILE *fp;*/
    char /*file[200],*/ board[200];

    init_all();
    strsncpy(board, getparm("board"), 32);
    if (!loginok)
        http_fatal("��ʱ��δ��¼��������login");
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    if (mybrdnum > 20)
        http_fatal("��Ԥ����������Ŀ�Ѵ����ޣ���������Ԥ��");
    if (ismybrd(board) >= 0)
        http_fatal("���Ѿ�Ԥ�������������");
    if (!has_read_perm(&currentuser, board))
        http_fatal("��������������");
    strcpy(mybrd[mybrdnum], board);
    SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, mybrdnum + 1);
    printf("<script>top.f2.location='bbsleft'</script>\n");
    printf
    ("Ԥ���������ɹ�<br><a href='javascript:history.go(-1)'>���ٷ���</a>");
    http_quit();
}
