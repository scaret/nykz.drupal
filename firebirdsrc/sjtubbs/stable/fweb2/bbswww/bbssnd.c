#include <glib.h>
#include "www.h"
int get_unique_file_name(char *name)
{
    static int counter = 0;
    char buf[40];
    char *postfix, *p;

    postfix = strrchr(name, '.');
    if (NULL != postfix)
    {
        for (p = postfix + 1; *p != '\0'; p++)
            if (!isalnum(*p))
            {
                *postfix = '\0';
                break;
            }
        snprintf(buf, 40, "%ld%d%d%s", time(0), getpid(), counter,
                 postfix);
    }
    else
    {
        snprintf(buf, 40, "%ld%d%d", time(0), getpid(), counter);
    }
    strsncpy(name, buf, 40);
    counter++;
    return 0;

}

#ifdef LOGHANDLER
static int
log2db(char *user_id, char *ip_address, char *filename, char *board_name,
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
static int do_uploadfile()
{
    char path[1024], path2[1024], buf[4048], filename[1024], URL[1024];
    char *p, *buftext;
    cgiFilePtr file;
    struct dir x;
    int got, buflen;
    FILE *upfile;

    if (!loginok)
        http_quit_print("�Ҵҹ����޷�ִ�б����������ȵ�¼");

    if (!(cgiFormSuccess == cgiFormFileSize("up", &(x.size))
          && 0 < x.size
          && (1024 * 1024 + 1) > x.size
          && cgiFormSuccess == cgiFormFileName("up", filename, 1024)
          && cgiFormSuccess == cgiFormFileOpen("up", &file)))
    {
        printf
            ("δ�ϴ��ļ�,<a href='javascript:history.go(-1)'>����</a>\n");
        http_quit();
    }

    cgiFormFileContentType("up", x.type, 30);
    x.active = 1;
    strsncpy(x.userid, currentuser.userid, 13);
    cgiFormInteger("level", &(x.level), 0);
    cgiFormInteger("live", &(x.live), 180);
    cgiFormString("board", x.board, 20);
    cgiFormString("exp", x.exp, 80);
    if (cgiFormSuccess == cgiFormStringSpaceNeeded("text", &buflen))
    {
        buftext = mp_alloc(buflen);
        cgiFormString("text", buftext, buflen);
    }
    else
    {
        buftext = mp_alloc(1);
        buftext[0] = '\0';
    }
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, x.board))
    {
        cgiFormFileClose(file);
        http_quit_print
            ("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ����ϴ�����.������������� BBSHelp ���� sysop �巢����ѯ��");
    }
#endif
    if (!has_post_perm(&currentuser, x.board))
    {
        cgiFormFileClose(file);
        http_quit_print("���������������Ȩ��������������");
    }
    get_unique_file_name(filename);
    x.date = time(0);
    strncpy(x.showname, filename, 40);

    g_message("%s upload file %s at board %s from %s, file size: %d",
              currentuser.userid, x.showname, x.board, cgiRemoteAddr,
              x.size);

    snprintf(path, sizeof(path), UPLOAD_FILE_PATH2 "/%s", x.board);
    if (!file_exist(path))
        mkdir(path, 0755);
    snprintf(path2, sizeof(path), "%s/%s", path, x.showname);
    upfile = fopen(path2, "w+");
    if (!upfile)
    {
        cgiFormFileClose(file);
        http_quit_print("����д�ļ�");
    }
    while (cgiFormFileRead(file, buf, sizeof(buf), &got) == cgiFormSuccess)
    {
        fwrite(buf, got, 1, upfile);
    }
    fclose(upfile);
    cgiFormFileClose(file);

    sprintf(path, UPLOAD_PATH2 "/%s", x.board);
    upfile = fopen(path, "a");
    if (!upfile)
        http_quit_print("����д�ļ�");
    fwrite(&x, sizeof(struct dir), 1, upfile);
    fclose(upfile);
    sprintf(URL, UPLOAD_FILE_URL_HEADER "/%s/%s", x.board, x.showname);
    printf
        ("<tr><td><form name=\"form1\" method=post action=bbspst enctype='multipart/form-data'>\n");
    printf("<input type=hidden name=board value=\"%s\">", x.board);

    printf("<input type=hidden name=title value=\"%s\">",
           getparm("title"));

    printf("<input type=hidden name=file value=\"%s\">", getparm("file"));
    if ((p = strstr(buftext, "�� ��")) != NULL)
    {
        p[0] = 0;
        if (p[-1] == '\n')
        {
            p[-1] = 0;
            if (p[-2] == '\n')
                p[-2] = 0;
        }
    }
    printf("<input type=hidden name=text value=\"%s\n%s\">", buftext, URL);

    printf("�������سɹ�, ");
    printf("URLΪ :<P><font color=green>" UPLOAD_FILE_URL_HEADER
           "/%s/%s </font><br>\n", x.board, x.showname);
    printf("(����URL(��ɫ����)�����Զ��ӵ������µĽ�β<br>\n");
    printf("�����ȼ�: %d��<br>\n", x.level + 1);
    printf("����ʱ��: %d��<br>\n", x.live);
    printf("�ļ���С: %d�ֽ�<br>\n", x.size);
    printf("�ļ�����: %s<br>\n", x.type);
    printf("�ļ�����: %s<br>\n", x.showname);
    printf("������ID: %s<br>\n", x.userid);
    printf("����ʱ��: %s<br>\n", Ctime(time(0)) + 4);
    printf("���ذ���: %s<br>\n", x.board);
    printf("�ļ�˵��: %s<br>\n", nohtml(x.exp));
    printf("<br><hr><center><input type=submit value=ȷ��> ");
    printf("</form>");

    return 0;
}
static int if_exist_title(char *title)
{
    struct
    {
        int hash_ip;
        char title[64][60];
    }
    my_posts;
    char buf1[256];
    int n;
    FILE *fp;

    sethomefile(buf1, currentuser.userid, "my_posts");
    fp = fopen(buf1, "r+");
    if (fp == NULL)
        fp = fopen(buf1, "w+");
    fread(&my_posts, sizeof(my_posts), 1, fp);
    for (n = 0; n < 64; n++)
        if (!strncmp(my_posts.title[n], title, 50))
        {
            fclose(fp);
            return 1;
        };
    my_posts.hash_ip = (my_posts.hash_ip + 1) & 63;
    strncpy(my_posts.title[my_posts.hash_ip], title, 50);
    fseek(fp, 0, SEEK_SET);
    fwrite(&my_posts, sizeof(my_posts), 1, fp);
    fclose(fp);
    return 0;
}
static int is_hide_board(int level)
{
    if (level == 0 || level & (PERM_POSTMASK | PERM_NOZAP))
        return 0;
    return 1;
}

static void write_posts(char *id, char *board, char *title)
{
    FILE *fp;
    struct post_log x;

    strcpy(x.author, id);
    strcpy(x.board, board);
    if (!strncmp(title, "Re: ", 4))
        title += 4;
    strsncpy(x.title, title, 61);
    if (title[0] == 0)
        return;
    if (if_exist_title(title))
        return;
    x.date = time(0);
    x.number = 1;
    fp = fopen(".post", "a");
    fwrite(&x, sizeof(struct post_log), 1, fp);
    fclose(fp);
}

int bbssnd_main()
{
    char filename[128], dir[80], board[80], title[80], buf[80], *content;
    char reid_str[16];
    char reply_to_user[STRLEN];
    int autocr = 0, r, sig, size, flag = 0;
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
    cgiFormString("reply_to_user", reply_to_user, STRLEN);

	if (strcasecmp(cgiRequestMethod, "POST"))
        http_quit_print("post error");
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

    if (strlen(getparm("noreply")) > 0)
        flag = FILE_NOREPLY;

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
            r = post_article_ex(board, title, filename, get_anonyid(currentuser.userid, board),
                             "����������ʹ", "������ʹ�ļ�", -1,
                             1, currid, reid, flag);
        else
            r = post_article_ex(board, title, filename,
                             currentuser.userid,
                             currentuser.username, fromhost,
                             sig - 1, autocr, currid, reid, flag);
    }

    if (r <= 0)
        http_quit_print("�ڲ������޷�����");
    if (currid != 0)
        reid = r;
    sprintf(buf, "M.%d.A", r);
#ifdef LOGHANDLER
    log2db(currentuser.userid, fromhost, buf, board, title, content, reid);
#endif

#ifdef REPLY_SERVICE_HANDLER
    if (reply_to_user[0] != 0 && strcmp(board, "comment") !=0)
        reply_service_handle(board, title, currentuser.userid, reply_to_user, r);
#endif

    if (loginok)
    {
        brc_init(currentuser.userid, board);
        brc_add_read(buf);
        brc_update(currentuser.userid, board);
    }
    unlink(filename);
    sprintf(buf, "bbsdoc?board=%s", board);
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
