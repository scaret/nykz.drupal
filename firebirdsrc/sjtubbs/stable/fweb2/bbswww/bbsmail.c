#include "www.h"

int getmailboxsize(char *userid, unsigned int userlevel)
{
    int i = 0;

    if (userlevel & (PERM_CHATCLOAK))
        return 6000;
    if (userlevel & (PERM_BOARDS))
    {
        i = getbmns(userid);
        return (4000 + 600 * (i - 1));
    }

    if (userlevel & (PERM_BASIC))
        return 2000;
    return 5;
}

int getmailsize(char *userid)
{
    struct fileheader fcache;
    struct stat DIRst, SIZEst, st;
    char sizefile[50], dirfile[256], mailfile[256];
    FILE *fp;
    int mailsize = 9999, fd, ssize = sizeof(struct fileheader);

    setmdir(dirfile, userid);
    sprintf(sizefile, "tmp/%s.mailsize", userid);
    if (stat(dirfile, &DIRst) == -1 || DIRst.st_size == 0)
        mailsize = 0;
    else if (stat(sizefile, &SIZEst) != -1 && SIZEst.st_size != 0
             && SIZEst.st_ctime >= DIRst.st_ctime)
    {
        fp = fopen(sizefile, "r");
        if (fp)
        {
            fscanf(FCGI_ToFILE(fp), "%d", &mailsize);
            fclose(fp);
        }
    }
    if (mailsize != 9999)
        return mailsize;

    mailsize = 0;
    fd = open(dirfile, O_RDONLY);
    if (fd != -1)
    {
        while (read(fd, &fcache, ssize) == ssize)
        {
            sprintf(mailfile, "mail/%c/%c/%s/%s",
                    toupper(userid[0]), toupper(userid[1]), userid,
                    fcache.filename);
            if (stat(mailfile, &st) != -1)
            {
                mailsize += (st.st_size / 1024 + 1);
            }
        }
        close(fd);
    }
    fp = fopen(sizefile, "w+");
    if (fp)
    {
        fprintf(fp, "%d", mailsize);
        fclose(fp);
    }
    return mailsize;
}

int bbsmail_main()
{
    FILE *fp;
    int filetime, i, start, total;
    int ret, max = 0, size = 0;
    char *ptr, buf[512], dir[80];
    struct fileheader *data;

    init_all();
    if (!loginok)
        http_quit_print("����δ��¼, ���ȵ�¼");
    strsncpy(buf, getparm("start"), 10);
    start = atoi(buf);
    if (buf[0] == 0)
        start = 999999;
    printf("<center>\n");
    printf("<hr>\n");
    sprintf(dir, "mail/%c/%c/%s/.DIR", toupper(currentuser.userid[0]),
            toupper(currentuser.userid[1]), currentuser.userid);
    total = file_size(dir) / sizeof(struct fileheader);
    if (total < 0 || total > 30000)
        http_quit_print("too many mails");
    data = (struct fileheader *) calloc(total, sizeof(struct fileheader));
    if (data == 0)
        http_quit_print("memory overflow");
    if (!file_exist(dir))
    {
        char buf[80];

        sprintf(buf, "mail/%c/%c/%s", toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid);
        if (!file_exist(buf))
            mkdir(buf, 0770);
        fp = fopen(dir, "a+");
        fclose(fp);
    }
    fp = fopen(dir, "r");
    if (fp == 0)
        http_quit_print("dir error");
    total = fread(data, sizeof(struct fileheader), total, fp);
    fclose(fp);
    if (start > total - 19)
        start = total - 19;
    if (start < 0)
        start = 0;
    printf("<table width=650>\n");
    printf("<form action=bbsman2 method=post>\n");
    printf
        ("<tr bgcolor=e8e8e8><td>���<td>ɾ<td>״̬<td>������<td>����<td>�ż�����\n");
    for (i = start; i < start + 19 && i < total; i++)
    {

        int type = 'N';
        char filename[256];

        printf("<tr><td>%d", i + 1);
        printf("<td><input style='height:14px' type=checkbox name=box%s>",
               data[i].filename);
        if (data[i].accessed[0] & FILE_READ)
            type = ' ';
        if (data[i].accessed[0] & FILE_MARKED)
            type = (type == 'N') ? 'M' : 'm';

        printf("<td>%c", type);
        ptr = strtok(data[i].owner, " (");
        if (ptr == 0)
            ptr = " ";
        ptr = nohtml(ptr);
        printf("<td><a href=bbsqry?userid=%s>%13.13s</a>", ptr, ptr);
        filetime = atoi(data[i].filename + 2);
        printf("<td>%12.12s", Ctime(filetime) + 4);
        printf("<td><a href=bbsmailcon?file=%s&num=%d>",
               data[i].filename, i);
        if (strncmp("Re: ", data[i].title, 4))
            printf("�� ");
        hprintf("%42.42s", void1(data[i].title));
        sprintf(filename, "mail/%c/%c/%s/%s",
                toupper(currentuser.userid[0]),
                toupper(currentuser.userid[1]), currentuser.userid,
                data[i].filename);
        printf("</a>%s\n", eff_size(filename));
    }
    free(data);
    printf("</table><hr>\n");
    max = getmailboxsize(currentuser.userid, currentuser.userlevel);
	size = getmailsize(currentuser.userid);
    printf
        ("<input style='height:20px' onclick='return confirm(\"ȷʵҪɾ�����б�ǵ��ż���\")' type=submit value='ɾ����ǵ��ż�'>\n");
    if (HAS_PERM(PERM_MAILINFINITY))
    {
        printf("[������������Ϊ: ������, �ż�����: %d��] ", total);
    }
    else
    {    
        printf("[������������Ϊ: %dK, Ŀǰ��ʹ��: %dK, �ż�����: %d��] ", max,
           size, total);
    }    
	if((!HAS_PERM(PERM_MAILINFINITY)) && (size >= max))
	{
		printf("[<span style=\"color:red;\">�ż��������޷��������ż�</span>]\n");
	}
	else
	{
		printf("[<a href=\"bbspstmail\">�������ż�</a>]\n");
	}
    printf("</form>\n");
    if (start > 0)
    {
        i = start - 19;
        if (i < 0)
            i = 0;
        printf("[<a href=bbsmail?start=0>��һҳ</a>] ");
        printf("[<a href=bbsmail?start=%d>��һҳ</a>] ", i);
    }
    if (start < total - 19)
    {
        i = start + 19;
        if (i > total - 1)
            i = total - 1;
        printf("[<a href=bbsmail?start=%d>��һҳ</a>] ", i);
        printf("[<a href=bbsmail>���һҳ</a>]");
    }
    printf
        ("<form><input type=submit value=��ת��> �� <input style='height:20px' type=text name=start size=3> ��</form>");
	if((!HAS_PERM(PERM_MAILINFINITY)) && (size >= max))
        printf("<script>alert('�����ż�����, ����������ż�!')</script>\n");
    http_quit();
    return 0;
}

