#include "www.h"

int is_bad_id(char *userid)
{
    FILE *fp;
    char buf[STRLEN], ptr2[IDLEN + 2], *ptr;

    if ((fp = fopen(".badname", "r")) != NULL)
    {
        strtolower(ptr2, userid);
        while (fgets(buf, STRLEN, fp) != NULL)
        {
            ptr = strtok(buf, " \n\t\r");
            if (ptr != NULL && *ptr != '#')
            {
                if ((ptr[0] == '*' && strstr(ptr2, &ptr[1]) != NULL)
                        || (ptr[0] != '*' && !strcmp(ptr2, ptr)))
                {
                    fclose(fp);
                    return 1;
                }
            }
        }
        fclose(fp);
    }
    /*
       fp=fopen(".badname2", "r");
       if(fp) {
       while(1) {
       if(fscanf(fp, "%12s", buf)<=0) break;
       if(!strcasecmp(buf, s)) {
       fclose(fp);
       return 1;
       }
       }
       }
     */
    return 0;
}

int badymd(int y, int m, int d)
{
    int max[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
        max[2] = 29;
    if (y < 10 || y > 100 || m < 1 || m > 12)
        return 1;
    if (d < 0 || d > max[m])
        return 1;
    return 0;
}

int bbsdoreg_main()
{
    FILE *fp;
    struct userec x;
    int i, xz,vfd;
	unsigned int vcode = 0,real_vcode;
    char buf[80], filename[80], pass1[80], pass2[80], dept[80], phone[80],
    words[1024],vcodefile[32],vcodestr[8],vcodebuf[1024];
	struct regform record = {0};	//add by rwzmm @ sjtubbs

    init_all();
    bzero(&x, sizeof(x));
    xz = atoi(getparm("xz"));
    strsncpy(x.userid, getparm("userid"), 13);
    strsncpy(pass1, getparm("pass1"), 13);
    strsncpy(pass2, getparm("pass2"), 13);
    strsncpy(x.username, getparm("username"), 32);
    strsncpy(x.realname, getparm("realname"), 32);
    strsncpy(dept, getparm("dept"), 64);
    strsncpy(x.address, getparm("address"), 64);
    strsncpy(x.email, getparm("email"), 64);
    strsncpy(phone, getparm("phone"), 64);
    strsncpy(words, getparm("words"), 1000);
	strsncpy(vcodefile,getparm("vcodefile"),30);
	strsncpy(vcodestr,getparm("vcode"),7);
	vcode = atoi(vcodestr);
	if(0==vcode || 0==strlen(vcodefile))
		http_fatal("��֤�벻��ȷ");
	snprintf(vcodebuf,1000,"httpd/vcode-tmp/%s",vcodefile);
	vfd=open(vcodebuf,O_RDONLY);
	if(-1==vfd)
		http_fatal("���ļ�����");
	read(vfd,&real_vcode,sizeof(real_vcode));
	close(vfd);
	if(vcode!=real_vcode)
		http_fatal("��֤�벻��ȷ2");
	unlink(vcodefile);
    x.gender = 'M';
    if (atoi(getparm("gender")))
        x.gender = 'F';
    x.birthyear = atoi(getparm("year")) - 1900;
    x.birthmonth = atoi(getparm("month"));
    x.birthday = atoi(getparm("day"));
    for (i = 0; x.userid[i]; i++)
        if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", toupper(x.userid[i])))
            http_fatal("�ʺ�ֻ����Ӣ����ĸ���");
    if (strlen(x.userid) < 2)
        http_fatal("�ʺų���̫��(2-12�ַ�)");
    if (strlen(pass1) < 4)
        http_fatal("����̫��(����4�ַ�)");
    if (strcmp(pass1, pass2))
        http_fatal("������������벻һ��, ��ȷ������");
    if (strlen(x.username) < 2)
        http_fatal("�������ǳ�(�ǳƳ�������2���ַ�)");
    if (strlen(x.realname) < 4)
        http_fatal("��������ʵ����(��������, ����2����)");
    if (strlen(dept) < 6)
        http_fatal("������λ�����Ƴ�������Ҫ6���ַ�(��3������)");
    if (strlen(x.address) < 6)
        http_fatal("ͨѶ��ַ��������Ҫ6���ַ�(��3������)");
    if (badstr(x.passwd) || badstr(x.username) || badstr(x.realname))
        http_fatal("����ע�ᵥ�к��зǷ��ַ�");
    if (badstr(dept) || badstr(x.address) || badstr(x.email)
            || badstr(phone))
        http_fatal("����ע�ᵥ�к��зǷ��ַ�");
    if (badymd(x.birthyear, x.birthmonth, x.birthday))
        http_fatal("���������ĳ�������");
    if (is_bad_id(x.userid))
        http_fatal("�����ʺŻ��ֹע���id, ������ѡ��");
    if (getuser(x.userid))
        http_fatal("���ʺ��Ѿ�����ʹ��,������ѡ��");
    if (getuser_like(x.userid))
        http_fatal("���ʺ��������ʺŹ������ƣ�������ѡ��");
    strsncpy(x.passwd, crypt_passwd(pass1), 14);
    strcpy(x.termtype, "vt100");
    strcpy(x.lasthost, fromhost);
    x.userlevel = PERM_BASIC;
    x.firstlogin = time(0);
    x.lastlogin = time(0);
    x.lastlogout = time(0);
    x.numlogins = 1;
    x.userdefine = -1;
    x.flags[0] = CURSOR_FLAG | PAGER_FLAG;
    if (xz == 1)
        x.userdefine ^= DEF_COLOREDSEX;
    if (xz == 2)
        x.userdefine ^= DEF_S_HOROSCOPE;
    adduser(&x);
//modified by rwzmm @ sjtubbs, new register form!
//the following code is for old register modula

//    fp = fopen("new_register", "a");
//    if (fp)
//    {
//        fprintf(fp, "usernum: %d, %s\n", getusernum(x.userid) + 1,
//                Ctime(time(0)));
//        fprintf(fp, "userid: %s\n", x.userid);
//        fprintf(fp, "realname: %s\n", x.realname);
//        fprintf(fp, "dept: %s\n", dept);
//        fprintf(fp, "addr: %s\n", x.address);
//        fprintf(fp, "phone: %s\n", phone);
//        fprintf(fp, "----\n");
//        fclose(fp);
//    }


//end

////the following code is for new register modula

		record.usernum = getusernum(x.userid) + 1;
		strncpy(record.date, Ctime(time(0)), 30);
		strncpy(record.userid, x.userid, IDLEN + 2);
		strncpy(record.realname, x.realname, NAMELEN);
		strncpy(record.dept, dept, STRLEN);
		strncpy(record.addr, x.address, STRLEN);
		strncpy(record.phone, phone, STRLEN);
		record.status = 0;
	    fp = fopen("register_sjtubbs", "a");
		if(fp)
		{
			fwrite(&record, sizeof(struct regform), 1, fp);
			fclose(fp);
		}

////end

    f_append("trace.post", "G");
    sprintf(filename, "home/%c/%s", toupper(x.userid[0]), x.userid);
    mkdir(filename, 0755);
    printf("<center><table><td><td><pre>\n");
    printf("�װ�����ʹ���ߣ����ã�\n\n");
    printf("��ӭ���� %s, �������ʺ��Ѿ��ɹ����Ǽ��ˡ�\n", BBSNAME);
    printf("��վ��ַ�� %s, ֧��WWW, telnet\n", BBSHOST);
    printf("�ȶ��ֵ�¼��ʽ.\n");
    printf("��Ŀǰӵ�б�վ������Ȩ��, �����Ķ����¡������ķ�������˽��\n");
    printf("�ż����������˵���Ϣ�����������ҵȵȡ�����ͨ����վ�����ȷ\n");
    printf("������֮���������ø����Ȩ�ޡ�Ŀǰ����ע�ᵥ�Ѿ����ύ\n");
    printf("�ȴ����ġ�һ�����24Сʱ���ھͻ��д𸴣������ĵȴ���ͬʱ��\n");
    printf("��������վ�����䡣\n");
    printf
    ("��������κ����ʣ�����ȥBBSHelp(��������)�巢��������\n\n</pre></table>");
    printf("<hr color=green><br>���Ļ�����������:<br>\n");
    printf("<table border=1 width=400>");
    printf("<tr><td>�ʺ�λ��: <td>%d\n", getusernum(x.userid));
    printf("<tr><td>ʹ���ߴ���: <td>%s (%s)\n", x.userid, x.username);
    printf("<tr><td>��  ��: <td>%s<br>\n", x.realname);
    printf("<tr><td>��  ��: <td>%s<br>\n", x.username);
    printf("<tr><td>��վλ��: <td>%s<br>\n", x.lasthost);
    printf("<tr><td>�����ʼ�: <td>%s<br></table><br>\n", x.email);
    printf
    ("<center><input type=button onclick='window.close()' value=�رձ�����></center>\n");
    newcomer(&x, words);
    sprintf(buf, "%s %-12s %d\n", Ctime(time(0)) + 4, x.userid,
            getusernum(x.userid));
    f_append("wwwreg.log", buf);
    sprintf(buf, "uid %d from %s", getusernum(x.userid), fromhost);
    log_usies("APPLY", buf);
}

int badstr(unsigned char *s)
{
    int i;

    for (i = 0; s[i]; i++)
        if (s[i] != 9 && (s[i] < 32 || s[i] == 255))
            return 1;
    return 0;
}

int newcomer(struct userec *x, char *words)
{
    FILE *fp;
    char filename[80];
    //added by hongliang on Nov. 23th, 2003 for displaying nickname
    char	nickname[80];

	time_t now = time(0);

    sprintf(nickname, "WWW������·: %s", x->username);

    sprintf(filename, "tmp/%d.tmp", getpid());
    fp = fopen(filename, "w");
    fprintf(fp, "��Һ�, \n\n");
    fprintf(fp, "���� %s(%s), ���� %s\n", x->userid, x->username,
            fromhost);
    fprintf(fp, "��������˵ر���, ���Ҷ��ָ��.\n\n");
    fprintf(fp, "���ҽ���:\n\n");
    fprintf(fp, "%s", words);
    fclose(fp);
    post_article("message", nickname, filename, x->userid,
                 x->username, fromhost, -1, 0, now, now);
    unlink(filename);
}

int adduser(struct userec *x)
{
    int i;
    FILE *fp;

    fp = fopen(".PASSWDS", "r+");
    flock(fileno(fp), LOCK_EX);
    for (i = 0; i < MAXUSERS; i++)
    {
        if (shm_ucache->userid[i][0] == 0)
        {
            strncpy(shm_ucache->userid[i], x->userid, 13);
            save_user_data(x);
            if (shm_ucache->number < i + 1)
                shm_ucache->number = i + 1;
            break;
        }
    }
    flock(fileno(fp), LOCK_UN);
    fclose(fp);
}
