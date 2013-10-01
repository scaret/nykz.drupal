#include "www.h"

extern char *get_title(char *title);

extern char *setmodulefile(char *buf, char *boardname, char *filename);
extern int ismoduleexist(char *board);

extern int updateqzdenyuser(int district, char userid[IDLEN]);

extern int seek_in_qzdenyfile(struct userec *user);

int bbswappst_main()
{
    FILE *fp;
    int i;
    char userid[80], dir[80], buf[512], path[512], file[512], board[512],
        reid_str[16], title[80] = "";
    int check = 0;
    char ch[3];
    struct fileheader f;
    struct shortfile *bp;

    time_t now = time(0);
    struct tm *tm;

    tm = localtime(&now);

    init_all();

    if (cgiFormSuccess == cgiFormString("check", ch, 2))
    {
        check = 1;
    }
    if (check == 0)
    {
        strsncpy(board, getparm("board"), 20);
    }
    else
    {
        cgiFormString("board", board, 20);
    }
    if (!loginok)
        http_quit_print("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
    seek_in_qzdenyfile(&currentuser);
    if (!has_post_perm(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print("�������������������Ȩ�ڴ���������������");
#ifdef	POST_AFTER_ONE_DAY
    if (too_young(&currentuser, board)
        && (strcmp(board, "WelcomeNewer") != 0
            || strcmp(currentuser.userid, "guest") == 0))
        http_quit_print
            ("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ��巢������.������������� BBSHelp ���� sysop �巢����ѯ��");
#endif
    if (shm_bcache->all_no_post_end >= shm_bcache->all_no_post_begin)
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {
            if ((unsigned int) tm->tm_hour >= shm_bcache->all_no_post_begin
                && (unsigned int) tm->tm_hour <
                shm_bcache->all_no_post_end)
            {
                printf("��վ��ÿ�� %d ʱ�� %d ʱ��ȫվֻ����",
                       shm_bcache->all_no_post_begin,
                       shm_bcache->all_no_post_end);
                http_quit();
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {

            if ((unsigned int) tm->tm_hour >= shm_bcache->no_post_begin
                && (unsigned int) tm->tm_hour < shm_bcache->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {

                    printf
                        ("��վ��ÿ��%dʱ��%dʱ��ͣУ���û��ķ���Ȩ�ޡ�\n\n          ������������� BBSHelp ���� sysop ��������֪ͨ��˵����",
                         shm_bcache->no_post_begin,
                         shm_bcache->no_post_end);
                    http_quit();

                }
            }
        }
    }
    else
    {
        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8))
        {
            if ((unsigned int) tm->tm_hour >= shm_bcache->all_no_post_begin
                || (unsigned int) tm->tm_hour <
                shm_bcache->all_no_post_end)
            {
                printf("��վ��ÿ�� %d ʱ������ %d ʱ��ȫվֻ����",
                       shm_bcache->all_no_post_begin,
                       shm_bcache->all_no_post_end);
                http_quit();
            }

        }

        if (!HAS_PERM
            (PERM_SYSOP | PERM_SPECIAL6 | PERM_SPECIAL7 | PERM_SPECIAL8 |
             PERM_BOARDS))
        {

            if ((unsigned int) tm->tm_hour >= shm_bcache->no_post_begin
                || (unsigned int) tm->tm_hour < shm_bcache->no_post_end)
            {
                if (!insjtunetwork(fromhost))
                {

                    printf
                        ("��վ��ÿ��%dʱ������%dʱ��ͣУ���û��ķ���Ȩ�ޡ�\n\n          ������������� BBSHelp ���� sysop ��������֪ͨ��˵����",
                         shm_bcache->no_post_begin,
                         shm_bcache->no_post_end);
                    http_quit();

                }
            }
        }

    }

    if (check == 0)
    {
        strsncpy(file, getparm("file"), 20);
    }
    else
    {
        cgiFormString("file", file, 20);
    }

    bzero(reid_str, 16);

    if (file[0])
    {
        if (file[0] != 'M')
            http_quit_print("������ļ���");
        sprintf(dir, "boards/%s/.DIR", getbcache(board)->filename);
        fp = fopen(dir, "r");
        if (fp == 0)
            http_quit_print("�����Ŀ¼");
        while (1)
        {
            if (fread(&f, sizeof(f), 1, fp) <= 0)
            {
                break;
            }

            if (!strcmp(f.filename, file))
            {
                char *ptr;

                sprintf(reid_str, "%d", f.reid);

                if ((f.accessed[0] & FILE_NOREPLY)
                    || (getbcache(board)->flag & NOREPLY_FLAG))
                {
                    fclose(fp);
                    http_quit_print("����Ϊ���ɻظ�");
                }
                ptr = f.title;
                if (!strncmp(ptr, "Re: ", 4))
                    ptr += 4;
                sprintf(title, "Re: %s", ptr);
                title[64] = 0;
                strsncpy(userid, f.owner, 60);
                fclose(fp);
                goto L2;
            }
        }
        fclose(fp);
        http_quit_print("������ļ���");
    }
  L2:
    bp = getbcache(board);

    printf
        ("<font class=\"title\">%s - �������¡���</font>[ʹ����: %s] ������: [%s]<hr>\n",
         BBSNAME, currentuser.userid, board);
    printf("<body onkeydown=\"ctlent()\">");
    printf
        ("<form name=\"form1\" method=post action=bbswapsnd enctype='multipart/form-data'>\n");
    printf
        ("<input type=hidden name='board' value='%s'><input type=hidden name='file' value='%s'><input type=hidden name='reidstr' value='%s'>",
         board, file, reid_str);
    printf
        ("����: <input type=text name=\"title\" size=20 maxlength=100 tabindex=\"1\" value='%s' id=\"inputCH\">",
         get_title(title));
    printf("<br>");
    if (bp->flag & ANONY_FLAG)
    {
    	printf(" ���ñ������� ");
    
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 checked>����");
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0>������<br>\n");
    }
    if (bp->flag & POSITIVE_ANONY_FLAG)
    {
    	printf(" ���ñ������� ");
    
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 >����");
        printf
            ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0 checked>������<br>\n");
    }

    printf("<input type=hidden name=signature value=1>");
    printf("<input type=hidden name=autocr value=1>");
    printf(
        "\n<script language=\"JavaScript\" type=\"text/javascript\">\n"
        "setInterval(\"checkLength()\", 100);\n"
        "function checkLength() {\n"
        "var maxChars = 44, i = 0, len = 0;\n"
        "var which = document.getElementById(\"inputCH\");\n"
        "for (; i<which.value.length; i++) {\n"
        "if (which.value.charCodeAt(i) >= 128)\n"
        "len += 2;\n"
        "else\n"
        "len++;\n"
        "if (len > maxChars) break;\n"
        "}\n"
        "if (len > maxChars){\n"
        "if (which.value.charCodeAt(i) >= 128)\n"
        "len -= 2;\n"
        "else\n"
        "len--;\n"
        "which.value = which.value.substring(0,i);\n"
        "}\n"
        "var curr = maxChars - len;\n"
        "}\n"
        "</script>");

    printf
        ("\n<textarea id=text name=text rows=8 cols=20 wrap=physicle tabindex=\"2\" onkeydown=\"ctlent()\">");

    printf("%s", getparm("text"));
    if (check == 1)
    {
        char mpath[STRLEN];
        char module[90];

        cgiFormString("level", module, 80);
        strcat(module, "_M");
        sprintf(mpath, "vote/%s/%s", board, module);
        fp = fopen(mpath, "r");
        printf("\n");
        if (fp == NULL)
            http_quit_print("�޷����ļ�");
        while (fgets(buf, 500, fp) != NULL)
        {
            printf("%s", buf);
        }
        fclose(fp);
    }
    if (file[0])
    {
        int lines = 0;

        printf("\n\n�� �� %s �Ĵ������ᵽ: ��\n", userid);
        sprintf(path, "boards/%s/%s/%s", board, getfolderstr(file), file);
        fp = fopen(path, "r");
        if (fp == NULL)
            http_quit_print("�޷����ļ�2");
        if (fp)
        {
            for (i = 0; i < 3; i++)
                if (fgets(buf, 500, fp) == 0)
                    break;
            while (1)
            {
                if (fgets(buf, 500, fp) == 0)
                    break;
                if (!strncmp(buf, ": ��", 4))
                    continue;
                if (!strncmp(buf, ": : ", 4))
                    continue;
                if (!strncmp(buf, "--\n", 3))
                    break;
                if (buf[0] == '\n')
                    continue;
                if (lines++ >= 20)
                {

                    printf(": (��������ʡ��...)\n");
                    break;
                }
                if (!strcasestr(buf, "</textarea>"))

                    printf(": %s", buf);
            }
            fclose(fp);
        }
    }

    printf("</textarea>\n");

    printf("<br><input type=submit value=��������> ");
    printf("<input type=reset value=���ݸ�ԭ>\n");
	printf("<br><font color=green>����ע������: <br>\n");
    printf
        ("��վ�涨ͬ�����ݵ������Ͻ��� 4 ���� 4 ���������������ظ�������ϸ���ݲμ��й�վ�档<br>\n");
    printf
        ("����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br>\n</font>");
    printf("<hr><br>\n");

    printf("���ظ���: <input type=file name=up>");
    printf("<input type=hidden name=MAX_FILE_SIZE value=1048577>");
    printf("<input type=hidden name=board value='%s'>", board);
    printf("<input type=hidden name=level value=0>\n");
    printf("<input name=live type=hidden value=180>\n");
    printf("<br>�����ļ�Ҫ˵��: <input name=exp size=20 maxlength=72>\n");

    printf("<br><input type=submit value='��ʼ�ϴ��ļ�'> \n");


    printf("</body>");

    printf("</form>\n");

    printf("\n<script language=\"javascript\">\n<!--\n");
    printf("function ctlent()\n");
    printf("{\n");
    printf("k = event.keyCode;	\n");
    printf("if((event.ctrlKey&&event.keyCode==13))\n");
    printf("{document.form1.submit();}\n");

    printf("if(event.ctrlKey&&k==87)\n");
    printf("{alert(\"Ϊ��ֹ����С�Ĺرյ�ǰ���ڣ��ʵ����˶Ի���\");}}\n");
    printf("-->\n");
    printf("</script>\n");
    printf("</html>");
    return 0;
}
