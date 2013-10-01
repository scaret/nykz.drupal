#include "www.h"

static void inform2(char *board, char *user, char *exp, int dt);
static void show_form3(char *board);
int bbsdenyadd_main()
{
    int i;
    char exp[80], board[80], *userid;
    int dt;

    int fAutoUndeny;

    bzero(denyuser, sizeof(denyuser));
    denynum = 0;
    init_all();
    if (!loginok)
        http_quit_print("����δ��¼, ���ȵ�¼");
    strsncpy(board, getparm("board"), 30);
    strsncpy(exp, getparm("exp"), 30);
    dt = atoi(getparm("dt"));

    fAutoUndeny = atoi(getparm("autoundeny"));

    if (!has_read_perm(&currentuser, board))
        http_quit_print("�����������");
    if (!has_BM_perm(&currentuser, board))
        http_quit_print("����Ȩ���б�����");
    loaddenyuser(board);
    userid = getparm("userid");
    if (userid[0] == 0)
    {
        show_form3(board);
        return 0;
    }
    if (getuser(userid) == 0)
        http_quit_print("�����ʹ�����ʺ�");
    strcpy(userid, getuser(userid)->userid);
    if (dt < 1 || dt > 30)
        http_quit_print("�����뱻������(1-30)");
    if (exp[0] == 0)
        http_quit_print("���������ԭ��");
    for (i = 0; i < denynum; i++)
        if (!strcasecmp(denyuser[i].id, userid))
            http_quit_print("���û��Ѿ�����");
    if (denynum > 40)
        http_quit_print("̫���˱�����");
    strsncpy(denyuser[denynum].id, userid, 13);
    strsncpy(denyuser[denynum].exp, exp, 30);

    strsncpy(denyuser[denynum].autoundeny, fAutoUndeny ? "(a)" : "(n)", 4);
    getdatestring(time(0) + dt * 86400, NA);
    strsncpy(denyuser[denynum].free_time, datestring, 17);

    denynum++;
    savedenyuser(board);
    printf("��� %s �ɹ�<br>\n", userid);
    inform2(board, userid, exp, dt);
    printf("[<a href=bbsdenyall?board=%s>���ر����ʺ�����</a>]", board);
    printf("\n</html>");
    return 0;
}

static void show_form3(char *board)
{
    printf
        ("<center><font class=\"title\">%s - ���������</font>[������: %s]<hr>\n",
         BBSNAME, board);
    printf
        ("<form action=bbsdenyadd><input type=hidden name=board value='%s'>",
         board);
    printf
        ("���ʹ����<input name=userid size=12> ����POSTȨ <input name=dt size=2> ��, ԭ��<input name=exp size=20>\n");

    printf
        ("<input type=\"checkbox\" name=\"autoundeny\" value=\"1\" checked>�Զ���� \n");
    printf("<input type=submit value=ȷ��></form>");
}

static void inform2(char *board, char *user, char *exp, int dt)
{
    FILE *fp;
    char path[80], title[80];
    time_t now = time(0);

    sprintf(title, "[����] %s��ȡ����%s��ķ���Ȩ��POSTȨ", user, board);
    sprintf(path, "tmp/%d.tmp", getpid());
    fp = fopen(path, "w");
    fprintf(fp, "�����������Զ�����ϵͳ�Զ�������\n\n");
    fprintf(fp, "%s��������Ա[%s]�����%s��POSTȨ[%d]��.\n", user,
            currentuser.userid, board, dt);
    fprintf(fp, "ԭ����: %s\n", exp);
    fclose(fp);
    post_article(board, title, path, "SJTUBBS", "�Զ�����ϵͳ",
                 "�Զ�����ϵͳ", -1, 0, now, now);

#ifdef DENY_POST_BOARDNAME

    if (strncmp(board, "BMTraining", 10) == 0)
    {
        post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS",
                     "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1, 0, now, now);
    }
    else
    {
        post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS",
                     "�Զ�����ϵͳ", "�Զ�����ϵͳ", -1, 0, now, now);
    }
#endif

    post_mail(user, title, path, currentuser.userid, currentuser.username,
              fromhost, -1);
    unlink(path);
    printf("ϵͳ�Ѿ�����֪ͨ��%s.<br>\n", user);
}
