#include "www.h"

int bbsmywww_main()
{
    int t_lines = 20, link_mode = 0, def_mode = 0, font_mode = 0, type;
    int fontn_mode = 1, ansisize_mode = 1, ansifont_mode = 0;

    init_all();
    if (!loginok)
        http_fatal("�Ҵҹ��Ͳ��ܶ��ƽ���");
    printf("<center>%s -- WWW���˶��� [ʹ����: %s]<hr color=green>",
           BBSNAME, currentuser.userid);
    type = atoi(getparm("type"));
    t_lines = atoi(getparm("t_lines"));
    link_mode = atoi(getparm("link_mode"));
    def_mode = atoi(getparm("def_mode"));
    font_mode = atoi(getparm("font_mode"));
    fontn_mode = atoi(getparm("fontn_mode"));
    ansisize_mode = atoi(getparm("ansisize_mode"));
    ansifont_mode = atoi(getparm("ansifont_mode"));
    if (type > 0)
    {
        save_set(t_lines, link_mode, def_mode, font_mode, fontn_mode, ansisize_mode, ansifont_mode);
        http_quit();
    }
    printf("<form action=bbsmywww>\n");
    printf("<table>\n");
    printf("<input type=hidden name=type value=1>");
    printf("<tr><td>һ����ʾ������: &nbsp;");
    printf("<td><input name=t_lines size=8 value=%d> (10-40)\n",
           my_t_lines);
    printf("<tr><td>����ʶ��: ");
    printf("<td><input type=radio name=link_mode value=0 %s>ʶ�� ",
           my_link_mode == 0 ? "checked" : "");
    printf("<input type=radio name=link_mode value=1 %s>��ʶ��\n",
           my_link_mode == 1 ? "checked" : "");
    printf("<tr><td>ȱʡģʽ: ");
    printf("<td><input type=radio name=def_mode value=0 %s>һ�� ",
           my_def_mode == 0 ? "checked" : "");
    printf("<input type=radio name=def_mode value=1 %s>����\n",
           my_def_mode == 1 ? "checked" : "");
    printf("<tr><td>Web ���������С: ");
    printf("<td><input type=radio name=font_mode value=0 %s>С�� ",
           my_font_mode == 0 ? "checked" : "");
    printf("<input type=radio name=font_mode value=1 %s>����\n",
           my_font_mode == 1 ? "checked" : "");
    printf("<tr><td>Web ����ʹ������: ");
    printf("<td><input type=radio name=fontn_mode value=0 %s>���� ",
           my_fontn_mode == 0 ? "checked" : "");
    printf("<input type=radio name=fontn_mode value=1 %s>Verdana\n",
           my_fontn_mode == 1 ? "checked" : "");
    printf("<tr><td>����������ִ�С: ");
    printf("<td><input type=radio name=ansisize_mode value=0 %s>С�� ",
           my_ansisize_mode == 0 ? "checked" : "");
    printf("<input type=radio name=ansisize_mode value=1 %s>����\n",
           my_ansisize_mode == 1 ? "checked" : "");
    printf("<tr><td>�������ʹ������: ");
    printf("<td><input type=radio name=ansifont_mode value=0 %s>���� ",
           my_ansifont_mode == 0 ? "checked" : "");
    printf("<input type=radio name=ansifont_mode value=1 %s>Verdana\n",
           my_ansifont_mode == 1 ? "checked" : "");
    printf("</table>\n");
    printf
    ("<tr><td align=center colspan=2><input type=submit value=ȷ��> <input type=reset value=��ԭ>\n");
    printf("<p> ע�⣺�����������ʹ�����塱��Ϊ Verdana ���ܻ�����Ķ������°�����ҡ�\n");
    printf("</form>\n");
}

int save_set(int t_lines, int link_mode, int def_mode, int font_mode,int fontn_mode, int ansisize_mode, int ansifont_mode)
{
    FILE *fp;
    char path[80];

    sprintf(path, "home/%c/%s/.mywww", toupper(currentuser.userid[0]),
            currentuser.userid);
    if (t_lines < 10 || t_lines > 40)
        http_fatal("���������");
    if (link_mode < 0 || link_mode > 1)
        http_fatal("���������ʶ�����");
    if (def_mode < 0 || def_mode > 1)
        http_fatal("�����ȱʡģʽ");
    if (font_mode < 0 || font_mode > 1)
        http_fatal("�������������");
    if (ansisize_mode < 0 || ansisize_mode > 1)
        http_fatal("�������������");
    fp = fopen(path, "w");
    fprintf(fp, "t_lines %d\n", t_lines);
    fprintf(fp, "link_mode %d\n", link_mode);
    fprintf(fp, "def_mode %d\n", def_mode);
    fprintf(fp, "font_mode %d\n", font_mode);
    fprintf(fp, "fontn_mode %d\n", fontn_mode);
    fprintf(fp, "ansisize_mode %d\n", ansisize_mode);
    fprintf(fp, "ansifont_mode %d\n", ansifont_mode);
    fclose(fp);
    printf("WWW���Ʋ����趨�ɹ�.<br>\n");
    printf("[<a href='javascript:history.go(-2)'>����</a>]");
}
