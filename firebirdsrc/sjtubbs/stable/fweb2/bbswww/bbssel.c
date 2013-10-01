#include "www.h"
#define MAXRESULT  50
int bbssel_main()
{
    char *board, buf[80], *board1, *title;
    int i, total = 0;
    int possibleBoards[MAXRESULT];

    cache_age = 60 * 60;
    init_all();
    board = trim(nohtml(getparm("board")));
    if (board[0] == 0)
    {
        printf("<font class=\"title\">%s - ѡ��������</font><hr>\n",
               BBSNAME);
        printf("<form action=bbssel>\n");
        printf("����������: <input type=text name=board>");
        printf(" <input type=submit value=ȷ��>");
        printf("</form>\n");
        printf("</html>");
        return 0;
    }
    else
    {
        for (i = 0; i < MAXBOARD; i++)
        {
            board1 = shm_bcache->bcache[i].filename;
            title = shm_bcache->bcache[i].title;

            if (shm_bcache->bcache[i].flag2[0] & BOARD_DIR_FLAG)
                continue;

            if (!has_read_perm(&currentuser, board1))
                continue;
            if (!strcasecmp(board, board1))
            {
                sprintf(buf, "bbsdoc?board=%s", board1);
                redirect(buf);
                http_quit();
            }
        }
        for (i = 0; i < MAXBOARD; i++)
        {
            board1 = shm_bcache->bcache[i].filename;
            title = shm_bcache->bcache[i].title;

            if (shm_bcache->bcache[i].flag2[0] & BOARD_DIR_FLAG)
                continue;

            if (!has_read_perm(&currentuser, board1))
                continue;
            if (strcasestr(board1, board) || strcasestr(title, board))
            {
                possibleBoards[total++] = i;
                if (total > MAXRESULT - 1)
                    break;
            }
        }
        if (total == 1)
        {
            board1 = shm_bcache->bcache[possibleBoards[0]].filename;
            sprintf(buf, "bbsdoc?board=%s", board1);
            redirect(buf);
            http_quit();
        }
        printf("<font class=\"title\">%s - ѡ��������</font><hr>\n",
               BBSNAME);
        printf("�Ҳ���������%s", board);
        printf("�����к���'%s'����������: <br><br>\n", board);
        printf("<table>");
        for (i = 0; i < total;)
        {
            board1 = shm_bcache->bcache[possibleBoards[i]].filename;
            title = shm_bcache->bcache[possibleBoards[i]].title;
            printf("<tr><td>%d", ++i);
            printf("<td><a href=bbsdoc?board=%s>%s</a><td>%s<br>\n",
                   board1, board1, title + 7);
        }
        printf("</table><br>\n");
        printf("���ҵ�%d������������������.\n", total);
        printf("</html>");
        return 0;
    }
}
