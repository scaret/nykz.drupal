#include "www.h"

int bbs0an_main()
{
    FILE *fp;
    int i, index = 0, total = 0;
    char *ptr, path[512], names[512], name[1024][80], file[1024][80],
    buf[512], title[256] = " ";
    char *board;

    init_all();
    strsncpy(path, getparm("path"), 511);
    if (strstr(path, "./"))
        http_fatal("��Ŀ¼������");
    sprintf(names, "0Announce%s/.Names", path);
    fp = fopen(names, "r");
    board = getbfroma(path);
    if (board[0] && getbcache(board)
            && !has_read_perm(&currentuser, board))
    {
        FILE *fp = fopen("bad_att.log", "a");

        fprintf(fp, "%s %s %s %s\n", Ctime(time(0)), currentuser.userid,
                fromhost, board);
        fclose(fp);
        http_fatal("Ŀ¼������");
    }
    if (fp == 0)
        http_fatal("Ŀ¼������");
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
    if (strstr(title, "SYSOPS") && !(currentuser.userlevel & PERM_SYSOP))
        http_fatal("�����Ŀ¼");
    buf[0] = 0;
    if (board[0])
        sprintf(buf, "%s�徫����", board);
    else
	sprintf(buf, "����������");
    if (!strncasecmp(board, "p_", 2))
        sprintf(buf, "<a href=bbsqry?userid=%s>%s</a>�����ļ�", board + 2,
                board + 2);
    printf("<center>\n");
    printf("<div align=center><font class=\"title\">%s - %s</font>����[�������: %d��]</div>", BBSNAME, buf,
           get_count(path));
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
            printf("<td>[����] <td>%s", nohtml(name[i]));
        }
        else if (file_isdir(buf))
        {
            printf("<td>[Ŀ¼] <td><a href=bbs0an?path=%s%s>%s</a>", path,
                   file[i], nohtml(name[i]));
        }
        else
        {
            printf("<td>[�ļ�] <td><a href=bbsanc?path=%s%s>%s</a>", path,
                   file[i], nohtml(name[i]));
        }
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
    printf
    ("</table><hr>[<a href='javascript:history.go(-1)'>������һҳ</a>] ");
    if (board[0])
    {
        if (!strncasecmp(board, "p_", 2))
            printf("[<a href=bbs0an?path=%s>%s�ļ���ҳ</a>]",
                   anno_path_of(board), board + 2);
        else
            printf("[<a href=bbsdoc?board=%s>��������</a>] ", board);
    }
    http_quit();
}

int get_count(char *path)
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
    fscanf(fp, "%d", &counts);
    counts++;
    fseek(fp, 0, SEEK_SET);
    fprintf(fp, "%d\n", counts);
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
    return counts;
}
