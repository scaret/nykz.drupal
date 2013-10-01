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

    char board[200];

    bzero(mybrd, sizeof(mybrd));
    mybrdnum = 0;
    init_all();
    strsncpy(board, getparm("board"), 32);
    if (!loginok)
        http_quit_print("��ʱ��δ��¼��������login");
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    if (mybrdnum > 20)
        http_quit_print("��Ԥ����������Ŀ�Ѵ����ޣ���������Ԥ��");
    if (ismybrd(board) >= 0)
        http_quit_print("���Ѿ�Ԥ�������������");
    if (!has_read_perm(&currentuser, board))
        http_quit_print("��������������");
    strcpy(mybrd[mybrdnum], board);
    SaveMyBrdsToGoodBrd(currentuser.userid, mybrd, mybrdnum + 1);
    printf("<script>top.f2.location='bbsleft'</script>\n");
    printf
        ("Ԥ���������ɹ�<br><a href='javascript:history.go(-1)'>���ٷ���</a>");
    printf("\n</html>");
    return 0;
}
