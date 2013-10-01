#include "www.h"

int bbsdochkmail_main()
{
    char *id;
    char *code, rlcode[32];
    char path[1024];
    int fd;
    struct userec *user;

    init_all();
    id = getparm("id");
    code = getparm("code");
    sethomefile(path, id, REAL_NAME_CHK_CODE);
    fd = open(path, O_RDONLY);
    if (fd == -1)
        http_quit_print("�޴��û����Ѿ���֤");
    read(fd, rlcode, 32);
    close(fd);
    if (strncmp(code, rlcode, 32))
    {
        printf("�޴��û�,��ȷ�����ӵ�ַ��ȷ");
        return 0;
    }
    unlink(path);
    user = getuser(id);
    if (!user)
    {
        printf("�޴��û�,��ȷ�����ӵ�ַ��ȷ");
        return 0;
    }
    user->userlevel = user->userlevel | PERM_REALNAME;
    save_user_data(user);
    refreshto("/", 5);
    printf("��֤�ɹ���5���ת����ҳ");
    return 0;
}
