#include "www.h"

int bbsudel_main()
{
    FILE *fp;
    struct dir x;
    struct shortfile *brd;
    char file[80], filename[80];
    int num = 0;

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ����޷�ɾ���ļ�, ���ȵ�¼");
    strsncpy(file, getparm("file"), 40);
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        http_fatal("��Ŀ¼");
    while (1)
    {
        if (fread(&x, sizeof(x), 1, fp) <= 0)
            break;
        if (!strncmp(x.showname, file, 36))
        {
            brd = getbcache(x.board);
            if (brd == 0)
                http_fatal("�ڲ�����10002");
            if (!has_BM_perm(&currentuser, x.board) &&
                    strcmp(x.userid, currentuser.userid) &&
                    !(currentuser.userlevel & PERM_OBOARDS))
            {
                http_fatal("����Ȩɾ�����ļ�");
            }
            sprintf(filename, UPLOAD_PATH "/%d", x.filename);
            unlink(filename);
            del_record(UPLOAD_PATH "/.DIR", sizeof(struct dir), num);
            sprintf(filename, "bbsfdoc?board=%s", x.board);
            redirect(filename);
            http_quit();
        }
        num++;
    }
    fclose(fp);
    http_fatal("������ļ���");
}
