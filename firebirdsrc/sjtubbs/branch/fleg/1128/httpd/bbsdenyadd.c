#include "www.h"
static struct deny
{
    char id[80];
    char exp[80];
    //added by hongliang on Nov. 23th, 2003 for whether auto undeny
    char autoundeny[10];
    char free_time[30];
    //      int free_time;
}
denyuser[256];
static int denynum = 0;

static int loaddenyuser(char *board)
{
    FILE *fp;
    char path[80], buf[256];

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "r");
    if (fp == 0)
        return;
    while (denynum < 100)
    {
        if (fgets(buf, 80, fp) == 0)
            break;
        sscanf(buf, "%s %s %s %s", denyuser[denynum].id,
               denyuser[denynum].exp, denyuser[denynum].autoundeny, denyuser[denynum].free_time);
        denynum++;
    }
    fclose(fp);
}

static int savedenyuser(char *board)
{
    FILE *fp;
    int i;
    char path[80], *exp;

    sprintf(path, "boards/%s/deny_users", board);
    fp = fopen(path, "w");
    if (fp == 0)
        return;
    for (i = 0; i < denynum; i++)
    {
        int m;

        exp = denyuser[i].exp;
        if (denyuser[i].id[0] == 0)
            continue;
        for (m = 0; exp[m]; m++)
        {
            if (exp[m] <= 32 && exp[m] > 0)
                exp[m] = '.';
        }
        fprintf(fp, "%-12s %-40s %s %16.16s���\n", denyuser[i].id,
                denyuser[i].exp, denyuser[i].autoundeny, denyuser[i].free_time);
    }
    fclose(fp);
}

int bbsdenyadd_main()
{
    int i;
    char exp[80], board[80], *userid;
    int dt;
    //added by hongliang on Nov. 23th, 2003 for whether auto undeny
    int fAutoUndeny;


    init_all();
    if (!loginok)
        http_fatal("����δ��¼, ���ȵ�¼");
    strsncpy(board, getparm("board"), 30);
    strsncpy(exp, getparm("exp"), 30);
    dt = atoi(getparm("dt"));

    //added by hongliang
    fAutoUndeny = atoi(getparm("autoundeny"));

    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    if (!has_BM_perm(&currentuser, board))
        http_fatal("����Ȩ���б�����");
    loaddenyuser(board);
    userid = getparm("userid");
    if (userid[0] == 0)
        return show_form3(board);
    if (getuser(userid) == 0)
        http_fatal("�����ʹ�����ʺ�");
    strcpy(userid, getuser(userid)->userid);
    if (dt < 1 || dt > 30)//modified form 99 to 30,by acdsee
        http_fatal("�����뱻������(1-30)");
    if (exp[0] == 0)
        http_fatal("���������ԭ��");
    for (i = 0; i < denynum; i++)
        if (!strcasecmp(denyuser[i].id, userid))
            http_fatal("���û��Ѿ�����");
    if (denynum > 40)
        http_fatal("̫���˱�����");
    strsncpy(denyuser[denynum].id, userid, 13);
    strsncpy(denyuser[denynum].exp, exp, 30);
    //added by hongliang
    strsncpy(denyuser[denynum].autoundeny, fAutoUndeny ? "(a)" : "(n)", 4);
    getdatestring(time(0) + dt * 86400, NA);
    strsncpy(denyuser[denynum].free_time, datestring, 17);
    //denyuser[denynum].free_time=time(0)+dt*86400;
    denynum++;
    savedenyuser(board);
    printf("��� %s �ɹ�<br>\n", userid);
    inform2(board, userid, exp, dt);
    printf("[<a href=bbsdenyall?board=%s>���ر����ʺ�����</a>]", board);
    http_quit();
}

int show_form3(char *board)
{
    printf("<center><font class=\"title\">%s - ���������</font>[������: %s]<hr>\n",
           BBSNAME, board);
    printf
    ("<form action=bbsdenyadd><input type=hidden name=board value='%s'>",
     board);
    printf
    ("���ʹ����<input name=userid size=12> ����POSTȨ <input name=dt size=2> ��, ԭ��<input name=exp size=20>\n", board);
    //added by hongliang on Nov. 23th, 2003 for auto undeny
    printf("<input type=\"checkbox\" name=\"autoundeny\" value=\"1\" checked>�Զ���� \n");
    printf("<input type=submit value=ȷ��></form>");
}

int inform2(char *board, char *user, char *exp, int dt)
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

    //BUG
    //Bug-description:	web can not auto post a article to DENY_POST_BOARDNAME('penalty' in SJTUBBS)
    //Bug-solution:		post it :)
    //Bug-fixer:		hongliang
    //Date:			Nov. 21th, 2003 14:14
    //start-hongliang
#ifdef DENY_POST_BOARDNAME

	if(strncmp(board, "BMTraining", 10) == 0)
	{
		post_article(TRAIN_ANNOUNCE_BOARDNAME, title, path, "SJTUBBS", "�Զ�����ϵͳ",
			"�Զ�����ϵͳ", -1, 0, now, now);
	}
	else
	{
		post_article(DENY_POST_BOARDNAME, title, path, "SJTUBBS", "�Զ�����ϵͳ",
                 "�Զ�����ϵͳ", -1, 0, now, now);
	}
#endif
    //end-hongliang

    post_mail(user, title, path, currentuser.userid, currentuser.username,
              fromhost, -1);
    unlink(path);
    printf("ϵͳ�Ѿ�����֪ͨ��%s.<br>\n", user);
}
