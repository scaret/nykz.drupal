#include "www.h"
#include "bbs.h"

void post_article3(char *board, char *title, char *file, char *id,
                   unsigned int currid, unsigned int reid);
int bbsdelreturn_main()
{
    FILE *fp;
    struct shortfile *brd;
    struct fileheader f;
    struct userec *u;
    char buf[80], dir[80], path[80], board[80], file[80], *id, *p,
        UTitle[128];
    int num = 0;
    int i;

    init_all();
    if (!loginok)
        http_quit_print("���ȵ�¼");
    id = currentuser.userid;
    strsncpy(board, getparm("board"), 60);
    strsncpy(file, getparm("file"), 20);
    if (!strcmp(board, "deleted"))
        http_quit_print("���岻�ָܻ���������");
    brd = getbcache(board);
    if (strncmp(file, "M.", 2) && strncmp(file, "G.", 2))
        http_quit_print("����Ĳ���");
    if (strstr(file, ".."))
        http_quit_print("����Ĳ���");
    if (brd == 0)
        http_quit_print("�������");
    if (!has_post_perm(&currentuser, board))
        http_quit_print("�����������");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board))
        http_quit_print
            ("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ��巢������.������������� BBSHelp ���� sysop �巢����ѯ��");
#endif
    sprintf(dir, "boards/%s/.RETURNDIR", board);
    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("����Ĳ���");
    while (1)
    {
        if (fread(&f, sizeof(struct fileheader), 1, fp) <= 0)
            break;
        if (!strcmp(f.filename, file))
        {
            if (strcasecmp(id, f.owner)
                && !has_BM_perm(&currentuser, board))
            {
                fclose(fp);
                http_quit_print("����Ȩ�ָ�����");
            }

            strcpy(UTitle, f.title);
            if ((p = strrchr(UTitle, '-')))
            {
                *p = 0;
                for (i = strlen(UTitle) - 1; i >= 0; i--)
                {
                    if (UTitle[i] != ' ')
                        break;
                    else
                        UTitle[i] = 0;
                }
            }
            strncpy(f.title, UTitle, STRLEN);
            f.accessed[0] = 0;

            return_del_article(dir, sizeof(struct fileheader), num);
            post_article3(board, f.title, file, f.owner, f.id, f.reid);

            printf
                ("�ָ��ɹ�.<br><a href='bbsdocdel?board=%s'>���ر�����������վ</a>",
                 board);
            u = getuser(f.owner);
            if (!junkboard(brd->filename) && u)
            {
                if (u->numposts > 0)
                    u->numposts++;
                save_user_data(u);
            }
            sprintf(buf, "%s %-12s bbsdel %s\n",
                    Ctime(time(0)) + 4, id, board);
            f_append("trace", buf);
            {
                char buf[256];

                sprintf(buf,
                        "%24.24s %s %s Do_Web_D_RETURN\n",
                        Ctime(time(0)), currentuser.userid, board);
                f_append("bm2.log", buf);
            }
            http_quit();
        }
        num++;
    }
    fclose(fp);
    printf("�ļ�������, �ָ�ʧ��.<br>\n");
    printf("<a href='bbsdocdel?board=%s'>���ر�����������վ</a>", board);
    printf("\n</html>");
    return 0;
}

void
post_article3(char *board, char *title, char *file, char *id,
              unsigned int currid, unsigned int reid)
{
    FILE *fp;
    char buf[1024];
    struct fileheader header;

    bzero(&header, sizeof(header));
    strcpy(header.owner, id);
    header.id = currid;
    header.reid = reid;
    sprintf(header.filename, "%s", file);
    strsncpy(header.title, title, 60);
    header.accessed[0] = 0;
    sprintf(buf, "boards/%s/.DIR", board);
    fp = fopen(buf, "a");
    fwrite(&header, sizeof(header), 1, fp);
    fclose(fp);
}
