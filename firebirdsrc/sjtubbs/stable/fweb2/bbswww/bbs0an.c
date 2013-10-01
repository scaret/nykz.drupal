#include <glib.h>
#include "www.h"
static int get_count(char *);
int bbs0an_main()
{
    FILE *fp;
    int i, index = 0, total = 0;
    char *ptr, path[512], names[512], name[1024][80], file[1024][80],
        buf[512], title[256] = " ";
    char *board;

    cache_age = 60 * 60 * 24;
    init_all();
    strsncpy(path, getparm("path"), 511);
    if (strstr(path, "./"))
        http_quit_print("Ŀ¼������");
    for (i = 0; i < (int) strlen(path) - 1; i++)
        if (path[i] == '/' && path[i + 1] == '/')
            http_quit_print("Ŀ¼������");
    sprintf(names, "0Announce%s/.Names", path);
    fp = fopen(names, "r");
    if (fp == 0)
        http_quit_print("Ŀ¼������");
    board = getbfroma(path);
    if (board[0] && getbcache(board)
        && !has_read_perm(&currentuser, board))
    {
        fclose(fp);
        g_warning("denied access 0Announce b:%s u:%s ip:%s", board,
                  currentuser.userid, fromhost);
        http_quit_print("Ŀ¼������");
    }
    while (1)
    {
        if (fgets(buf, 511, fp) == 0)
            break;
        if (!strncmp(buf, "# Title=", 8))
            strcpy(title, buf + 8);
        if (!strncmp(buf, "Name=", 5) && total < 1023)
        {
            strcpy(name[total], rtrim(buf + 5));
            strcpy(file[total], "");
            total++;
        }
        if (!strncmp(buf, "Path=~", 6) && total > 0)
        {
            sprintf(file[total - 1], "%s", trim(buf + 6));
        }
    }
    fclose(fp);
    if (strstr(title, "SYSOPS") && !(currentuser.userlevel & PERM_SYSOP))
        http_quit_print("�����Ŀ¼");
    buf[0] = 0;
    if (board[0])
        sprintf(buf, "%s�徫����", board);
    else
        sprintf(buf, "����������");
    if (!strncasecmp(board, "p_", 2))
        sprintf(buf, "<a href=bbsqry?userid=%s>%s</a>�����ļ�",
                board + 2, board + 2);
    printf("<center>\n");
    printf
        ("<div align=center><font class=\"title\">%s - %s</font>����[�������: %d��]</div>",
         BBSNAME, buf, get_count(path));
    printf("<hr>\n");
    if (total <= 0)
    {
        printf("<br><< Ŀǰû������ >>\n");
        http_quit();
    }
    printf("<table width=610>\n");
    printf("<tr bgcolor=e8e8e8><td>���<td>���<td>����<td>����<td>����");
    for (i = 0; i < total; i++)
    {
        char *id;

        if (strstr(name[i], "SYSOPS"))
            continue;
        index++;
        if (strlen(name[i]) <= 39)
        {
            id = "";
        }
        else
        {
            name[i][38] = 0;
            id = name[i] + 39;
            if (!strncmp(id, "BM: ", 4))
                id += 4;
            ptr = strchr(id, ')');
            if (ptr)
                ptr[0] = 0;
        }
        printf("<tr><td>%d", index);
        sprintf(buf, "0Announce%s%s", path, file[i]);

        if (!file_exist(buf))
        {
            printf("<td>[����] <td>%s", nohtml2(name[i]));
        }
#ifdef STATIC_URL
        else if (file_isdir(buf))
        {
            printf("<td>[Ŀ¼] <td><a href=bbs0an,path,%s",
                   url_quote(path));
            printf("%s.html>%s</a>", url_quote(file[i]), nohtml2(name[i]));
        }
        else
        {
            printf("<td>[�ļ�] <td><a href=bbsanc,path,%s",
                   url_quote(path));
            printf("%s.html>%s</a>", url_quote(file[i]), nohtml2(name[i]));
        }
#else
        else if (file_isdir(buf))
        {
            printf("<td>[Ŀ¼] <td><a href=bbs0an?path=%s",
                   url_quote(path));
            printf("%s>%s</a>", url_quote(file[i]), nohtml2(name[i]));
        }
        else
        {
            printf("<td>[�ļ�] <td><a href=bbsanc?path=%s",
                   url_quote(path));
            printf("%s>%s</a>", url_quote(file[i]), nohtml2(name[i]));
        }
#endif
        if (id[0])
        {
            printf("<td>%s", userid_str(id));
        }
        else
        {
            printf("<td>");
        }
        printf("<td>%6.6s %s", Ctime(file_time(buf)) + 4,
               Ctime(file_time(buf)) + 20);
    }
#ifdef STATIC_URL
    sprintf(buf, "%s", url_quote(path));
    i = strlen(buf) - 1;
    if (i >= 3 && buf[i] == 'F' && buf[i - 1] == '2' && buf[i - 2] == '%')
        i -= 3;
    while (i >= 2 && (buf[i] != 'F' || buf[i - 1] != '2' || buf[i - 2] != '%'))
        i--;
    if (i > 2)
    {
        buf[i - 2] = 0;
    }    
    else
    {
        sprintf(buf, "%s", "%2F");
    }    
    printf
        ("</table><hr>[<a href=bbs0an,path,%s.html>������һҳ</a>] ", buf);
#else
    sprintf(buf, "%s", url_quote(path));
    i = strlen(buf) - 1;
    if (i >= 1 && buf[i] == '/')
        i--;
    while (i >= 0 && buf[i] != '/')
        i--;
    if (i > 0){
        buf[i] = 0;
    }    
    else
    {
        sprintf(buf, "%s", "/");
    }    
    printf
        ("</table><hr>[<a href=bbs0an?path=%s>������һҳ</a>] ", buf);
#endif
    if (board[0])
    {
#ifdef STATIC_URL
        if (!strncasecmp(board, "p_", 2))
            printf("[<a href=bbs0an,path,%s.html>%s�ļ���ҳ</a>]",
                   anno_path_of(board), board + 2);
        else
            printf("[<a href=bbsdoc,board,%s.html>��������</a>] ", board);
#else
        if (!strncasecmp(board, "p_", 2))
            printf("[<a href=bbs0an?path=%s>%s�ļ���ҳ</a>]",
                   anno_path_of(board), board + 2);
        else
            printf("[<a href=bbsdoc?board=%s>��������</a>] ", board);
#endif
    }
    printf("\n</html>");
    return 0;
}

static int get_count(char *path)
{
    FILE *fp;
    char buf[256];
    int counts = 0;

    sprintf(buf, "0Announce%s/.counts", path);
    if (!file_exist(buf))
    {
        fp = fopen(buf, "w+");
    }
    else
    {
        fp = fopen(buf, "r+");
    }
    flock(fileno(fp), LOCK_EX);
    fscanf(FCGI_ToFILE(fp), "%d", &counts);
    counts++;
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", counts);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    return counts;
}
