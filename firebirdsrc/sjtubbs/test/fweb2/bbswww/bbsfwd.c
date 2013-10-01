#include "www.h"

static int do_fwd(struct fileheader *x, char *board, char *target)
{
    char title[512], path[200];

    sprintf(path, "boards/%s/%s/%s", board, getfolderstr(x->filename),
            x->filename);
    if (!file_exist(path))
        http_quit_print("�ļ������Ѷ�ʧ, �޷�ת��");
    sprintf(title, "[ת��] %s", x->title);
    title[60] = 0;
    post_mail(target, title, path, currentuser.userid,
              currentuser.username, fromhost, -1);
    printf("������ת�ĸ�'%s'<br>\n", nohtml(target));
    printf("[<a href='javascript:history.go(-2)'>����</a>]");
    return 0;
}

int bbsfwd_main()
{
    struct fileheader *x;
    char board[80], file[80], target[80];

    init_all();
    strsncpy(board, getparm("board"), 30);
    strsncpy(file, getparm("file"), 30);
    strsncpy(target, getparm("target"), 30);
    if (!loginok)
        http_quit_print("�Ҵҹ��Ͳ��ܽ��б������");
    if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_CHAT))
        http_quit_print("��������˷���Ȩ��������ɱ��β���");
    if (!has_read_perm(&currentuser, board))
        http_quit_print("�����������");
    x = get_file_ent(board, file, 1);
    if (x == 0)
        http_quit_print("������ļ���");
    printf
        ("<center><font class=\"title\">%s - ת��/�Ƽ������ѡ���</font>[ʹ����: %s]<hr>\n",
         BBSNAME, currentuser.userid);
    if (target[0])
    {
        if (!strstr(target, "@"))
        {
            if (!getuser(target))
                http_quit_print("�����ʹ�����ʺ�");
            strcpy(target, getuser(target)->userid);
        }
        return do_fwd(x, board, target);
    }
    printf("<table><tr><td>\n");
    printf("���±���: %s<br>\n", nohtml(x->title));
    printf("��������: %s<br>\n", x->owner);
    printf("ԭ������: %s<br>\n", board);
    printf("<form action=bbsfwd method=post>\n");
    printf("<input type=hidden name=board value=%s>", board);
    printf("<input type=hidden name=file value=%s>", file);
    printf
        ("������ת�ĸ� <input name=target size=30 maxlength=30 value=%s> (������Է���id��email��ַ). <br>\n",
         currentuser.email);
    printf("<input type=submit value=ȷ��ת��></form>");
    printf("<html>");
    return 0;
}
