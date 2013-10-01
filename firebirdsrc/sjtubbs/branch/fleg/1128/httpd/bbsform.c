#include "www.h"

int bbsform_main()
{
    int type;

    init_all();
    type = atoi(getparm("type"));
    if (!loginok)
        http_fatal("����δ��¼, �����µ�¼��");
    printf("<font class=\"title\">%s - ��дע�ᵥ</font><hr>\n", BBSNAME);
    check_if_ok();
    if (type == 1)
    {
        check_submit_form();
        http_quit();
    }
    printf
    ("����, %s, ����ʵ��ϸ��д���¸�������, ������ͨ���󼴿ɻ�ñ�վע���û��ĸ���Ȩ��.<br><hr>\n",
     currentuser.userid);
    printf("<form method=post action=bbsform?type=1>\n");
    printf
    ("*��ʵ����: <input name=realname type=text maxlength=8 size=8 value='%s'> (��������)<br>\n",
     nohtml(currentuser.realname));
    printf
    ("*ѧУϵ��: <input name=dept type=text maxlength=64 size=32 value='%s'> (��У������дѧУ��ϵ���꼶, �ѹ���������д������λ)<br>\n",
     nohtml(currentuser.reginfo));
    printf
    ("*��ס��ַ: <input name=address type=text maxlength=64 size=32 value=''> (����ϸ��д���ľ�ס��ַ, �������һ����ƺ���)<br>\n");
    printf
    ("*����绰: <input name=phone type=text maxlength=32 size=32> (�������д, û�п�д'��')<br><hr><br>\n");
    printf("<input type=submit> <input type=reset>");
    http_quit();
}

int check_if_ok()
{
    if (user_perm(&currentuser, PERM_LOGINOK))
        http_fatal("���Ѿ�ͨ����վ�������֤, �����ٴ���дע�ᵥ.");
    if (has_fill_form())
        http_fatal("Ŀǰվ����δ��������ע�ᵥ�������ĵȴ�.");
}


//modified by rwzmm @ sjtubbs, new register form!
//the following code is for old register modula

//int check_submit_form()
//{
//    FILE *fp;
//    fp = fopen("new_register", "a");
//    if (fp == 0)
//        http_fatal("ע���ļ�������֪ͨSYSOP");
//    fprintf(fp, "usernum: %d, %s\n", getusernum(currentuser.userid) + 1,
//            Ctime(time(0)));
//    fprintf(fp, "userid: %s\n", currentuser.userid);
//    fprintf(fp, "realname: %s\n", getparm("realname"));
//    fprintf(fp, "dept: %s\n", getparm("dept"));
//    fprintf(fp, "addr: %s\n", getparm("address"));
//    fprintf(fp, "phone: %s\n", getparm("phone"));
//    fprintf(fp, "----\n");
//    fclose(fp);
//    printf
//    ("����ע�ᵥ�ѳɹ��ύ. վ�����������������, ��������������.");

//end

////the following code is for new register modula

int check_submit_form()
{
    FILE *fp = NULL;
	struct regform record = {0};

	record.usernum = getusernum(currentuser.userid) + 1;
	strncpy(record.date, Ctime(time(0)), 30);
	strncpy(record.userid, currentuser.userid, IDLEN + 2);
	strncpy(record.realname, getparm("realname"), NAMELEN);
	strncpy(record.dept, getparm("dept"), STRLEN);
	strncpy(record.addr, getparm("address"), STRLEN);
	strncpy(record.phone, getparm("phone"), STRLEN);
	record.status = 0;

    fp = fopen("register_sjtubbs", "a");
    if (fp == 0)
        http_fatal("ע���ļ�������֪ͨSYSOP");

	fwrite(&record, sizeof(struct regform), 1, fp);
    fclose(fp);

    printf("����ע�ᵥ�ѳɹ��ύ. վ�����������������, ��������������.");
}

////end
