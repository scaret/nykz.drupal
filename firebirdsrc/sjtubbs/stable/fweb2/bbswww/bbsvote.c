#include "www.h"

extern int page, range;
extern struct shortfile *bcache;
extern struct BCACHE *brdshm;
static char *vote_type[] = { "�Ƿ�", "��ѡ", "��ѡ", "����", "�ʴ�" };
struct votebal currvote;
char controlfile[STRLEN], descfile[STRLEN], flagfile[STRLEN];
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

void printitems(struct votebal *abuf, struct ballot *uservote)
{
    int i;

    for (i = 0; i <= abuf->totalitems - 1; i++)
    {
        if (abuf->type == 1)
        {
            if (voted_flag == YEA)
                printf
                    ("<tr><td><INPUT TYPE=\"RADIO\" NAME=\"SF\" VALUE=\"%d\" %s>%c.%s",
                     i, ((uservote->voted >> i) & 1 ? "CHECKED" : " "),
                     'A' + i, abuf->items[i]);
            else
                printf
                    ("<tr><td><INPUT TYPE=\"RADIO\" NAME=\"SF\" VALUE=\"%d\">%c.%s",
                     i, 'A' + i, abuf->items[i]);

        }
        else if (abuf->type == 2)
        {
            if (voted_flag == YEA)
                printf
                    ("<tr><td><INPUT TYPE=\"RADIO\" NAME=\"DX\" VALUE=\"%d\" %s>%c.%s",
                     i, ((uservote->voted >> i) & 1 ? "CHECKED" : " "),
                     'A' + i, abuf->items[i]);
            else
                printf
                    ("<tr><td><INPUT TYPE=\"RADIO\" NAME=\"DX\" VALUE=\"%d\">%c.%s",
                     i, 'A' + i, abuf->items[i]);
        }
        else if (abuf->type == 3)
        {
            if (voted_flag == YEA)
                printf
                    ("<tr><td><INPUT TYPE=\"CHECKBOX\" NAME=\"FX%d\" VALUE=\"%d\" %s>%c.%s",
                     i, i, ((uservote->voted >> i) & 1 ? "CHECKED" : " "),
                     'A' + i, abuf->items[i]);
            else
                printf
                    ("<tr><td><INPUT TYPE=\"CHECKBOX\" NAME=\"FX%d\" VALUE=\"%d\">%c.%s",
                     i, i, 'A' + i, abuf->items[i]);
        }
        else if (abuf->type == 4)
        {
            if (voted_flag == YEA)
                printf
                    ("<tr>�˴������ֵ���ܳ���%d<tr>������һ��ֵ:<INPUT TYPE=\"TEXT\" NAME=\"SZ\" SIZE=\"10\" VALUE=\"%d\">",
                     abuf->maxtkt, uservote->voted);
            else
                printf
                    ("<tr>�˴������ֵ���ܳ���%d<tr>������һ��ֵ:<INPUT TYPE=\"TEXT\" NAME=\"SZ\" SIZE=\"10\">",
                     abuf->maxtkt);
            break;
        }

    }
    if (abuf->type != 5)
    {
        printf("<p><tr><td>���������ı������(����):");
        if (voted_flag == YEA)
        {
            printf
                ("<tr><td><TEXTAREA NAME=\"YJ\" WARP=\"PHYSICAL\" COLS=\"80\" ROWS=\"4\">");
            if (strcmp(uservote->msg[0], ""))
                printf("%s\n", uservote->msg[0]);
            if (strcmp(uservote->msg[1], ""))
                printf("%s\n", uservote->msg[1]);
            if (strcmp(uservote->msg[2], ""))
                printf("%s\n", uservote->msg[2]);
            printf("</TEXTAREA>");
        }
        else
        {
            printf
                ("<tr><td><TEXTAREA NAME=\"YJ\" WARP=\"PHYSICAL\" COLS=\"80\" ROWS=\"4\">");
            printf("</TEXTAREA>");
        }
    }
    else
    {
        if (voted_flag == YEA)
        {
            printf("<tr>��������������(ע��:�������еĲ��ֶ��ᱻ����):");
            printf
                ("<tr><TEXTAREA NAME=\"WD\" WARP=\"PHYSICAL\" COLS=\"80\" ROWS=\"4\">");
            if (strcmp(uservote->msg[0], ""))
                printf("%s\n", uservote->msg[0]);
            if (strcmp(uservote->msg[1], ""))
                printf("%s\n", uservote->msg[1]);
            if (strcmp(uservote->msg[2], ""))
                printf("%s\n", uservote->msg[2]);
            printf("</TEXTAREA>");
        }
        else
        {
            printf("<tr>��������������(ע��:�������еĲ��ֶ��ᱻ����):");
            printf
                ("<tr><TEXTAREA NAME=\"WD\" WARP=\"PHYSICAL\" COLS=\"80\" ROWS=\"4\"></TEXTAREA>");
        }

    }

}

int has_special_perm(abuf)
struct votebal abuf;
{
    char board[80];

    strsncpy(board, getparm("board"), 32);
    if (currentuser.firstlogin > abuf.opendate)
    {
        http_quit_print
            ("�Բ���, ��ͶƱ�����ʺ�����֮ǰ������������ͶƱ\n");
    }
    else if (!HAS_PERM(abuf.level & ~(LISTMASK | VOTEMASK)))
    {
        http_quit_print("�Բ�����Ŀǰ����Ȩ�ڱ�Ʊ��ͶƱ\n");
    }
    else if (abuf.level & LISTMASK)
    {
        char listfilename[STRLEN];

        sprintf(listfilename, "vote/%s/%s", board, "vote.list");
        if (access(listfilename, 0) != 0)
            http_quit_print
                ("�Բ��𣬱�Ʊ����Ҫ�趨��ͶƱ���᷽�ɽ���ͶƱ\n");
        else if (!file_has_word(listfilename, currentuser.userid))
            http_quit_print("�Բ���, ͶƱ�������Ҳ������Ĵ���\n");
    }
    else if (abuf.level & VOTEMASK)
    {
        if (currentuser.numlogins < abuf.x_logins
            || currentuser.numposts < abuf.x_posts
            || (unsigned int) currentuser.stay < (abuf.x_stay * 3600)
            || (unsigned int) currentuser.firstlogin >
            (abuf.opendate - abuf.x_live * 86400))
            http_quit_print("�Բ�����Ŀǰ�в����ʸ��ڱ�Ʊ��ͶƱ\n");
    }
    return YEA;
}

int bbsvote_main()
{
    char board[80], buf[512];
    struct votebal abuf;
    struct shortfile *x1;
    struct ballot uservote;
    int width = 720, fd;
    time_t opendate;
    FILE *fp;

    init_all();
    strsncpy(board, getparm("board"), 32);
    opendate = atoi(getparm("opendate"));
    if (!has_read_perm(&currentuser, board))
    {
        http_quit_print("�����������");
    }

    x1 = getbcache(board);
    if (my_font_mode == 0)
        width = 670;

    printf("<body>");
    printf
        ("<form name=form1 action='bbsdovote' method=post onSubmit=\"return verify(this);\" enctype='multipart/form-data'>\n<input type=hidden name=board value='%s'>",
         board);
    printf("<input type=hidden name=opendate value='%lu'>", opendate);
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
    sprintf(controlfile, "vote/%s/control", board);
    if ((fd = open(controlfile, O_RDONLY, 0)) == -1)
    {
        printf("<tr><td>�򿪼�¼�ļ�����������</td>");
        goto out;
    }
    while (read(fd, &abuf, sizeof(struct votebal)) ==
           sizeof(struct votebal))
    {
        if (opendate == abuf.opendate)
            break;
    }
    if (opendate != abuf.opendate)
    {
        printf("<tr><td>Ѱ���������</td>");
        close(fd);
        goto out;
    }
    close(fd);
    has_special_perm(abuf);
    printf("<input type=hidden name=type value=\"%d\">", abuf.type);
    printf("<input type=hidden name=totalitems value='%d'>",
           abuf.totalitems);
    printf
        ("<table width=670><tr style='height:14px'><td><b>ͶƱ����:%s</b></td><tr><td>ͶƱ����:%s</td></tr>%s</table><hr>",
         abuf.title, vote_type[abuf.type - 1],
         abuf.
         showip ? "<tr><td>����ͶƱ����ʾͶƱ��ip��Ϣ</td></tr>" : "");
    sprintf(descfile, "vote/%s/desc.%ld", board, opendate);
    fp = fopen(descfile, "r");
    if (fp == 0)
    {
        printf("<tr><td>�������ļ�����������</td>");
        goto out;
    }
    printf("<table width=670><tr style='height:14px'><pre>");
    while (1)
    {
        if (fgets(buf, 512, fp) == 0)
            break;
        hhprintf("%s", void1(buf));
    }
    fclose(fp);
    printf("</pre>");
    sprintf(flagfile, "vote/%s/flag.%ld", board, opendate);
    if (search_record
        (flagfile, (char *) &uservote, sizeof(uservote), cmpvuid,
         currentuser.userid) <= 0)
    {
        voted_flag = NA;
    }
    else
        voted_flag = YEA;
    printitems(&abuf, &uservote);

  out:
    printf("</table><hr>\n");
    printf("<center><input type=submit value='ͶƱ'></center> ");
    printf("</form>\n");
    printf("</body>\n");
    if (abuf.type == 1 || abuf.type == 2)
    {

        printf("<script language=\"javascript\">\n");
        printf("<!--\n");
        printf("function verify(f)\n");
        printf("{var v=0;");
        printf("for(var i=0;i<=%d;i++){\n", abuf.totalitems - 1);
        printf("var e=f.elements[4+i];\n");
        printf("if(e.checked){v++;}\n");
        printf("}if(v==0){alert(\"������ѡ��һ��!\");return false;}\n");
        printf("return true;}\n");
        printf("-->\n");
        printf("</script>\n");

    }
    if (abuf.type == 3)
    {
        printf("<script language=\"javascript\">\n");
        printf("<!--\n");
        printf("function verify(f)\n");
        printf("{var v=0;\n");
        printf("for(var i=0;i<=%d;i++){\n", abuf.totalitems - 1);
        printf("var e=f.elements[4+i];\n");
        printf("if(e.checked){v++;\n}");
        printf
            ("}if(v>%d){alert(\"�������г���%d��ѡ��!\");return false;}\n",
             abuf.maxtkt, abuf.maxtkt);
        printf("if(v==0){alert(\"������ѡ������һ��!\");return false;}\n");
        printf("return true;}\n");
        printf("-->\n");
        printf("</script>\n");
    }
    if (abuf.type == 4)
    {
        printf("<script language=\"javascript\">\n");
        printf("<!--\n");
        printf("function verify(f){\n");
        printf("var msg;\n");
        printf("var e=f.SZ;\n");
        printf("var v=e.value;\n");
        printf("if((v<0)||(v>%d)){\n", abuf.maxtkt);
        printf("alert(\"������������ָ����Χ�ڵ���ֵ\");\n");
        printf("return false;\n");
        printf("}\n");
        printf("return true;\n");
        printf("}\n");
        printf("-->\n");
        printf("</script>\n");

    }
    if (abuf.type == 5)
    {

        printf("<script language=\"javascript\">\n");
        printf("<!--\n");
        printf("function verify(f)\n");
        printf("{var e=f.elements[4];\n");
        printf("if(!e.value){alert(\"��������������\");return false;}\n");
        printf("return true;}\n");
        printf("-->\n");
        printf("</script>\n");

    }
    return 0;
}
