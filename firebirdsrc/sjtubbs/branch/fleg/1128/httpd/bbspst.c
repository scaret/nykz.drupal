#include "www.h"


int bbspst_main()
{
    FILE *fp;
    int i;
    char userid[80], dir[80], buf[512], path[512], file[512], board[512], reid_str[16],
    title[80] = "";
    struct fileheader f;
    struct shortfile *bp;


    init_all();
    strsncpy(board, getparm("board"), 20);
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܷ������£����ȵ�¼");
    if (!has_post_perm(&currentuser, board))
        http_fatal("�������������������Ȩ�ڴ���������������");
#ifdef	POST_AFTER_ONE_DAY
	if(too_young(&currentuser, board))
		http_fatal("��ע��ʱ�仹����24Сʱ����ʱ�����ڱ��巢������.������������� BBSHelp ���� sysop �巢����ѯ��");
#endif
	
	//����ȡ�ñ��ظ����ļ���������Ƿ��������£���ôfile = NULL
    strsncpy(file, getparm("file"), 20);

	bzero(reid_str, 16);
	
    if (file[0])
    {              // Re mode.
        if (file[0] != 'M')
            http_fatal("������ļ���");
        sprintf(dir, "boards/%s/.DIR", getbcache(board)->filename);
        fp = fopen(dir, "r");
        if (fp == 0)
            http_fatal("�����Ŀ¼");
        while (1)
        {
            if (fread(&f, sizeof(f), 1, fp) <= 0)
			{
				break;
			}
             
            if (!strcmp(f.filename, file))
            {
                char *ptr;
//				strsncpy(reid_str, itoa(f.reid), 16);
//				_itoa(f.reid, reid_str, 10);
				sprintf(reid_str, "%d", f.reid);

                if ((f.accessed[0] & FILE_NOREPLY)
                        || (getbcache(board)->flag & NOREPLY_FLAG))
                    http_fatal("����Ϊ���ɻظ�");
                ptr = f.title;
                if (!strncmp(ptr, "Re: ", 4))
                    ptr += 4;
                sprintf(title, "Re: %s", ptr);
                title[64] = 0;
                strsncpy(userid, f.owner, 60);
                goto L2;
            }
        }
        fclose(fp);
        http_fatal("������ļ���");
    }
L2:
    bp = getbcache(board);
    printf("<center>\n");
    printf("<font class=\"title\">%s - �������¡���</font>[ʹ����: %s]<hr>\n", BBSNAME,
           currentuser.userid);
    printf
    ("<table class=show_border bgcolor=f0f0f0>\n");
    printf("<tr><td>");
    printf("<font color=green>����ע������: <br>\n");
    printf
    ("��վ�涨ͬ�����ݵ������Ͻ��� 4 ���� 4 ���������������ظ�������ϸ���ݲμ��й�վ�档<br>\n");
    printf
    ("����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br>\n</font>");
    printf("��վ����ѿ��Ÿ������ع��ܣ���ӭʹ�á�\n");
    printf("<tr><td><form name=\"form1\" method=post action=bbssnd?board=%s&reidstr=%s>\n", 
		board, /*is_new_post,*/ reid_str);
    printf
    ("ʹ�ñ���: <input type=text name=\"title\" size=40 maxlength=100 tabindex=\"1\" value='%s'>",
     void3(void1(title)));
    printf(" ������: [%s]<br>\n", board);
    printf(" ���ñ������� ");
    if (bp->flag & ANONY_FLAG)
    {
        printf
        ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 checked>����");
        printf
        ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0>������<br>\n");
    }
    if (bp->flag & POSITIVE_ANONY_FLAG)
    {
        printf
        ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=1 >����");
        printf
        ("<input style='border: 1 solid #d0f0c0' type=radio name=anony value=0 checked>������<br>\n");
    }
    /*
       else{
       printf("<input style='border: 1 solid #d0f0c0' type=radio name=allowre value=1 checked>�ɻظ�");
       printf("<input style='border: 1 solid #d0f0c0' type=radio name=allowre value=0>���ɻظ�<br>\n");
       }
     */
    printf("���ߣ�%s", currentuser.userid);
    printf("  ʹ��ǩ���� ");
    printf("<input type=radio name=signature value=1 checked>1");
    printf("<input type=radio name=signature value=2>2");
    printf("<input type=radio name=signature value=3>3");
    printf("<input type=radio name=signature value=4>4");
    printf("<input type=radio name=signature value=5>5");
    printf("<input type=radio name=signature value=0>0");
    printf(" <input type=checkbox name=autocr checked>�Զ�����");
    printf(" <a target=_balnk href=bbssig>�鿴ǩ����</a> ");
    printf(" <a href=bbsupload?board=%s>�ϴ�����</a><br>\n", board);
    printf("<div align=\"center\">������Ϻ󣬿���ֱ�Ӱ�Ctrl+Enter���ٷ�������</div>\n");
    printf("\n<textarea name=text rows=20 cols=80 wrap=physicle tabindex=\"2\" onkeydown=\"ctlent()\">\n\n");
    if (file[0])
    {
        int lines = 0;

        printf("�� �� %s �Ĵ������ᵽ: ��\n", userid);
        sprintf(path, "boards/%s/%s", board, file);
        fp = fopen(path, "r");
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
    printf("<tr><td class=post align=center>");
    printf("<br><input type=submit value=��������> ");
    printf("<input type=reset value=���ݸ�ԭ></form>\n");
    printf("</table>");

    //added by hongliang on Nov. 21th, 2003 0:21
    printf("\n<script language=\"javascript\">\n<!--\n");
    if ( file[0] )
    {
        printf("document.form1.text.focus();\n");
    }
    else
    {
        printf("document.form1.title.focus();\n");
    }
    printf("function ctlent(eventobject)\n{\n\t");
    printf("if ( event.ctrlKey && window.event.keyCode==13 )\n\t");
    printf("{\n\t\tthis.document.form1.submit();\n\t}\n\t");
    printf("else if ( window.event.keyCode == 87 && window.event.ctrlKey )\n\t");
    printf("{\n\t\talert(\"��ֹ����رմ��ڣ����Ե����˶Ի������𾪻�\");\n\t}\n}");
    printf("\n-->\n</script>\n");

    http_quit();
}


//this function is added by hongliang on Nov. 23th, 2003 for auto undeny user
int seek_in_denyfile(char *board, char *seekstr)
{
    FILE   *fp;
    char    buf[STRLEN], buff[STRLEN];
    char   *namep, *date;
    char	flag;
    time_t nowtime;

    sprintf(buff, "boards/%s/deny_users", board);
    if ((fp = fopen(buff, "r")) == NULL)
        return 0;
    while (fgets(buf, STRLEN, fp) != NULL)
    {
        namep = (char *) strtok(buf, ": \n\r\t");
        if (namep != NULL && ci_strcmp(namep, seekstr) == 0)
        {
            fclose(fp);
            nowtime=time(0);
            getdatestring(nowtime,NA);
            flag = buf[55];
            date = (char *) (buf + 58);
            if (flag=='a' && strncmp (datestring,date,16) >= 0)
            {
                bbsautoundeny(board, seekstr);
                return 0;
            }
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int ci_strcmp(register char *s1, register char *s2)
{
    char c1, c2;

    while (1)
    {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 >= 'a' && c1 <= 'z')
            c1 += 'A' - 'a';
        if (c2 >= 'a' && c2 <= 'z')
            c2 += 'A' - 'a';
        if (c1 != c2)
            return (c1 - c2);
        if (c1 == 0)
            return 0;
    }
}
