#include "www.h"

//modified by rwzmm@sjtubbs
//delete .msgdir while delete .msgfile.me


int bbsmailmsg_main()
{
    char filename[80];
    char msgdirfile[80];

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܴ���ѶϢ�����ȵ�¼");
    sprintf(filename, "home/%c/%s/msgfile.me",
            toupper(currentuser.userid[0]), currentuser.userid);
    post_mail(currentuser.userid, "����ѶϢ����", filename,
              currentuser.userid, currentuser.username, fromhost, -1);

    sprintf(msgdirfile, "home/%c/%s/msgfile.me",
            toupper(currentuser.userid[0]), currentuser.userid);

    unlink(filename);
    unlink(msgdirfile);

    printf("ѶϢ�����Ѿ��Ļ���������<br>\n");
    printf("<a href='javascript:history.go(-2)'>���ٷ���</a>");
    http_quit();
}
