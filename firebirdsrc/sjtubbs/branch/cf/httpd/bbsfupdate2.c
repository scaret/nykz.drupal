#include "www.h"
static struct dir x;


int bbsfupdate2_main()
{

    FILE * fp;

    struct shortfile *brd;

    char file[80], board[80];



    init_all();

    strsncpy(board, getparm("board"), 30);


    if (!loginok)
        http_fatal("�Ҵҹ����޷������ļ�, ���ȵ�¼");

    strsncpy(file, getparm("file"), 40);

    fp = fopen(UPLOAD_PATH "/.DIR", "r+");     //? w+ ?
    flock(fileno(fp), LOCK_EX);


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

            if (!has_BM_perm(&currentuser, x.board)
                    &&
                    strcmp(x.userid, currentuser.userid)
                    &&
                    !(currentuser.userlevel & PERM_OBOARDS))
            {

                http_fatal("����Ȩ���±��ļ�");

            }


            x.level = atoi(getparm("level"));

            strsncpy(x.exp, getparm("exp"), 72);

            x.live = atoi(getparm("live"));


            fseek(fp, -sizeof(x), SEEK_CUR);

            fwrite(&x, sizeof(x), 1, fp);


            {

                char buf[256];


                sprintf(buf, "%16.16s %s %d %s %s %s\n",

                        Ctime(time(0)) + 4, currentuser.userid, x.size,
                        fromhost, x.board, x.showname);

                f_append("update.log", buf);

            }

            printf("�������³ɹ�, ");

            printf
            ("URLΪ :<P><font color=green>http://bbs.sjtu.edu.cn/showfile?name=%s </font><br>\n",
             x.showname);

            printf
            ("(����URL(��ɫ����)�ɿ�����������ֱ������, ϵͳ���ж������Զ����ɳ�����.)<br>\n");

            printf
            ("�粻����, ��ֻ���ڰ������������������������.<hr color=green>\n");

            printf("�����ȼ�: %d��<br>\n", x.level + 1);

            printf("����ʱ��: %d��<br>\n", x.live);

            printf("�ļ���С: %d�ֽ�<br>\n", x.size);

            printf("�ļ�����: %s<br>\n", x.type);

            printf("�ļ�����: %s<br>\n", x.showname);

            printf("������ID: %s<br>\n", x.userid);

            printf("����ʱ��: %s<br>\n", Ctime(time(0)) + 4);

            printf("���ذ���: %s<br>\n", x.board);

            printf("�ļ�˵��: %s<br>\n", nohtml(x.exp));

            printf("������ID: %s<br>\n", currentuser.userid);

            printf
            ("<hr color=green><a href='javascript:history.go(-2)'>����</a>\n");

            http_quit();


            flock(fileno(fp), LOCK_UN);

            fclose(fp);

            return;

        }

    }

    fclose(fp);

    http_fatal("������ļ���");

}


