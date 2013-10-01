#include "www.h"

int bbsmnote3_main()
{
    FILE *fp;
    char path[256], buf[10000], board[256];

    init_all();
    printf("<center>\n");
    if (!loginok)
        http_fatal("�Ҵҹ��ͣ����ȵ�¼");
    strsncpy(board, getparm("board"), 30);
    if (!has_BM_perm(&currentuser, board))
        http_fatal("����Ȩ���б�����");
    strsncpy(board, getbcache(board)->filename, 30);
    sprintf(path, "vote/%s/note2", board);
    if (!strcasecmp(getparm("type"), "update"))
        save_note(path);
    printf("%s -- �༭�������� [������: %s]<hr>\n", BBSNAME, board);
    printf("<form method=post action=bbsmnote3?type=update&board=%s>\n",
           board);
    fp = fopen(path, "r");
    buf[0] = 0;
    if (fp)
    {
        fgets(buf, 256, fp);
        fclose(fp);
    }
    printf
    ("���������: <input type=text name=text maxlength=80 size=80 value=\"%s\"> \n",
     void1(nohtml(buf)));
    printf("<input type=submit value=����> ");
    printf("<input type=reset value=��ԭ>\n");
    printf("<hr>\n");
    http_quit();
}

int save_note(char *path)
{
    char buf[10000];
    FILE *fp;

    fp = fopen(path, "w");
    strsncpy(buf, getparm("text"), 9999);
    buf[80] = 0;
    fprintf(fp, "%s", buf);
    fclose(fp);
    printf("���������޸ĳɹ���<br>\n");
    printf("<a href='javascript:history.go(-2)'>����</a>");
    http_quit();
}
