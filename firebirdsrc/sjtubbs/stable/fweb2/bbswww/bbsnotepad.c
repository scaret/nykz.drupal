#include "www.h"

int bbsnotepad_main()
{
    FILE *fp;
    char buf[256];

    last_modified_time = file_time("etc/notepad");
    cache_age = 10 * 60;
    init_all();
    printf
        ("<center><font class=\"title\">%s - ���԰塡��</font>[����: %6.6s]</center>\n		    <hr><pre class=tight>\n",
         BBSNAME, Ctime(time(0)) + 4);
    fp = fopen("etc/notepad", "r");
    if (fp == 0)
    {
        printf("��������԰�Ϊ��");
        http_quit();
    }
    while (1)
    {
        if (fgets(buf, 255, fp) == 0)
            break;
        hprintf("%s", buf);
    }
    fclose(fp);
    printf("</html>");
    return 0;
}
