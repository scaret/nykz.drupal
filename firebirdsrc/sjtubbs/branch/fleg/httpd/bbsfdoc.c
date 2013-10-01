#include "www.h"

int bbsfdoc_main()
{
    FILE *fp;
    char *ptr, board[256];
    struct dir x[20];
    int i, start, total = 0, n,entriesInX,flag=0;
    char *name;
    int length,a;

    init_all();
    strsncpy(board, getparm("board"), 30);
    if (!has_read_perm(&currentuser, board))
        http_fatal("�����������");
    ptr = getparm("start");
    if (ptr[0] == 0)
    {
        start = 0;//0��ʾ���һҳ
    }
    else
    {
        start = atoi(ptr);
        if(start<0)
            start=0;
    }
    fp = fopen(UPLOAD_PATH "/.DIR", "r");
    if (fp == 0)
        http_fatal("û���ҵ��ļ�");
    while(start==0 || total<start)
    {
        n=0;
        while(n<20)
        {
            if (fread(&x[n], sizeof(struct dir), 1, fp) <= 0)
            {
                flag=1;
                break;
            }
            if (!x[n].active)
                continue;
            if (strcasecmp(x[n].board, board))
                continue;
            n++;
        }
        total++;
        if(flag)
        {
            if(n==0)
                total--;
            else
                entriesInX=n;
            break;
        }
        entriesInX=n;
    }


    printf("<center> %s -- ����������� [������: %s]<hr color=green>\n",
           BBSNAME, board);
    printf
    ("<table width=800 border=1><tr><td>���<td>�ļ���<td>������<td>ʱ��<td>��С<td>˵��<td>��ע<td>����\n");
    for (i = 0; i < entriesInX ; i++)
    {
        name=nohtml(x[i].showname);
        length=strlen(name);
        printf("<tr><td>%d", (total-1)*20 + i+1);
        printf("<td><a href=/showfile?name=");
        for(a=0;a<length;a++)
        {
            printf("%%%x",(int)name[a]&0x000000FF);
        }
        printf(" target=_blank>%s </a>",name);
        printf("<td>%s", userid_str(x[i].userid));
        printf("<td>%6.6s", Ctime(x[i].date) + 4);
        printf("<td>%d", x[i].size);
        printf("<td><a href=bbsfexp?name=");
        for(a=0;a<length;a++)
        {
            printf("%%%x",(int)name[a]&0x000000FF);
        }
        printf(">more </a>\n");
        printf("<td width='33%'>%s ",nohtml(x[i].exp));
        printf("<td><a onclick='return confirm(\"ȷ��?\")' href=bbsudel?file=");
        for(a=0;a<length;a++)
        {
            printf("%%%x",(int)name[a]&0x000000FF);
        }
        printf(">ɾ��</a>\n");
    }
    printf("</table><br>\n");
    printf
    ("[<a href=bbsfdoc?board=%s>ˢ��</a>] [<a href=bbsdoc?board=%s>��������</a>] [<a href=bbsupload?board=%s>�����ļ�</a>]",
     board, board, board);
    printf("[<a href=bbsfdoc?board=%s&start=1>��ҳ</a>] ", board);
    if (total > 1)
        printf("[<a href=bbsfdoc?board=%s&start=%d>��һҳ</a>] ", board,
               total-1);
    if (flag==0)
        printf("[<a href=bbsfdoc?board=%s&start=%d>��һҳ</a>] ", board,
               total+1);
    printf("[<a href=bbsfdoc?board=%s>ĩҳ</a>] ", board);
    printf
    ("<form name=form1 action=bbsfdoc?board=%s method=post>\n",
     board);
    printf
    ("<input type=submit value=��ת��> �� <input type=text name=start size=4>ҳ,ÿҳ20�� ");
    printf("</form>\n");
}
