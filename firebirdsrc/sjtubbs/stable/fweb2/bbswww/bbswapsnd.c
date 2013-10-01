#include <glib.h>
#include "www.h"
extern int get_unique_file_name(char *name);
#ifdef LOGHANDLER
int log2db2(char *user_id, char *ip_address, char *filename, char *board_name,
       char *title, char *content, unsigned int reid)
{
    char *query_string, *content_ok, title_ok[256], table_name[16];
    int content_len, title_len, query_len;
    time_t timestamp;
    struct tm *tm;

    timestamp = time(NULL);
    tm = localtime(&timestamp);

    sprintf(table_name, "posts%02d", tm->tm_hour);

    content_len = strlen(content);
    content_len = 65534 > content_len ? content_len : 65534;

    content_ok = mp_alloc((content_len + 1) * 4 + 1024);
    query_string = content_ok + (content_len + 1) * 2;

    title_len = strlen(title);
    title_len =
        (int) sizeof(title_ok) / 2 - 1 >
        title_len ? title_len : (int) sizeof(title_ok) / 2 - 1;

    mysql_escape_string(content_ok, content, content_len);
    mysql_escape_string(title_ok, title, title_len);
    query_len =
        snprintf(query_string, (content_len + 1) * 2 + 1024,
                 LOGHANDLER_SQL
                 "insert into %s (user_id,ip_addr,time,filename,board,title,content,ext,reid) values ('%s','%s',FROM_UNIXTIME(%ld),'%s','%s','%s','%s','www','%d')\n",
                 table_name, user_id, ip_address, timestamp, filename,
                 board_name, title_ok, content_ok, reid);
    return loghander_write(query_string, query_len);
}
#endif
extern int do_uploadfile();
extern int if_exist_title(char *title);
extern int is_hide_board(int level);
extern void write_posts(char *id, char *board, char *title);

int bbswapsnd_main()
{
    char filename[128], dir[80], board[80], title[80], buf[80], *content;
    char reid_str[16];
    int autocr = 0, r, sig, size;
    unsigned int i;
    int anony;
    int noip = 0;

    FILE *fp;
    char keyfile[80];
    char keywords[64][64];
    int ii = 0, len, key = 0;
    char *pos;
    struct shortfile *brd;

    time_t now = 0;
    unsigned int currid = 0;
    unsigned int reid = 0;
    time_t now1 = time(0);
    struct tm *tm;

    tm = localtime(&now1);
    init_all();
    fflush(stdout);
    if (cgiFormSuccess == cgiFormFileSize("up", &size))
    {
        do_uploadfile();
        return 0;
    }
    cgiFormString("board", board, 18);

    cgiFormString("title", title, 50);

    if (title[0] == ' ')
    {
        i = 1;
        while (title[i] == ' ')
            i++;
        if (i == strlen(title))
            sprintf(title, "�ޱ���");
    }

    cgiFormString("reidstr", reid_str, 16);
    if (reid_str[0])
    {
        reid = atoi(reid_str);
        currid = 0;
    }
    else
    {
        now = time(0);
        reid = now;
        currid = now;
    }

    if (!loginok)
        http_quit_print("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");

    if (!has_post_perm(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print("�������������������Ȩ�ڴ���������������");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print
            ("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ��巢������.������������� BBSHelp ���� sysop �巢����ѯ��");
#endif
    if (!HAS_PERM
        (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
         PERM_BOARDS))
    {
        if ((unsigned int) tm->tm_hour >= shm_bcache->all_no_post_begin
            && (unsigned int) tm->tm_hour < shm_bcache->all_no_post_end)
        {
            printf("��վ��ÿ�� %d ʱ�� %d ʱ��ȫվֻ����",
                   shm_bcache->all_no_post_begin,
                   shm_bcache->all_no_post_end);
            http_quit();
        }

    }

    if (!HAS_PERM
        (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
         PERM_BOARDS))
    {

        if ((unsigned int) tm->tm_hour >= shm_bcache->no_post_begin
            && (unsigned int) tm->tm_hour < shm_bcache->no_post_end)
        {
            if (!insjtunetwork(fromhost))
            {

                printf
                    ("��վ��ÿ��%dʱ������%dʱ��ͣУ���û��ķ���Ȩ�ޡ�\n\n          ������������� BBSHelp ���� sysop ��������֪ͨ��˵����",
                     shm_bcache->no_post_begin, shm_bcache->no_post_end);
                http_quit();

            }
        }
    }
    if (strlen(getparm("autocr")) > 0)
        autocr = 1;
    brd = getbcache(board);
    if (brd == 0)
        http_quit_print("���������������");
    strcpy(board, brd->filename);
    for (i = 0; i < strlen(title); i++)
        if (title[i] <= 27 && title[i] >= -1)
            title[i] = ' ';
    sig = atoi(getparm("signature"));
    anony = atoi(getparm("anony"));
    content = getparm("text");

    sprintf(keyfile, "%s", "etc/keywords");
    memset(keywords, 0, sizeof(keywords));
    fp = fopen(keyfile, "r");
    if (fp)
    {
        while (1)
        {
            if (fgets(buf, 64, fp) == 0)
                break;
            sprintf(keywords[ii], buf);
            ii++;
        }
        fclose(fp);
    }
    ii = 0;
    while (1)
    {
        len = strlen(keywords[ii]);
        if (len <= 1)
            break;
        keywords[ii][len - 1] = '\0';
        while ((pos = strstr(title, keywords[ii])))
        {
            key = 1;
            break;

        }
        ii++;
    }

    ii = 0;
    while (1)
    {
        len = strlen(keywords[ii]);
        if (len <= 1)
            break;

        while ((pos = strstr(content, keywords[ii])))
        {
            key = 1;
            break;

        }
        ii++;

    }
    if (title[0] == 0)
        http_quit_print("���±���Ҫ�б���");
    sprintf(dir, "boards/%s/.DIR", board);
    if (!has_post_perm(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print("����������Ψ����, ����������Ȩ���ڴ˷�������.");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print
            ("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ��巢������.������������� BBSHelp ���� sysop �巢����ѯ��");
#endif
    sprintf(filename, "boards/%s/deny_users", board);
    if (file_has_word(filename, currentuser.userid))
        http_quit_print("�ܱ�Ǹ, �㱻������Աֹͣ�˱����postȨ��.");
    if (loginok && abs(time(0) - *(int *) (u_info->from + 56)) < 6)
    {
        *(int *) (u_info->from + 56) = time(0);
        http_quit_print("���η��ļ������, ����Ϣ���������");
    }
    if (loginok)
        *(int *) (u_info->from + 56) = time(0);
    sprintf(filename, "tmp/%d.tmp", getpid());
    unlink(filename);
    f_append(filename, content);
    if (brd->flag & ANONY_FLAG || brd->flag & POSITIVE_ANONY_FLAG)
    {
        noip = anony;
    }
    else
        noip = 0;
    if (key)
    {
        r = post_article("junk", title, filename, currentuser.userid,
                         currentuser.username, fromhost, sig - 1,
                         autocr, currid, reid);
        http_quit_print("���µı���������а����˱�վ������Ĺؼ��֣�");
    }
    else
    {
        if (noip)
            r = post_article(board, title, filename, get_anonyid(currentuser.userid, board),
                             "����������ʹ", "������ʹ�ļ�", -1,
                             1, currid, reid);
        else
            r = post_article(board, title, filename,
                             currentuser.userid,
                             currentuser.username, fromhost,
                             sig - 1, autocr, currid, reid);
    }

    if (r <= 0)
        http_quit_print("�ڲ������޷�����");
    sprintf(buf, "M.%d.A", r);
#ifdef LOGHANDLER
    log2db2(currentuser.userid, fromhost, buf, board, title, content, reid);
#endif
    if (loginok)
    {
        brc_init(currentuser.userid, board);
        brc_add_read(buf);
        brc_update(currentuser.userid, board);
    }
    unlink(filename);
    sprintf(buf, "bbswapdoc?board=%s", board);
    if (loginok)
    {
        if (!junkboard(brd->filename) && !is_hide_board(brd->level))
        {
            currentuser.numposts++;
            save_user_data(&currentuser);
            write_posts(currentuser.userid, board, title);
        }
        else
        {
            save_user_data(&currentuser);
        }
    }
    f_append("trace.post", "b");
    if (loginok && has_BM_perm(&currentuser, board))
    {
        char buf[256];

        sprintf(buf, "%24.24s %s %s W_P\n", Ctime(time(0)),
                currentuser.userid, board);
        f_append("bm2.log", buf);
    }
    redirect(buf);
    return 0;
}
