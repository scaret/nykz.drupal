#include "www.h"

extern int page, range;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;
static char *vote_type[] = { "�Ƿ�", "��ѡ", "��ѡ", "����", "�ʴ�" };
struct votebal currvote;
char controlfile[STRLEN];
unsigned int result[33];
int vnum;
int voted_flag;
FILE *sug;

static int cmpvuid(userid, uv)
char *userid;
struct ballot *uv;
{
    return !strcmp(userid, uv->uid);
}

int bbsvotelist_main()
{
    char board[80], page_str[10], buf[STRLEN + 10], datestring[30];
    struct votebal abuf;
    struct shortfile *x1;
    struct ballot uservote;
    struct tm *tm;
    int num_voted, num_of_vote;
    int width = 720, i, fd;

    init_all();
    strsncpy(board, getparm("board"), 32);
    strsncpy(page_str, getparm("page"), 10);
    if (!has_read_perm(&currentuser, board))
    {
        http_quit_print("�����������");
    }
    x1 = getbcache(board);
    strcpy(board, x1->filename);
    if (my_font_mode == 0)
        width = 670;

    printf("<body>");
    printf
        ("<form name=form1 action='bbsvote' method=post onsubmit='this.page.value -= 1;return true'>\n<input type=hidden name=board value='%s'>",
         board);
    printf("<center>\n");
    printf("<table><tr><td>");
    printf("<td><font class=title><b>%s(%s)</b></font></table>\n",
           x1->filename, x1->title + 11);

    printf("<nobr>\n");

    printf
        ("<table width=%d><tr style='height:14px'><td align=left width=40%%> ����: %s",
         width, userid_str(x1->BM));
    printf
        ("<td align=right width=10%%><font style='font-size:12px; color:#f00000'>ͶƱ��</font></td>");
    printf
        ("<table width=670><tr style='height:14px'><td><b><a href=bbsmnote3?board=wine>ͶƱ���б�</a></b></table><hr>");
    sprintf(controlfile, "vote/%s/control", board);
    num_of_vote = get_num_records(controlfile, sizeof(struct votebal));
    if (num_of_vote == 0)
    {
        printf
            ("<td><font style='font-size:12px; color:#f00000'>Ŀǰû���κ�ͶƱ����</font></td>");
        goto out;
    }
    if ((fd = open(controlfile, O_RDONLY, 0)) == -1)
    {
        printf("<tr><td>�򿪼�¼�ļ�����������<td>");
        goto out;
    }
    printf
        ("<table width=670><tr bgcolor='e8e8e8'><td>���<td>������<td width=72>������<td>ͶƱ����<td>���<td>����<td>����<td>����ͶƱ��");
    for (i = 1; i <= num_of_vote; i++)
    {
        read(fd, &abuf, sizeof(struct votebal));
        sprintf(buf, "vote/%s/flag.%ld", board, abuf.opendate);
        if (search_record
            (buf, (char *) &uservote, sizeof(uservote), cmpvuid,
             currentuser.userid) <= 0)
        {
            voted_flag = NA;
        }
        else
            voted_flag = YEA;
        num_voted = get_num_records(buf, sizeof(struct ballot));
        tm = localtime(&abuf.opendate);
        sprintf(datestring, "%02d��%02d��", tm->tm_mon + 1, tm->tm_mday);
        printf
            ("<tr><td>%d<td><a href=bbsqry?userid=%s>%s</a><td>%s<td><a href=bbsvote?board=%s&opendate=%lu>%s</a><td>%s<td>%d<td>%d<td>%s",
             i, abuf.userid, abuf.userid, datestring, board, abuf.opendate,
             abuf.title, vote_type[abuf.type - 1], abuf.maxdays, num_voted,
             voted_flag == YEA ? "��" : "��");

    }
    close(fd);

  out:
    printf("</table><hr>\n");
    printf("<center><a href=bbsdoc?board=%s>����</a></center> ", board);

    printf("</form>\n");
    printf("</body>\n");
    printf("</html>");
    return 0;
}
