#include "www.h"

int bbsleft_main()
{
    char /*buf[1024],*/ *ptr;

    init_all();
    check_mybrd();
    printf("<style type=text/css>\n");
    printf("A {color: #000080}\n");
    printf("BODY {BACKGROUND-COLOR: #c0c0c0; }\n");
    //printf("BODY {BACKGROUND-COLOR: #c0c0c0; FONT-SIZE: 12px;}\n");
    printf("</style>\n");
    printf("
           <script>
           function closebut(x, y) {
           if(document.img0) document.img0.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img1) document.img1.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img2) document.img2.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img3) document.img3.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img4) document.img4.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.img5) document.img5.src='"UPLOAD_SERVER_HEADER"/folder.gif';
           if(document.getElementById('div0')) document.getElementById('div0').style.display='none';
           if(document.getElementById('div1')) document.getElementById('div1').style.display='none';
           if(document.getElementById('div2')) document.getElementById('div2').style.display='none';
           if(document.getElementById('div3')) document.getElementById('div3').style.display='none';
           if(document.getElementById('div4')) document.getElementById('div4').style.display='none';
           if(document.getElementById('div5')) document.getElementById('div5').style.display='none';
           x.style.display='block';
           y.src='"UPLOAD_SERVER_HEADER"/folder2.gif';
       }
           function t(x, y) {
           if(x.style.display!='none') {
           x.style.display='none';
           y.src='"UPLOAD_SERVER_HEADER"/folder.gif';
       }
           else
           closebut(x, y);
       }
           function openchat() {
           url='bbschat';
           chatWidth=screen.availWidth;
           chatHeight=screen.availHeight;
           winPara='toolbar=no,location=no,status=no,menubar=no,scrollbars=auto,resizable=yes,left=0,top=0,'+'width='+(chatWidth-10)+',height='+(chatHeight-27);
           window.open(url,'_blank',winPara);
       }
           </script>
           ");
    printf("<nobr>\n");
    if (!loginok )
    {
        printf("<center>
               <form action=bbslogin target=_top method=post><br>
               �û���¼<br>
               �ʺ� <input style='height:20px;' type=text name=id maxlength=12 size=8><br>
               ���� <input style='height:20px;' type=password name=pw maxlength=12 size=8><br>
               <input style='width:72px; height:22px; BACKGROUND-COLOR: #d0f0c0' type=submit value=��¼��վ>
	       </form>
               </center>
               <body>
               ");
    }
    else
    {
        char buf[256] = "δע���û�";

        printf("<br>\n�û�: <a href=bbsqry?userid=%s target=f3>%s</a><br>",
               currentuser.userid, currentuser.userid);
        //                if(currentuser.userlevel & PERM_LOGINOK) strcpy(buf, cexp(countexp(&currentuser), currentuser.user_exp_type));
        if (currentuser.userlevel & PERM_LOGINOK)
            strcpy(buf, "��ͨվ��");
        if (currentuser.userlevel & PERM_BOARDS)
            strcpy(buf, "����");
        if (currentuser.userlevel & PERM_XEMPT)
            strcpy(buf, "�����ʺ�");
        if (currentuser.userlevel & PERM_CHATCLOAK)
            strcpy(buf, "��վ����");
        if (currentuser.userlevel & PERM_SYSOP)
            strcpy(buf, "��վվ��");
        printf
        ("����: %s<hr style='color:2020f0; height:1px' width=84px align=left>",
         buf);
        printf("<a href=bbslogout target=_top>ע�����ε�¼</a><br>\n");
    }
        printf("<center><form action=bbssel target=f3>\n");
        printf
        ("ѡ��������: <br><input type=text name=board maxlength=15 size=8>");
        //printf(" <input type=submit value=ȷ��>");
        printf("</form></center>\n");
    printf
    ("<hr style='color:2020f0; height=1px' width=84px align=left>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbssec>��������ҳ</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbs0an>����������</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbstop10>����ʮ��</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a target=f3 href=bbstopb10>����������</a><br>\n");
    if (loginok)
    {
        /*FILE *fp;*/
        int i, mybrdnum = 0;
        char mybrd[GOOD_BRC_NUM][80];

        printf
        ("<img src="UPLOAD_SERVER_HEADER"/folder.gif name=img0><a href=\"javascript: t(document.getElementById('div0'), document.img0)\">�ҵ��ղؼ�</a><br>\n");
        printf("<div id=div0 style='display:none'>\n");
        mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
        for (i = 0; i < mybrdnum; i++)
            printf
            ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsdoc?board=%s> %s</a><br>\n",
             mybrd[i], mybrd[i]);
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmybrd> Ԥ������</a><br>\n");
        printf("</div>\n");
    }
    printf("
           <img src="UPLOAD_SERVER_HEADER"/folder.gif name=img1><a href=\"javascript: t(document.getElementById('div1'), document.img1)\">����������</a><br>
           <div id=div1 style='display:none'>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=0> BBSϵͳ </a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=1> �Ϻ�����</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=2> ѧ��ԺУ</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=3> ���Լ���</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=4> ѧ����ѧ</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=5> �����Ļ�</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=6> �����˶�</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=7> ��������</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=8> ֪�Ը���</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=9> �����Ϣ</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=10> ����Ⱥ��</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsboa?sec=11> ��Ϸר��</a><br>
           <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsall> ȫ��������</a><br>
           </div>
           ");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/folder.gif name=img2><a href=\"javascript: t(document.getElementById('div2'), document.img2)\"≯��˵����</a><br>\n");
    printf("<div id=div2 style='display:none'>\n");
    if (loginok)
    {
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsfriend target=f3> ���ߺ���</a><br>\n");
    }
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsusr target=f3> �����ķ�</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsqry target=f3> ��ѯ����</a><br>\n");
    if (currentuser.userlevel & PERM_PAGE)
    {
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbssendmsg target=f3> ����ѶϢ</a><br>\n");
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=bbsmsg target=f3> �鿴����ѶϢ</a><br>\n");
    }
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a href=showinfo target=f3> �����������</a><br>\n");
    printf("</div>\n");
    ptr = "";
    if (currentuser.userlevel & PERM_CLOAK)
        ptr =
            "<img src<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 onclick='return confirm(\"ȷʵ�л�����״̬��?\")' href=bbscloak> �л�����</a><br>\n";
    if (loginok)
        printf("
               <img src="UPLOAD_SERVER_HEADER"/folder.gif name=img3><a href=\"javascript: t(document.getElementById('div3'), document.img3)\">���˹�����</a><br>
               <div id=div3 style='display:none'>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsinfo> ��������</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsplan> ��˵����<a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbssig> ��ǩ����<a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbspwd> �޸�����</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsparm> �޸ĸ��˲���</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmywww> WWW���˶���</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsnick> ��ʱ���ǳ�</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsfall> �趨����</a><br>
               %s</div>
               <img src="UPLOAD_SERVER_HEADER"/folder.gif name=img5><a href=\"javascript: t(document.getElementById('div5'), document.img5)\">�����ż���</a><br>
               <div id=div5 style='display:none'>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsnewmail> �������ʼ�</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmail> �����ʼ�</a><br>
               <img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbspstmail> �����ʼ�</a><br>
               </div>
               ", ptr);
    printf("<img src="UPLOAD_SERVER_HEADER"/folder.gif name=img4>");
    printf
    ("<a href=\"javascript: t(document.getElementById('div4'), document.img4)\">�ر������</a><br>\n");
    printf("<div id=div4 style='display:none'>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsnotepad>�������԰�</a><br>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsalluser>����ʹ����</a><br>\n");
    //printf("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsmovie>�����</a><br>\n");
    if (currentuser.userlevel & PERM_OBOARDS)
    {
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsallf>�������</a><br>\n");
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link.gif><a target=f3 href=bbsfind2>���ز�ѯ</a><br>\n");
    }
    printf("</div>\n");
    printf
    ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a href=bbsfind target=f3>���²�ѯ</a><br>\n");
    //      printf("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a href=bbssel target=f3>����������</a><br>\n");
    printf("<img src="UPLOAD_SERVER_HEADER"/telnet.gif><a href='telnet:%s'>Telnet��¼</a><br>\n",
           BBSHOST);
    if (!loginok)
        printf
        ("<img src="UPLOAD_SERVER_HEADER"/link0.gif><a href=\"javascript:void open('bbsreg', '', 'width=620,height=520')\">���û�ע��</a><br>\n");
    if (loginok)
    {
        if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_POST))
            printf
            ("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�penalty�幫��, ��������sysop��������. ��������, ����sysop���������.')</script>\n");
        if (count_new_mails() > 0)
            printf("<script>alert('�������ż�!')</script>\n");
    }
    if (loginok && !(currentuser.userlevel & PERM_LOGINOK)
            && !has_fill_form())
        printf
        ("<br><a style='color:red; font-size:15px' target=f3 href=bbsform><b>��дע�ᵥ</b></a><br>\n");
    printf("</body>");
}

int count_new_mails()
{
    struct fileheader x1;
    int unread = 0;
    char buf[1024];
    FILE *fp;

    if (currentuser.userid[0] == 0)
        return 0;
    sprintf(buf, "%s/mail/%c/%s/.DIR", BBSHOME,
            toupper(currentuser.userid[0]), currentuser.userid);
    fp = fopen(buf, "r");
    if (fp == 0)
        return;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
        if (!(x1.accessed[0] & FILE_READ))
            unread++;
    fclose(fp);
    return unread;
}

int check_mybrd()
{
    FILE *fp;
    char file[200];
    char mybrd[GOOD_BRC_NUM][80], mybrd2[GOOD_BRC_NUM][80];
    int i, total = 0, mybrdnum = 0;

    if (!loginok)
        return;
    mybrdnum = LoadMyBrdsFromGoodBrd(currentuser.userid, mybrd);
    total = 0;
    for (i = 0; i < mybrdnum; i++)
    {
        if (has_read_perm(&currentuser, mybrd[i]))
        {
            strcpy(mybrd2[total], mybrd[i]);
            total++;
        }
    }
    if (total == mybrdnum)
        return;
    fp = fopen(file, "w");
    fwrite(mybrd2, 80, total, fp);
    fclose(fp);
}
