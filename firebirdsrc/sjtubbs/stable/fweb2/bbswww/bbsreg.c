#include "www.h"

int bbsreg_main()
{
    unsigned int r;

    init_all();
    js_head();
    if (noregist())
        http_quit_print("�Բ��𣬱�վ��ͣע�����û�");

    r = random();
	
    printf("<header>\n");
    printf("<SCRIPT LANGUAGE=\"JavaScript\">\n");
    printf("$(document).ready(function () {\n");
    printf("  $('input[name=\"userid\"]').keyup(function (){\n");
    printf("    var strname = $('input[name=\"userid\"]').val();\n");
    printf("    if (strname != \"\")\n");
    printf("    {\n");
    printf("      $.ajax(\"bbschkid?userid=\"+strname).done(\n");
    printf("        function(data) {\n");
    printf("          $('td[name=\"check_status\"]').html(data);\n");
    printf("        });\n");
    printf("    } \n");
    printf("    else {\n");
    printf("      $('td[name=\"check_status\"]').html(\"\")\n");
    printf("    }\n");
    printf("  });\n");
    printf("});\n");
    printf("function refresh(){\n");
    printf("var r = Math.floor(Math.random()*(1<<31));\n");
    printf("document.getElementById(\"vcodeimg\").src=\"bbsvcode?file=\"+r;\n");
    printf("document.form1.vcodefile.value=r;\n");
    printf("}\n");
    printf("</SCRIPT>\n");
    printf("</header>\n");

    printf
        ("<nobr><center><font class=\"title\">%s - ���û�ע��</font><hr>\n",
         BBSNAME);
    printf
        ("<font color=green>��ӭ���뱾վ. �������ϳ��ʺ�, �ǳ�, ��������, ϵͳ��Ϊ������, ����ʵ��д.</font>");
    printf("<form name=\"form1\" method=post action=bbsdoreg>\n");
    printf("<table width=700>\n");
    printf
        ("<tr><td align=right>*���������:<td align=left><input name=userid size=12 maxlength=12>(2-12�ַ�, ����ȫΪӢ����ĸ)\n");
    printf
        ("<tr><td><td name=check_status></tr>\n");
    printf
        ("<tr><td align=right>*����������:<td align=left><input type=password name=pass1 size=12 maxlength=12> (4-12�ַ�)\n");
    printf
        ("<tr><td align=right>*��ȷ������:<td align=left><input type=password name=pass2 size=12 maxlength=12>\n");
    printf
        ("<tr><td align=right>*��������֤��:<td align=left><input type=text name=vcode maxlength=6>\n");
    printf
        ("<tr><td align=right><input type=\"button\" value=\"������\" onclick=\"return refresh()\"><td align=left><img id=vcodeimg src=\"bbsvcode?file=%x\" />\n	<input type=hidden name=vcodefile value=%x /> \n",
         r, r);
    printf
        ("<tr><td align=right>*�������ǳ�:<td align=left><input name=username size=20 maxlength=32> (2-30�ַ�, ��Ӣ�Ĳ���)\n");
    printf
        ("<tr><td align=right>*������������ʵ����:<td align=left><input name=realname size=20> (��������, ����2������)\n");
    printf
        ("<tr><td align=right>*ѧУϵ��������λ:<td align=left><input name=dept size=40 maxlength=64> (����6���ַ�)\n");
    printf
        ("<tr><td align=right>*��ϸͨѶ��ַ:<td align=left><input name=address size=40 maxlength=64> (����10���ַ�)\n");
    printf("<tr><td align=right>*���ĳ�������:\n");
    printf("<td align=left><input name=year size=4 maxlength=4>��");
    printf("<input name=month size=2 maxlength=2>��");
    printf("<input name=day size=2 maxlength=2>��<br>");
    printf
        ("<tr><td align=right>�����������Ա�: <td align=left><input type=radio name=gender value=0 checked>�� ");
    printf("<input type=radio name=gender value=1>Ů<br>\n");
    printf("<tr><td align=right>������ʾ��ʽ: ");
    printf
        ("<td align=left><input type=radio name=xz value=0 checked>��ɫ�����Ա� ");
    printf("<input type=radio name=xz value=1>��������ɫ ");
    printf("<input type=radio name=xz value=2>����ʾ����\n");
    printf
        ("<tr><td align=right>����Email��ַ(��ѡ):<td align=left><input name=email size=40 maxlength=64>\n");
    printf
        ("<tr><td align=right>��������绰(��ѡ):<td align=left><input name=phone size=40>\n");
    printf("<tr><td align=right>��վ����(��ѡ):<td align=left>");
    printf
        ("<textarea name=words rows=3 cols=40 wrap=physicle></textarea>");
    printf("</table><br><hr>\n");
    printf
        ("<input type=submit value=�ύ���> <input type=reset value=������д>\n");
    printf("</form></center>\n");

    printf("</html>");
    return 0;
}
