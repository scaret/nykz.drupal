#include "www.h"

int bbsreg_main()
{
    unsigned int r;

    init_all();
    if (noregist())
        http_quit_print("�Բ��𣬱�վ��ͣע�����û�");

    r = random();
    printf
        ("<nobr><center><font class=\"title\">%s - ���û�ע��</font><hr>\n",
         BBSNAME);
    printf
        ("<font color=green>��ӭ���뱾վ. �������ϳ��ʺ�, �ǳ�, ��������, ϵͳ��Ϊ������, ����ʵ��д.</font>");
    printf("<form name=\"form1\" method=post action=bbsdoreg>\n");
    printf("<table width=700>\n");
    printf
        ("<tr><td align=right>*���������:<td align=left><input name=userid size=12 maxlength=12> (2-12�ַ�, ����ȫΪӢ����ĸ) <input type=\"button\" value=\"����û���\" onclick=\"return chkid()\">\n");
    printf
        ("<tr><td align=right>*����������:<td align=left><input type=password name=pass1 size=12 maxlength=12> (4-12�ַ�)\n");
    printf
        ("<tr><td align=right>*��ȷ������:<td align=left><input type=password name=pass2 size=12 maxlength=12>\n");
    printf
        ("<tr><td align=right>*��������֤��:<td align=left><input type=text name=vcode maxlength=4><img src=\"bbsvcode?file=%x\" />\n	<input type=hidden name=vcodefile value=%x />",
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

    printf("<SCRIPT LANGUAGE=\"JavaScript\">\n");
    printf("function chkid(){\n");
    printf("strname=document.form1.userid.value;\n");
    printf("if (strname == \"\")\n");
    printf("{\n");
    printf("window.alert(\"����д�����û�����\");\n");
    printf("document.form1.userid.focus();\n");
    printf("return (false);\n");
    printf("} \n");
    printf("var url=\"bbschkid?userid=\"+strname\n");
    printf
        ("WriterWindow = window.open(url,\"writerWindow\",\"toolbar=no,width=300,height=150;,directories=no,status=no,scrollbars=no,resize=no,menubar=no\")\n");
    printf("}\n");
    printf("</SCRIPT>\n");
    printf("</html>");
    return 0;
}
