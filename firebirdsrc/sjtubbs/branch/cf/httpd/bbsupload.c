#include "www.h"

int bbsupload_main()
{
    char board[80];
	cache_age = 60*60*24;
    init_all();
    strsncpy(board, getparm("board"), 80);
    printf
    ("<center>%s -- �ϴ�������%s������ <hr color=green>\n",
     BBSNAME, board);
    printf
    ("ע�⣬bbs��������Դ����Ϊ��ʡ�ռ䣬�����ϴ�������ļ��������ϴ�������޹ص��ļ���<br>\n");
    printf("Ŀǰ���������ļ���С����Ϊ2M�ֽ�. <br>\n");
    printf
    ("�����ع��ҷ��ɣ�<font color=red>�Ͻ����طǷ����ϺͿ��ܵ��¾��׵�����</font>��<br>\n");
    printf
    ("<form method=post action=bbsdoupload enctype='multipart/form-data'>\n");
    printf("<table>\n");
    printf("<tr><td>���ظ���: <td><input type=file name=up>");
    printf("<input type=hidden name=MAX_FILE_SIZE value=5000000>");
    printf("<input type=hidden name=board value='%s'>", board);
    printf("<tr><td>�����Ƽ�����: <td>");
    printf("<input type=radio name=level value=0 checked>1�� ");
    printf("<input type=radio name=level value=1>2�� ");
    printf("<input type=radio name=level value=2>3�� ");
    printf("<input type=radio name=level value=3>4�� ");
    printf("<input type=radio name=level value=4>5�� ");
    printf
    ("<tr><td>ϣ��������ʱ��: <td><input name=live size=4 maxlength=4 value=180>(1-9999)��<br>\n");
    printf
    ("<tr><td>�����ļ�Ҫ˵��: <td><input name=exp size=60 maxlength=72>\n");
    printf("</table>\n");
    printf("<hr color=green><br>\n");
    printf("<input type=submit value='��ʼ�ϴ��ļ�'> \n");
    printf
    ("<input type=button onclick='javascript:history.go(-1)' value='������һҳ'> \n");
    printf("</form>");
}