#include "www.h"
static int count_new_mails()
{
    struct fileheader x1;
    int unread = 0;
    char buf[1024];
    FILE *fp;

    if (currentuser.userid[0] == 0)
        return 0;
    sprintf(buf, "%s/mail/%c/%c/%s/.DIR", BBSHOME,
            toupper(currentuser.userid[0]), toupper(currentuser.userid[1]),
            currentuser.userid);
    fp = fopen(buf, "r");
    if (fp == 0)
        return 0;
    while (fread(&x1, sizeof(x1), 1, fp) > 0)
        if (!(x1.accessed[0] & FILE_READ))
            unread++;
    fclose(fp);
    return unread;
}

static int check_mybrd()
{
    FILE *fp;
    char file[200];
    char mybrd[GOOD_BRC_NUM][80], mybrd2[GOOD_BRC_NUM][80];
    int i, total = 0, mybrdnum = 0;

    if (!loginok)
        return 0;
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
        return 0;
    sethomefile(file, currentuser.userid, ".goodbrd");
    fp = fopen(file, "w");
    fwrite(mybrd2, 80, total, fp);
    fclose(fp);
    return 0;
}

int bbsleftnew_main()
{
    char *ptr;
    char buf[256];

    init_all();
    check_mybrd();
    printf("<html>");
    printf
        ("<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\">");
    printf("<link rel=stylesheet type=text/css href='/css/bbs.css'>");
    printf("<link rel=stylesheet type=text/css href='/css/blue.css'>");
    printf("<input type=hidden name=\"temp\" id=\"temp\" value=\"\">");
    printf("<script src=\"/js/jquery.js\"></script>");
    printf("<script src=\"/js/jquery_cookie.js\"></script>");
    printf("<script src=\"/js/bbsleft.js\"></script>");
    printf
        ("\n           <script>\n           function closebut(x, y) {\n           if(document.img0) document.img0.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n           if(document.img1) document.img1.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n           if(document.img2) document.img2.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n           if(document.img3) document.img3.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n           if(document.img4) document.img4.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n           if(document.img5) document.img5.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n           if(document.getElementById('div0')) document.getElementById('div0').style.display='none';\n           if(document.getElementById('div1')) document.getElementById('div1').style.display='none';\n           if(document.getElementById('div2')) document.getElementById('div2').style.display='none';\n           if(document.getElementById('div3')) document.getElementById('div3').style.display='none';\n           if(document.getElementById('div4')) document.getElementById('div4').style.display='none';\n           if(document.getElementById('div5')) document.getElementById('div5').style.display='none';\n           x.style.display='block';\n           y.src='"
         UPLOAD_SERVER_HEADER
         "/folder2.gif';\n       }\n           function t(x, y) {\n           if(x.style.display!='none') {\n           x.style.display='none';\n           y.src='"
         UPLOAD_SERVER_HEADER
         "/folder.gif';\n       }\n           else\n           closebut(x, y);\n       }\n           function openchat() {\n           url='bbschat';\n           chatWidth=screen.availWidth;\n           chatHeight=screen.availHeight;\n           winPara='toolbar=no,location=no,status=no,menubar=no,scrollbars=auto,resizable=yes,left=0,top=0,'+'width='+(chatWidth-10)+',height='+(chatHeight-27);\n           window.open(url,'_blank',winPara);\n       }\n           </script>\n           ");
    printf("<nobr>\n");
    printf("<body class=left>\n");
    printf
        ("<iframe onload=Restore_Last() id=\"framehide\" name=\"framehide\" width=\"0\" height=\"0\" src=\"\" frameborder=\"0\" scrolling=\"no\"></iframe>");
    if (!loginok)
    {
        printf
            ("\n               <form action=bbslogin target=_top method=post onsubmit=Remeber_Last()><br>\n               用户登录:<br>\n               帐号 <input style='height:20px;' type=text name=id maxlength=12 size=8><br>\n               密码 <input style='height:20px;' type=password name=pw maxlength=12 size=8><br>\n               <input style='width:45px; ' type=submit onClick=Remeber_Last() value=登录>\n	       <input style='width:45px; ' name=\"button\" type=\"button\" onClick=\"open('bbsreg', '', 'width=620,height=550')\" value=\"注册\" class=\"button\">\n               <hr class=left width=94px align=left></form>");
    }
    else
    {
        strcpy(buf, "未注册用户");
        printf
            ("<br>\n用户: <a class=left href=bbsqry?userid=%s target=f3>%s</a><br>",
             currentuser.userid, currentuser.userid);

        if (currentuser.userlevel & PERM_LOGINOK)
            strcpy(buf, "普通站友");
        if (currentuser.userlevel & PERM_BOARDS)
            strcpy(buf, "板主");
        if (currentuser.userlevel & PERM_XEMPT)
            strcpy(buf, "永久帐号");
        if (currentuser.userlevel & PERM_CHATCLOAK)
            strcpy(buf, "本站智囊");
        if (currentuser.userlevel & PERM_SYSOP)
            strcpy(buf, "本站站长");
        printf("级别: %s<br>", buf);
        printf
            ("<a class=left href=bbslogout target=_top>[注销本次登录]</a>\n                <hr class=left width=94px align=left>\n");
    }
    printf("<form action=bbssel target=f3>\n");
    printf
        ("选择讨论区: <br><input type=text name=board maxlength=15 size=8>");
    printf(" <input type=submit value=go>");
    printf("</form>\n");
    printf("<hr class=left width=94px align=left>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link0.gif><a class=left target=f3 href=bbssec>讨论区首页</a><br>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link0.gif><a class=left target=f3 href=bbs0an>精华公布栏</a><br>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link0.gif><a class=left target=f3 href=php/bbsindex.html>WEB导读</a><br>\n");
    if (loginok)
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link0.gif><a class=left target=f3 href=wiki>水源百科</a><br>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link0.gif><a class=left target=f3 href=bbstopb10>热门讨论区</a><br>\n");
    if (loginok)
    {
        printf
            ("<a href=javascript:submenu(\"main.hck\",1) target=\"_self\"><img id=\"img_main.hck\" src=\"/gif/folderclose.gif\" class=pm></a><a class=left href=\"bbsshowhome?boardfile=main.hck\" target=\"f3\">我的收藏夹</a><br>\n");
        printf("<div id=\"main.hck\" style='display:none'></div>");
    }
    printf("\n           <img src=" UPLOAD_SERVER_HEADER
           "/folder.gif name=img1><a class=left href=\"javascript: t(document.getElementById('div1'), document.img1)\">分类讨论区</a><br>\n           <div id=div1 style='display:none'>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=0> BBS系统 </a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=1> 上海交大</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=2> 学子院校</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=3> 电脑技术</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=4> 学术科学</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=5> 艺术文化</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=6> 体育运动</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=7> 休闲娱乐</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=8> 知性感性</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=9> 社会信息</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=10> 社团群体</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsboa?sec=11> 游戏专区</a><br>\n           <img src="
           UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsall> 全部讨论区</a><br>\n           </div>\n           ");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/folder.gif name=img2><a class=left href=\"javascript: t(document.getElementById('div2'), document.img2)\">谈天说地区</a><br>\n");
    printf("<div id=div2 style='display:none'>\n");
    if (loginok)
    {
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link.gif><a class=left href=bbsfriend target=f3> 在线好友</a><br>\n");
    }
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link.gif><a class=left href=bbsusr target=f3> 环顾四方</a><br>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link.gif><a class=left href=bbsqry target=f3> 查询网友</a><br>\n");
    if (currentuser.userlevel & PERM_PAGE)
    {
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link.gif><a class=left href=bbssendmsg target=f3> 发送讯息</a><br>\n");
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link.gif><a class=left href=bbsmsg target=f3> 查看所有讯息</a><br>\n");
    }
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link.gif><a class=left href=bbsshowinfo target=f3> 在线情况分析</a><br>\n");
    printf("</div>\n");
    ptr = "";
    if (currentuser.userlevel & PERM_CLOAK)
        ptr = "<img src=" UPLOAD_SERVER_HEADER
            "/link.gif><a class=left target=f3 onclick='return confirm(\"确实切换隐身状态吗?\")' href=bbscloak> 切换隐身</a><br>\n";

    if (loginok)
    {
            printf("\n               <img src=" UPLOAD_SERVER_HEADER
                   "/folder.gif name=img3><a class=left href=\"javascript: t(document.getElementById('div3'), document.img3)\">个人工具箱</a><br>\n               <div id=div3 style='display:none'>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsinfo> 个人资料</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsplan> 改说明档<a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbssig> 改签名档<a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbspwd> 修改密码</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsparm> 修改个人参数</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsmywww> WWW个人定制</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsnick> 临时改昵称</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsfall> 设定好友</a><br>\n               %s</div>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/folder.gif name=img5><a class=left href=\"javascript: t(document.getElementById('div5'), document.img5)\">处理信件区</a><br>\n"
                   , ptr);
            printf("<div id=div5 style='display:none'>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsnewmail> 阅览新邮件</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbsmail> 所有邮件</a><br>\n               <img src="
                   UPLOAD_SERVER_HEADER
                   "/link.gif><a class=left target=f3 href=bbspstmail> 发送邮件</a><br>\n               </div>\n               ");
                   
    }
    
    printf("<img src=" UPLOAD_SERVER_HEADER "/folder.gif name=img4>");
    printf
        ("<a class=left href=\"javascript: t(document.getElementById('div4'), document.img4)\">特别服务区</a><br>\n");
    printf("<div id=div4 style='display:none'>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsnotepad>看看留言板</a><br>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link.gif><a class=left target=f3 href=bbsalluser>所有使用者</a><br>\n");

    if (currentuser.userlevel & PERM_OBOARDS)
    {
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link.gif><a class=left target=f3 href=bbsallf>上载情况</a><br>\n");
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link.gif><a class=left target=f3 href=bbsfind2>上载查询</a><br>\n");
    }
    printf("</div>\n");
    if (loginok)
    {
        if (currentuser.userlevel & PERM_REALNAME)
        {
            printf("<img src=" UPLOAD_SERVER_HEADER
                   "/link0.gif><a class=left href=bbsfind target=f3>文章查询</a><br>\n");
        }
    }

    int juhuiRegIsOpen = 0;
    FILE *fp;

    if ((fp = fopen(REGIST_OPENCHECK_FILE, "r")) != NULL)
    {
        fscanf(FCGI_ToFILE(fp), "%d", &juhuiRegIsOpen);
        fclose(fp);
    }

    if (juhuiRegIsOpen || (currentuser.userlevel & PERM_OVOTE))
    {
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link0.gif><a class=left href=bbsregistview target=f3>站庆报名</a><br>\n");
    }

    printf("<img src=" UPLOAD_SERVER_HEADER
           "/link0.gif><a class=left href='javascript:top.f3.document.location.reload()'>刷新版面</a><br>\n");
    printf("<img src=" UPLOAD_SERVER_HEADER
           "/telnet.gif><a class=left href='telnet:%s'>Telnet登录</a><br>\n",
           BBSHOST);
    if (!loginok)
        printf("<img src=" UPLOAD_SERVER_HEADER
               "/link0.gif><a class=left href=\"javascript:void open('bbsreg', '', 'width=620,height=550')\">新用户注册</a><br>\n");
    if (loginok)
    {

        if (HAS_PERM(PERM_LOGINOK) && !HAS_PERM(PERM_POST))
            printf
                ("<script>alert('您被封禁了全站发表文章的权限, 请参看penalty板公告, 期满后在sysop板申请解封. 如有异议, 可在sysop板提出申诉.')</script>\n");
        if (count_new_mails() > 0)
            printf("<script>alert('您有新信件!')</script>\n");
    }
    if (loginok && !(currentuser.userlevel & PERM_LOGINOK)
        && !has_fill_form())
        printf
            ("<br><a class=left style='color:red; font-size:15px' target=f3 href=bbsform><b>填写注册单</b></a><br>\n");
    printf("<script>function addiv() { \n \
           if(document.getElementById('ad')) { \
               var addiv = document.getElementById('ad'); \n \
               var adb = document.getElementById('adb'); \n \
               if(addiv.style.visibility == 'hidden' ) { \n \
                   addiv.style.visibility = 'visible'; \n \
                   adb.innerHTML = '关闭'; \n \
               } else { \n \
                   addiv.style.visibility = 'hidden'; \n \
                   adb.innerHTML = '展开'; \n \
              } \n \
           } }\n \
           </script>");

    printf
        ("<br>[广告]&nbsp;<a href=# class=left><span id=adb onclick='javascript:addiv();'>关闭</span></a><br /><div id=ad>");

    if ((fp = fopen("etc/Ads_Left", "r")) != NULL)
    {
        while (fgets(buf, STRLEN, fp))
        {
            if (buf == NULL || strlen(buf) == 0 || buf[0] == '#')
                continue;
            printf("%s\n", buf);
        }
        fclose(fp);
    }

    printf("</div>");

    printf
        ("<script src=\"http://www.google-analytics.com/urchin.js\" type=\"text/javascript\"></script><script type=\"text/javascript\"> _uacct = \"UA-144567-2\"; _usample=13.6;urchinTracker(); </script>");
    printf("</body></html>");

    return 0;
}
