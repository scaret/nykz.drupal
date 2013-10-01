#ifndef WITHOUT_ADMIN_TOOLS
#include <stdio.h>
#include "bbs.h"

extern int cmpbnames ();
extern int numboards;
extern int cleanmail ();
extern char *chgrp ();
extern int dowall ();
extern int cmpuids ();
extern int t_cmpuids ();

extern int noreply;

int showperminfo (int, int);

#ifndef NEW_CREATE_BRD
char cexplain[STRLEN];
#endif
char buf2[STRLEN];
char lookgrp[30];
char bnames[3][20];

//this file, change all + 8 to + 13 by alt@


struct GroupInfo
{
    char name[SectNum][16];
    char items[SectNum][32];
    char chs[SectNum][8];
    time_t update;
    int num;
}
GroupsInfo;

FILE *cleanlog;

int
getbnames (userid, bname, find)
char *userid, *bname;
int *find;
{
    int oldbm = 0;
    FILE *bmfp;
    char bmfilename[STRLEN], tmp[20];

    *find = 0;
    sethomefile (bmfilename, userid, ".bmfile");
    bmfp = fopen (bmfilename, "r");
    if (!bmfp)
        return 0;
    for (oldbm = 0; oldbm < 3;)
    {
        fscanf (bmfp, "%s\n", tmp);
        if (!strcmp (bname, tmp))
            *find = oldbm + 1;
        strcpy (bnames[oldbm++], tmp);
        if (feof (bmfp))
            break;
    }
    fclose (bmfp);
    return oldbm;
}


int
m_info ()
{
    struct userec uinfo;
    int id;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("修改使用者资料");
    if (!gettheuserid (1, "请输入使用者代号: ", &id))
        return -1;
    memcpy (&uinfo, &lookupuser, sizeof (uinfo));

    move (1, 0);
    clrtobot ();
    disply_userinfo (&uinfo);
    uinfo_query (&uinfo, 1, id);
    return 0;
}

int
m_ordainBM ()
{
    int id, pos, oldbm = 0, i, find, bm = 1;
    struct boardheader fh;
    FILE *bmfp;
    char bmfilename[STRLEN], bname[STRLEN];
    char buf[5][STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("任命板主\n");
    clrtoeol ();
    if (!gettheuserid (2, "输入欲任命的使用者帐号: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "guest"))
    {
        move (5, 0);
        prints ("你不能任命 guest 当板主");
        pressanykey ();
        clear ();
        return -1;
    }
    if (!gettheboardname
            (3, "输入该使用者将管理的讨论区名称: ", &pos, &fh, bname))
        return -1;
    if (fh.BM[0] != '\0')
    {
        if (!strncmp (fh.BM, "SYSOP", 5))
        {
            move (4, 0);
            if (askyn ("该讨论区板主是 SYSOP, 你确定该板需要板主吗", NA, NA)
                    == NA)
            {
                clear ();
                return -1;
            }
            fh.BM[0] = '\0';
        }
        else
        {
            for (i = 0, oldbm = 1; fh.BM[i] != '\0'; i++)
                if (fh.BM[i] == ' ')
                    oldbm++;
            if (oldbm == 3)
            {
                move (5, 0);
                prints ("%s 讨论区已有三名板主", bname);
                pressreturn ();
                clear ();
                return -1;
            }
            bm = 0;
        }
    }
    oldbm = getbnames (lookupuser.userid, bname, &find);
    if (find || oldbm == 3)
    {
        move (5, 0);
        prints (" %s 已经是%s板的板主了", lookupuser.userid,
                find ? "该" : "三个");
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\n你将任命 %s 为 %s 板板%s.\n", lookupuser.userid, bname,
            bm ? "主" : "副");
    if (askyn ("你确定要任命吗?", NA, NA) == NA)
    {
        prints ("取消任命板主");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    move (8, 0);
    prints ("请输入任命附言(最多五行，按 Enter 结束)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    strcpy (bnames[oldbm], bname);
    if (!oldbm)
    {
        char secu[STRLEN];

        lookupuser.userlevel |= PERM_BOARDS;
        substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
        sprintf (secu, "板主任命, 给予 %s 的板主权限", lookupuser.userid);
        securityreport (secu);
        move (15, 0);
        prints (secu);
    }
    if (fh.BM[0] == '\0')
        strcpy (genbuf, lookupuser.userid);
    else
        sprintf (genbuf, "%s %s", fh.BM, lookupuser.userid);
    strncpy (fh.BM, genbuf, sizeof (fh.BM));
    sprintf (genbuf, "%-38.38s(BM: %s)", fh.title + 11, fh.BM);
    get_grp (fh.filename);
    edit_grp (fh.filename, lookgrp, fh.title + 11, genbuf);
    substitute_record (BOARDS, &fh, sizeof (fh), pos);
    sethomefile (bmfilename, lookupuser.userid, ".bmfile");
    bmfp = fopen (bmfilename, "w+");
    if (bmfp)
    {
        for (i = 0; i < oldbm + 1; i++)
            fprintf (bmfp, "%s\n", bnames[i]);
        fclose (bmfp);
    }
    sprintf (bmfilename, "任命 %s 为 %s 讨论区%s",
             lookupuser.userid, fh.filename, bm ? "板主" : "板副");
    securityreport (bmfilename);
    move (16, 0);
    prints (bmfilename);
	if(strncmp(bname, "BMTraining", 10))	//not a bmt board
	{
		sprintf (genbuf, "\n\t\t\t【 通告 】\n\n"
             "\t任命 %s 为 %s 板%s！\n"
             "\t请 %s 在三天内在 BoardMaster 板报到。\n"
             "\t逾期未能报到者，将取消板主权限。\n",
             lookupuser.userid, bname, bm ? "板主" : "板副", lookupuser.userid);
	}
	else	//bmt board
	{
		sprintf (genbuf, "\n\t\t\t【 通告 】\n\n"
             "\t任命 %s 为 %s 板%s！\n"
             "\t请 %s 在三天内在 District%c 板报到。\n"
             "\t逾期未能报到者，将取消板主权限。\n",
             lookupuser.userid, bname, bm ? "板主" : "板副", lookupuser.userid, bname[10]);
	}
    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (genbuf, "\n\n站长任命附言：\n");
        strcat (genbuf, buf[i]);
        strcat (genbuf, "\n");
    }
    strcpy (currboard, bname);
#ifdef ORDAINBM_POST_BOARDNAME
	if(strncmp(bname, "BMTraining", 10) == 0)
	{
		autoreport (bmfilename, genbuf, lookupuser.userid, TRAIN_ANNOUNCE_BOARDNAME);
	}
	else
	{
		autoreport (bmfilename, genbuf, lookupuser.userid, ORDAINBM_POST_BOARDNAME);
	}

#else

    autoreport (bmfilename, genbuf, lookupuser.userid, NULL);
#endif

    pressanykey ();
    return 0;
}

int
m_retireBM ()
{
    int id, pos, right = 0, oldbm = 0, i, j, bmnum;
    int find, bm = 1;
    struct boardheader fh;
    FILE *bmfp;
    char bmfilename[STRLEN], buf[5][STRLEN];
    char bname[STRLEN], usernames[3][STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("板主离职\n");
    clrtoeol ();
    if (!gettheuserid (2, "输入欲离职的板主帐号: ", &id))
        return -1;
    if (!gettheboardname (3, "请输入该板主要辞去的板名: ", &pos, &fh, bname))
        return -1;
    oldbm = getbnames (lookupuser.userid, bname, &find);
    if (!oldbm || !find)
    {
        move (5, 0);
        prints (" %s %s板板主，如有错误，请通知程序站长。",
                lookupuser.userid, (oldbm) ? "不是该" : "没有担任任何");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = find - 1; i < oldbm; i++)
    {
        if (i != oldbm - 1)
            strcpy (bnames[i], bnames[i + 1]);
    }
    bmnum = 0;
    for (i = 0, j = 0; fh.BM[i] != '\0'; i++)
    {
        if (fh.BM[i] == ' ')
        {
            usernames[bmnum][j] = '\0';
            bmnum++;
            j = 0;
        }
        else
        {
            usernames[bmnum][j++] = fh.BM[i];
        }
    }
    usernames[bmnum++][j] = '\0';
    for (i = 0, right = 0; i < bmnum; i++)
    {
        if (!strcmp (usernames[i], lookupuser.userid))
        {
            right = 1;
            if (i)
                bm = 0;
        }
        if (right && i != bmnum - 1)
            strcpy (usernames[i], usernames[i + 1]);
    }
    if (!right)
    {
        move (5, 0);
        prints
        ("对不起， %s 板板主名单中没有 %s ，如有错误，请通知程序站长。",
         bname, lookupuser.userid);
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\n你将取消 %s 的 %s 板板%s职务.\n",
            lookupuser.userid, bname, bm ? "主" : "副");
    if (askyn ("你确定要取消他的该板板主职务吗?", NA, NA) == NA)
    {
        prints ("\n呵呵，你改变心意了？ %s 继续留任 %s 板板主职务！",
                lookupuser.userid, bname);
        pressanykey ();
        clear ();
        return -1;
    }
    if (bmnum - 1)
    {
        sprintf (genbuf, "%s", usernames[0]);
        for (i = 1; i < bmnum - 1; i++)
            sprintf (genbuf, "%s %s", genbuf, usernames[i]);
    }
    else
        genbuf[0] = '\0';
    strncpy (fh.BM, genbuf, sizeof (fh.BM));
    if (fh.BM[0] != '\0')
        //next 5 line, change 8 to 13 by alt@
        sprintf (genbuf, "%-38.38s(BM: %s)", fh.title + 11, fh.BM);
    else
        sprintf (genbuf, "%-38.38s", fh.title + 11);
    get_grp (fh.filename);
    edit_grp (fh.filename, lookgrp, fh.title + 11, genbuf);
    substitute_record (BOARDS, &fh, sizeof (fh), pos);
    sprintf (genbuf, "取消 %s 的 %s 讨论区板主职务", lookupuser.userid,
             fh.filename);
    securityreport (genbuf);
    move (8, 0);
    prints (genbuf);
    sethomefile (bmfilename, lookupuser.userid, ".bmfile");
    if (oldbm - 1)
    {
        bmfp = fopen (bmfilename, "w+");
        if (bmfp)
        {
            for (i = 0; i < oldbm - 1; i++)
                fprintf (bmfp, "%s\n", bnames[i]);
            fclose (bmfp);
        }
    }
    else
    {
        char secu[STRLEN];

        unlink (bmfilename);
        if (!(lookupuser.userlevel & PERM_OBOARDS)
                && !(lookupuser.userlevel & PERM_SYSOP))
        {
            lookupuser.userlevel &= ~PERM_BOARDS;
            substitute_record (PASSFILE, &lookupuser, sizeof (struct userec),
                               id);
            sprintf (secu, "板主卸职, 取消 %s 的板主权限", lookupuser.userid);
            securityreport (secu);
            move (9, 0);
            prints (secu);
        }
    }
    prints ("\n\n");
    if (askyn ("需要在相关板面发送通告吗?", YEA, NA) == NA)
    {
        pressanykey ();
        return 0;
    }
    prints ("\n");
    if (askyn ("正常离任请按 Enter 键确认，撤职惩罚按 N 键", YEA, NA) == YEA)
        right = 1;
    else
        right = 0;
    if (right)
        sprintf (bmfilename, "%s 板%s %s 离任通告", bname,
                 bm ? "板主" : "板副", lookupuser.userid);
    else
        sprintf (bmfilename, "[通告]撤除 %s 板%s %s ", bname,
                 bm ? "板主" : "板副", lookupuser.userid);
    strcpy (currboard, bname);
    if (right)
    {
        sprintf (genbuf, "\n\t\t\t【 通告 】\n\n"
                 "\t经站务组讨论：\n"
                 "\t同意 %s 辞去 %s 板的%s职务。\n"
                 "\t在此，对他曾经在 %s 板的辛苦劳作表示感谢。\n\n"
                 "\t希望今后也能支持本板的工作.",
                 lookupuser.userid, bname, bm ? "板主" : "板副", bname);
    }
    else
    {
        sprintf (genbuf, "\n\t\t\t【撤职通告】\n\n"
                 "\t经站务组讨论决定：\n"
                 "\t撤除 %s 板%s %s 的%s职务。\n",
                 bname, bm ? "板主" : "板副", lookupuser.userid,
                 bm ? "板主" : "板副");
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    move (14, 0);
    prints ("请输入%s附言(最多五行，按 Enter 结束)",
            right ? "板主离任" : "板主撤职");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 15, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (genbuf, right ? "\n\n离任附言：\n" : "\n\n撤职说明：\n");
        strcat (genbuf, buf[i]);
        strcat (genbuf, "\n");
    }
#ifdef ORDAINBM_POST_BOARDNAME
	if(strncmp(bname, "BMTraining", 10) == 0)
	{
		autoreport (bmfilename, genbuf, NULL, TRAIN_ANNOUNCE_BOARDNAME);
	}
	else
	{
		autoreport (bmfilename, genbuf, NULL, ORDAINBM_POST_BOARDNAME);
	}
#else

    autoreport (bmfilename, genbuf, NULL, NULL);
#endif

    prints ("\n执行完毕！");
    pressanykey ();
    return 0;
}

#ifdef NEW_CREATE_BRD
int
getGroupSet (void)
{
    FILE *mfp;
    char buf[256];
    char *ptr;
    int i = 0, groupNum, j = 0, k = 0;
    struct stat st;

    if (stat ("etc/menu.ini", &st) == -1 || st.st_size == 0)
    {
        GroupsInfo.update = 0;
        return 0;
    }
    if (GroupsInfo.update >= st.st_mtime)
    {
        return 1;
    }
    GroupsInfo.update = st.st_mtime;
    mfp = fopen ("etc/menu.ini", "r");
    if (mfp == NULL)
    {
        GroupsInfo.update = 0;
        return 0;
    }
    for (i = 0; i < SectNum; i++)
    {
        for (j = 0; j < 16; j++)
            GroupsInfo.name[i][j] = '\0';
        for (j = 0; j < 32; j++)
            GroupsInfo.items[i][j] = '\0';
        for (j = 0; j < 8; j++)
            GroupsInfo.chs[i][j] = '\0';
    }
    j = 0;
    while (fgets (buf, 256, mfp) != NULL)
    {
        if (j >= SectNum && k >= SectNum)
            break;
        my_ansi_filter (buf);
        if (buf[0] == '#')
            continue;
        if (strstr (buf, "EGroups"))
        {
            if (buf[0] != '@')
                continue;
            ptr = strchr (buf, '\"');
            if (ptr == NULL)
                continue;
            while (ptr++)
            {
                if (*ptr == '\"')
                    break;
                //add by alt 4 字母分区
                if (*ptr >= 'a' && *ptr <= 'z')
                    *ptr -= 32;	//小写统一到大写
                if (*ptr >= 'A' && *ptr <= 'Z')
                    *ptr -= 7;	//字母统一到数字
                groupNum = *ptr - '0';
                if (groupNum < SectNum && groupNum >= 0)
                    break;
            }
            if (*ptr == '\"' || *ptr == '\0')
                continue;
            strcpy (buf, ptr);
            ptr = strchr (buf, ')');
            if (ptr == NULL)
                continue;
            for (i = 1; *(ptr + i) == ' ' && *(ptr + i) != '\0'; i++)
                ;
            ptr += i;
            for (i = 0;
                    *(ptr + i) != ' ' && *(ptr + i) != '\0'
                    && *(ptr + i) != '\"' && i < 16; i++)
            {
                GroupsInfo.name[groupNum][i] = *(ptr + i);
            }
            GroupsInfo.name[groupNum][i] = '\0';
            ptr += i;
            if (*ptr != '\0' && *ptr != '\n')
            {
                for (i = 1; *(ptr + i) == ' ' && *(ptr + i) != '\0'; i++)
                    ;
                ptr += i;
                if (*ptr != '\0' && *ptr != '\n')
                {
                    for (i = 0;
                            *(ptr + i) != '\"' && *(ptr + i) != ' '
                            && *(ptr + i) != '\0' && i < 32; i++)
                    {
                        GroupsInfo.items[groupNum][i] = *(ptr + i);
                    }
                    GroupsInfo.items[groupNum][i] = '\0';
                }
                else
                    GroupsInfo.items[groupNum][0] = '\0';
            }
            else
                GroupsInfo.items[groupNum][0] = '\0';
            j++;
            continue;
        }
        if (strstr (buf, "EGROUP"))
        {
            if (buf[0] != 'E')
                continue;
            if (buf[6] >= 'a' && buf[6] <= 'z')
                buf[6] -= 32;	//小写统一到大写
            if (buf[6] >= 'A' && buf[6] <= 'Z')
                buf[6] -= 7;	//字母统一到数字
            groupNum = buf[6] - '0';
            if (groupNum >= SectNum || groupNum < 0)
                continue;
            ptr = strchr (buf, '\"');
            if (ptr == NULL)
                continue;
            ptr++;
            for (i = 0;
                    *(ptr + i) != '\"' && *(ptr + i) != ' '
                    && *(ptr + i) != '\0' && i < 8; i++)
            {
                GroupsInfo.chs[groupNum][i] = *(ptr + i);
            }
            GroupsInfo.chs[groupNum][i] = '\0';
            k++;
        }
    }
    fclose (mfp);
    if (j != k)
    {
        GroupsInfo.update = 0;
        return 0;
    }

    GroupsInfo.num = j;
    return 1;
}

int
chkGroupsInfo ()
{
    int haveError = NA, i;

    clear ();
    if (getGroupSet () == 0)
        haveError = YEA;
    else
    {
        for (i = 0; i < SectNum; i++)
            if ((GroupsInfo.name[i][0] == '\0') ^ (GroupsInfo.chs[i][0] == '\0'))
                haveError = YEA;
    }
    if (haveError != YEA)
        return 1;
    prints ("\n【\033[0;1;4;33m注意\033[m】系统发现 menu.ini 的配置"
            "可能存在问题，建议检查后再继续进行！\n        ");
    prints ("如果不明白该提示的含义，请拷贝本屏信息到灌水站报告问题！\n\n");
    prints ("本站目前的讨论区分类情况如下：\n\n");
    prints (" 分区   分区名称     分区类别描述             类别代码\n");
    prints ("-------------------------------------------------------\n");
    for (i = 0; i < SectNum; i++)
    {
        if (GroupsInfo.name[i][0] == '\0' && GroupsInfo.chs[i][0] == '\0')
            continue;
        prints ("GROUP_%d %-12s %-24s \"%s\"\n", i,
                GroupsInfo.name[i][0] ==
                '\0' ? "######" : GroupsInfo.name[i],
                GroupsInfo.items[i][0] ==
                '\0' ? "######" : GroupsInfo.items[i],
                GroupsInfo.chs[i][0] == '\0' ? "######" : GroupsInfo.chs[i]);
    }
    prints ("\n\n");
    if (askyn
            ("上面的信息表明 menu.ini 可能有误，您仍然想继续执行吗", NA, NA) == YEA)
        return 1;
    return 0;
}

void ShowBrdSet (struct boardheader fh);

int
ChangeTheBoard (struct boardheader *oldfh, int pos)
{
    struct boardheader newfh;
    char buf[STRLEN], title[STRLEN], vbuf[100], ch;
    char num[2], chs[2], items[9], nntp[3], group[16];
    int i, j, a_mv;

    if (oldfh)
        memcpy (&newfh, oldfh, sizeof (newfh));
    else
        memset (&newfh, 0, sizeof (newfh));
    move (2, 0);
    if (oldfh)
    {
        prints
        ("您将重新设定 [\033[32m%s\033[m] 的板面属性 [Enter-->设定不变]",
         oldfh->filename);
    }
    else
        prints ("您将开始创建一个\033[32m新\033[m的讨论区 [ENTER-->取消创建]");
    while (1)
    {
        struct boardheader dh;

        getdata (3, 0, "讨论区名称(英文名): ", buf, BDNAME_LEN, DOECHO, YEA);
        if (buf[0] == '\0')
        {
            if (oldfh)
                strcpy (newfh.filename, oldfh->filename);
            else
                return -1;
            break;
        }
        if (killwordsp (buf) == 0)
            continue;
        if (!valid_brdname (buf))
            continue;
        if (search_record (BOARDS, &dh, sizeof (dh), cmpbnames, buf))
        {
            prints ("\n错误! 此讨论区已经存在!!");
            if (oldfh)
                continue;
            pressanykey ();
            return -1;
        }
        strcpy (newfh.filename, buf);
        break;
    }
    prints ("\n[讨论区说明]分区代码 + 分区描述 + 转信标志 + 讨论区标题\n");
    if (oldfh)
    {
        prints ("初始设定：%11s\033[32m%s\033[m\n", oldfh->title,
                oldfh->title + 11);
    }
    else
        prints ("格式范例：a[本站] ○ \033[32m这是一个站内讨论区\033[m\n");
    while (1)
    {
        if (oldfh)
            strcpy (buf, oldfh->title + 11);
        else
            buf[0] = '\0';
        getdata (8, 0, "讨论区标题(中文名): ", buf, 41, DOECHO, NA);
        if (buf[0] == '\0')
        {
            if (oldfh == NULL)
                return -1;
            continue;
        }
        if (killwordsp (buf) == 0)
            continue;
        strcpy (title, buf);
        break;
    }
    if (askyn ("该讨论区需要参与转信吗", NA, NA) == YEA)
    {
        strcpy (nntp, "●");
        newfh.flag |= OUT_FLAG;
    }
    else
    {
        strcpy (nntp, "○");
        newfh.flag &= ~OUT_FLAG;
    }
    if (oldfh)
    {
        ch = oldfh->title[0];
        for (i = 0; i < SectNum; i++)
        {
            if (GroupsInfo.chs[i][0] != '\0' && strchr (GroupsInfo.chs[i], ch))
                break;
        }
        if (i >= SectNum)
            ch = '0';
        else
            ch = '0' + i;
    }
    else
        ch = '0';
    for (i = 0, j = 3; i < SectNum; i++)
    {
        if (GroupsInfo.name[i][0] == '\0')
            continue;
        move (j++, 60);
        prints ("┃%s(%d)%-12s\033[m┃",
                (i + '0' == ch) ? "\033[32m" : "", i, GroupsInfo.name[i]);
    }
    while (1)
    {
        if (ch > '0' + 9)
            ch += 7;		//数字变字母
        num[0] = ch;
        num[1] = '\0';
        getdata (11, 0, "该讨论区放置在哪一分区(参考右边提示编号)？: ",
                 num, 2, DOECHO, NA);
        if (num[0] >= 'a' && num[0] <= 'z')
            num[0] -= 32;		//小写统一到大写
        if (num[0] >= 'A' && num[0] <= 'Z')
            num[0] -= 7;		//字母统一到数字
        if (num[0] == '\0' || num[0] < '0' || num[0] >= ('0' + SectNum))
            continue;
        if (GroupsInfo.name[num[0] - '0'][0] == '\0')
            continue;
        break;
    }
    prints ("\n第 %d 区的分类符号参考：[\033[32m%s\033[m] 分类描述参考：%s",
            num[0] - '0', GroupsInfo.chs[num[0] - '0'],
            GroupsInfo.items[num[0] - '0']);
    while (1)
    {
        if (oldfh == NULL
                || !strchr (GroupsInfo.chs[num[0] - '0'], oldfh->title[0]))
            chs[0] = GroupsInfo.chs[num[0] - '0'][0];
        else
            chs[0] = oldfh->title[0];
        chs[1] = '\0';
        getdata (14, 0, "请输入该讨论区的分类符号: ", chs, 2, DOECHO, NA);
        if (chs[0] == '\0')
            continue;
        if (strchr (GroupsInfo.chs[num[0] - '0'], chs[0]))
            break;
    }
    move (12, 0);
    prints ("\n第 %d 区的分类符号参考：[%s] 分类描述参考：\033[32m%s\033[m",
            num[0] - '0', GroupsInfo.chs[num[0] - '0'],
            GroupsInfo.items[num[0] - '0']);
    while (1)
    {
        if (oldfh)
        {
            strncpy (buf, oldfh->title + 1, 7);
            buf[6] = '\0';
        }
        else
            buf[0] = '\0';
        getdata (15, 0, "请输入该讨论区的分类描述: ", buf, 7, DOECHO, NA);
        if (buf[0] == '\0')
            continue;
        if (killwordsp (buf) == 0)
            continue;
        strcpy (items, buf);
        break;
    }
    sprintf (newfh.title, "%c%-6s %s %s", chs[0], items, nntp, title);
    if (oldfh == NULL || oldfh->BM[0] == '\0' || oldfh->BM[0] == ' '
            || !strncmp (newfh.BM, "SYSOP", 5))
    {
        prints
        ("\n板主设置  [ 如果不需要板主，则系统自动设置为由 SYSOP 管理]\n");
        if (askyn ("本板需要板主吗", YEA, NA) == NA)
            strcpy (newfh.BM, "SYSOP");
        else
            newfh.BM[0] = '\0';
    }
    else
    {
        prints ("\n板主设置  [ 目前板面的板主是: %s ]\n", oldfh->BM);
        if (askyn
                ("当出现板主名单错误的时候，才需要修改此处。需要修改板主名单吗",
                 NA, NA) == YEA)
        {
            prints ("【\033[31m警告\033[m】修改板主仅供出错修正使用，"
                    "板主任免请勿改动此处！\n");
            strcpy (newfh.BM, oldfh->BM);
            getdata (20, 0, "板主名单: ", newfh.BM, sizeof (newfh.BM),
                     DOECHO, NA);
            if (newfh.BM[0] == ' ')
                newfh.BM[0] = '\0';
        }
        else
            strcpy (newfh.BM, oldfh->BM);
    }
    clear ();
    if (askyn ("本讨论区需要设置 READ 或 POST 限制吗", NA, NA) == YEA)
    {
        char ans[4];

        sprintf (buf, "限制 (R)阅读 或 (P)张贴 文章 [%c]: ",
                 oldfh ? (newfh.level & PERM_POSTMASK ? 'P' : 'R') : 'P');
        getdata (3, 0, buf, ans, 2, DOECHO, YEA);
        if (ans[0] == '\0')
        {
            ans[0] = oldfh ? (newfh.level & PERM_POSTMASK ? 'P' : 'R') : 'P';
            ans[1] = '\0';
        }
        if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
            newfh.level &= ~PERM_POSTMASK;
        else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
            newfh.level |= PERM_POSTMASK;
        clear ();
        move (2, 0);
        prints ("设定 %s '%s' 讨论区的权限\n",
                newfh.level & PERM_POSTMASK ? "张贴" : "阅读", newfh.filename);
        newfh.level = setperms (newfh.level, "权限", NUMPERMS, showperminfo);
        clear ();
    }
    move (1, 0);
	//modified by littlesword@SJTUbbs,positive_anony
    if (askyn
            ("设置本板为匿名板面吗", newfh.flag & ANONY_FLAG ? YEA : NA, NA) == YEA)
		if (askyn ("设置本板匿名发文为默认吗", newfh.flag & ANONY_FLAG ? YEA : NA , NA) == YEA)
			newfh.flag |= ANONY_FLAG;
		else
			newfh.flag |= POSITIVE_ANONY_FLAG;
    else
	{
        newfh.flag &= ~ANONY_FLAG;
		newfh.flag &= ~POSITIVE_ANONY_FLAG;
	}
    move (2, 0);
    if (askyn ("设置本板为不可 Re 模式板面吗",
               (newfh.flag & NOREPLY_FLAG) ? YEA : NA, NA) == YEA)
        newfh.flag |= NOREPLY_FLAG;
    else
        newfh.flag &= ~NOREPLY_FLAG;
    move (3, 0);
    if (oldfh != NULL)
    {
        //      if(askyn("是否移动精华区的位置",NA,NA)==YEA) a_mv = 2;
        //      else a_mv = 0;
        a_mv = 2;
    }
    else
    {
        if (askyn ("本板需要设立精华区吗", YEA, NA) == YEA)
            a_mv = 1;
        else
            a_mv = 0;
    }
    if (a_mv != 0)
    {
        ch = num[0];
        for (i = 0, j = 1; i < SectNum; i++)
        {
            if (GroupsInfo.name[i][0] == '\0')
                continue;
            move (j++, 50);
            prints ("┃%s(%d)%s GROUP_%d\033[m",
                    (i + '0' == ch) ? "\033[32m" : "", i,
                    GroupsInfo.name[i], i);
        }
        while (1)
        {
            strcpy (chs, num);
            if (*chs > '0' + 9)
                *chs += 7;		//数字分到字母
            getdata (j + 1, 48, "精华区分布如上，请选择: ", chs, 2, DOECHO, NA);
            if (chs[0] >= 'a' && chs[0] <= 'z')
                chs[0] -= 32;	//小写统一到大写
            if (chs[0] >= 'A' && chs[0] <= 'Z')
                chs[0] -= 7;	//字母统一到数字
            if (chs[0] == '\0' || chs[0] < '0' || chs[0] >= '0' + SectNum)
                continue;
            if (GroupsInfo.name[chs[0] - '0'][0] == '\0')
                continue;
            break;
        }
    }
    ShowBrdSet (newfh);
    if (oldfh)
    {
        if (askyn ("您确认要更改吗", NA, NA) == NA)
            return -1;
        i = pos;
    }
    else
        i = getbnum ("");
    if (chs[0] > '0' + 9)
        chs[0] += 7;		//数字分离到字母
    sprintf (group, "GROUP_%c", chs[0]);
    if (oldfh == NULL)
    {
        strcpy (vbuf, "vote/");
        strcat (vbuf, newfh.filename);
        setbpath (genbuf, newfh.filename);
        if ((!dashd (genbuf) && mkdir (genbuf, 0755) == -1)
                || (!dashd (vbuf) && mkdir (vbuf, 0755) == -1))
        {
            prints ("\n初始化讨论区目录时出错!\n");
            pressreturn ();
            clear ();
            return -1;
        }
        if (a_mv != 0)
        {
            if (newfh.BM[0] != '\0')
                sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11, newfh.BM);
            else
                sprintf (vbuf, "%-38.38s", newfh.title + 11);
            if (chs[0] >= 'A' && chs[0] <= 'Z')
                chs[0] -= 7;	//字母统一到数字
            if (add_grp
                    (group, GroupsInfo.name[chs[0] - '0'], newfh.filename,
                     vbuf) == -1)
                prints ("\n成立精华区失败....\n");
            else
                prints ("已经置入精华区...\n");
        }
    }
    else
    {				// 修改讨论区
        char tmp_grp[30];

        if (strcmp (oldfh->filename, newfh.filename))
        {
            char old[256], tar[256];

            a_mv = 1;
            setbpath (old, oldfh->filename);
            setbpath (tar, newfh.filename);
            rename (old, tar);
            sprintf (old, "vote/%s", oldfh->filename);
            sprintf (tar, "vote/%s", newfh.filename);
            rename (old, tar);
        }
        if (newfh.BM[0] != '\0')
            sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11, newfh.BM);
        else
            sprintf (vbuf, "%-38.38s", newfh.title + 11);
        get_grp (oldfh->filename);
        edit_grp (oldfh->filename, lookgrp, oldfh->title + 11, vbuf);
        if (a_mv)
        {
            get_grp (oldfh->filename);
            strcpy (tmp_grp, lookgrp);
            if (strcmp (tmp_grp, group) || a_mv == 1)
            {
                char tmpbuf[160], oldpath[STRLEN], newpath[STRLEN];

                sprintf (tmpbuf, "%s:", oldfh->filename);
                del_from_file ("0Announce/.Search", tmpbuf);
                if (newfh.BM[0] != '\0')
                    sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11,
                             newfh.BM);
                else
                    sprintf (vbuf, "%-38.38s", newfh.title + 11);
                if (chs[0] >= 'A' && chs[0] <= 'Z')
                    chs[0] -= 7;	//字母统一到数字
                if (add_grp (group, GroupsInfo.name[chs[0] - '0'],
                             newfh.filename, vbuf) == -1)
                    prints ("\n成立精华区失败....\n");
                else
                    prints ("已经置入精华区...\n");
                sprintf (newpath, "0Announce/groups/%s/%s", group,
                         newfh.filename);
                sprintf (oldpath, "0Announce/groups/%s/%s", tmp_grp,
                         oldfh->filename);
                if (dashd (oldpath))
                {
                    deltree (newpath);
                }
                rename (oldpath, newpath);
                del_grp (tmp_grp, oldfh->filename, oldfh->title + 11);
            }
        }
    }
    if (i > 0)
    {
        substitute_record (BOARDS, &newfh, sizeof (newfh), i);
        if (oldfh)
        {
            sprintf (genbuf, "更改讨论区 %s 的资料 --> %s",
                     oldfh->filename, newfh.filename);
            report (genbuf);
        }
    }
    else if (append_record (BOARDS, &newfh, sizeof (newfh)) == -1)
    {
        prints ("\n成立新板失败！\n");
        pressanykey ();
        clear ();
        return -1;
    }
    numboards = -1;
    {
        char secu[STRLEN];

        if (oldfh)
            sprintf (secu, "修改讨论区：%s(%s)", oldfh->filename, newfh.filename);
        else
            sprintf (secu, "成立新板：%s", newfh.filename);
        securityreport (secu);
    }
    pressanykey ();
    clear ();
}

int
m_newbrd ()
{
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return -1;
    if (chkGroupsInfo () == 0)
        return -1;
    clear ();
    stand_title ("开启新讨论区");
    ChangeTheBoard (NULL, 0);
    return 0;
}

void
ShowBrdSet (struct boardheader fh)
{
    int i = 4;

    move (i, 0);
    prints ("讨论区 [%s] 基本情况：", fh.filename);
    i += 2;
    move (i++, 0);
    prints ("项目名称           项目属性");
    move (i++, 0);
    prints ("------------------------------");
    move (i++, 0);
    prints ("讨论区名称       : %s", fh.filename);
    move (i++, 0);
    prints ("讨论区管理员     : %s", fh.BM[0] == '\0' ? "尚无板主" : fh.BM);
    move (i++, 0);
    prints ("是否为可匿名讨论区 : %s",
            ((fh.flag & ANONY_FLAG) || (fh.flag & POSITIVE_ANONY_FLAG)) ? "匿名" : "非匿名");
    move (i++, 0);
    prints ("文章是否可以回复 : %s",
            (fh.flag & NOREPLY_FLAG) ? "不可 Re" : "可以");
    move (i++, 0);
    prints ("READ/POST 限制   : ");
    if (fh.level & ~PERM_POSTMASK)
    {
        prints ("%s", (fh.level & PERM_POSTMASK) ? "POST" :
                (fh.level & PERM_NOZAP) ? "ZAP" : "READ");
    }
    else
        prints ("无限制");
    move (i++, 0);
    prints ("本讨论区说明     : %s\n\n", fh.title);
}

int
m_editbrd ()
{
    int pos;
    struct boardheader fh;
    char bname[STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return -1;
    if (chkGroupsInfo () == 0)
        return -1;
    clear ();
    stand_title ("修改讨论区基本属性");
    if (!gettheboardname (2, "输入讨论区名称: ", &pos, &fh, bname))
        return -1;
    ShowBrdSet (fh);
    move (15, 0);
    if (askyn ("是否需要更改以上属性", NA, NA) == YEA)
    {
        clear ();
        stand_title ("修改讨论区基本属性");
        ChangeTheBoard (&fh, pos);
    }
    return 0;
}
#else
int
m_newbrd ()
{
    struct boardheader newboard;
    extern int numboards;
    char ans[4];
    char vbuf[100];
    char *group;
    int bid;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;
    clear ();
    stand_title ("开启新讨论区");
    memset (&newboard, 0, sizeof (newboard));
    move (2, 0);
    ansimore2 ("etc/boardref", NA, 3, 7);
    while (1)
    {
        getdata (10, 0, "讨论区名称:   ", newboard.filename, BDNAME_LEN,
                 DOECHO, YEA);
        if (newboard.filename[0] != 0)
        {
            struct boardheader dh;

            if (search_record
                    (BOARDS, &dh, sizeof (dh), cmpbnames, newboard.filename))
            {
                prints ("\n错误! 此讨论区已经存在!!");
                pressanykey ();
                return -1;
            }
        }
        else
            return -1;
        if (valid_brdname (newboard.filename))
            break;
        prints ("\n不合法名称!!");
    }
    newboard.flag = 0;
    while (1)
    {
        getdata (11, 0, "讨论区说明:   ", newboard.title, 60, DOECHO, YEA);
        if (newboard.title[0] == '\0')
            return -1;
        if (strstr (newboard.title, "●") || strstr (newboard.title, "⊙"))
        {
            newboard.flag |= OUT_FLAG;
            break;
        }
        else if (strstr (newboard.title, "○"))
        {
            newboard.flag &= ~OUT_FLAG;
            break;
        }
        else
            prints ("错误的格式, 无法判断是否转信!!");
    }
    strcpy (vbuf, "vote/");
    strcat (vbuf, newboard.filename);
    setbpath (genbuf, newboard.filename);
    if (getbnum (newboard.filename) > 0 || mkdir (genbuf, 0755) == -1
            || mkdir (vbuf, 0755) == -1)
    {
        prints ("\n错误的讨论区名称!!\n");
        pressreturn ();
        clear ();
        return -1;
    }
    move (12, 0);
    if (askyn ("本板诚征板主吗(否则由 SYSOP 管理)?", YEA, NA) == NA)
        strcpy (newboard.BM, "SYSOP");
    else
        newboard.BM[0] = '\0';
    move (13, 0);
    if (askyn ("是否限制存取权力", NA, NA) == YEA)
    {
        getdata (14, 0, "限制 Read/Post? [R]: ", ans, 2, DOECHO, YEA);
        if (*ans == 'P' || *ans == 'p')
            newboard.level = PERM_POSTMASK;
        else
            newboard.level = 0;
        move (1, 0);
        clrtobot ();
        move (2, 0);
        prints ("设定 %s 权力. 讨论区: '%s'\n",
                (newboard.level & PERM_POSTMASK ? "POST" : "READ"),
                newboard.filename);
        newboard.level =
            setperms (newboard.level, "权限", NUMPERMS, showperminfo);
        clear ();
    }
    else
        newboard.level = 0;
    move (14, 0);
    if (askyn ("是否加入匿名板", NA, NA) == YEA)
		if (askyn ("是否设置匿名为默认", NA, NA) == YEA)
			newboard.flag |= ANONY_FLAG;
		else
			newboard.flag |= POSITIVE_ANONY_FLAG;
    else
	{
        newboard.flag &= ~ANONY_FLAG;
		newboard.flag &= ~POSITIVE_ANONY_FLAG;
	}
    if (askyn ("该板的全部文章均不可以回复", NA, NA) == YEA)
        newboard.flag |= NOREPLY_FLAG;
    else
        newboard.flag &= ~NOREPLY_FLAG;
    if ((bid = getbnum ("")) > 0)
    {
        substitute_record (BOARDS, &newboard, sizeof (newboard), bid);
    }
    else if (append_record (BOARDS, &newboard, sizeof (newboard)) == -1)
    {
        pressreturn ();
        clear ();
        return -1;
    }
    group = chgrp ();
    if (group != NULL)
    {
        if (newboard.BM[0] != '\0')
            sprintf (vbuf, "%-38.38s(BM: %s)", newboard.title + 11, newboard.BM);
        else
            sprintf (vbuf, "%-38.38s", newboard.title + 11);
        if (add_grp (group, cexplain, newboard.filename, vbuf) == -1)
            prints ("\n成立精华区失败....\n");
        else
            prints ("已经置入精华区...\n");
    }
    numboards = -1;
    prints ("\n新讨论区成立\n");
    {
        char secu[STRLEN];

        sprintf (secu, "成立新板：%s", newboard.filename);
        securityreport (secu);
    }
    pressreturn ();
    clear ();
    return 0;
}

int
m_editbrd ()
{
    char bname[STRLEN], buf[STRLEN], oldtitle[STRLEN], vbuf[256], *group;
    char oldpath[STRLEN], newpath[STRLEN], tmp_grp[30];
    int pos, noidboard, a_mv;
    struct boardheader fh, newfh;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;
    clear ();
    stand_title ("修改讨论区资讯");
    if (!gettheboardname (2, "输入讨论区名称: ", &pos, &fh, bname))
        return -1;
    noidboard = (fh.flag & ANONY_FLAG) || (fh.flag & POSITIVE_ANONY_FLAG);
    noreply = fh.flag & NOREPLY_FLAG;
    move (3, 0);
    memcpy (&newfh, &fh, sizeof (newfh));
    prints ("讨论区名称:     %s\n", fh.filename);
    prints ("讨论区说明:     %s\n", fh.title);
    prints ("讨论区管理员:   %s\n", fh.BM);
    prints ("可匿名讨论区:   %s\n", (noidboard) ? "Yes" : "No");
    prints ("文章不可以回复: %s\n", (noreply) ? "Yes" : "No");
    strcpy (oldtitle, fh.title);
    prints ("限制 %s 权力: %s", (fh.level & PERM_POSTMASK) ? "POST" :
            (fh.level & PERM_NOZAP) ? "ZAP" : "READ",
            (fh.level & ~PERM_POSTMASK) == 0 ? "不设限" : "有设限");
    move (8, 0);
    if (askyn ("是否更改以上资讯", NA, NA) == NA)
    {
        clear ();
        return 0;
    }
    move (9, 0);
    prints ("直接按 <Return> 不修改此栏资讯...");
    while (1)
    {
        getdata (10, 0, "新讨论区名称: ", genbuf, BDNAME_LEN, DOECHO, YEA);
        if (*genbuf != 0)
        {
            struct boardheader dh;

            if (search_record (BOARDS, &dh, sizeof (dh), cmpbnames, genbuf))
            {
                move (2, 0);
                prints ("错误! 此讨论区已经存在!!");
                move (10, 0);
                clrtoeol ();
                continue;
            }
            if (valid_brdname (genbuf))
            {
                strncpy (newfh.filename, genbuf, sizeof (newfh.filename));
                strcpy (bname, genbuf);
                break;
            }
            else
            {
                move (2, 0);
                prints ("不合法的讨论区名称!");
                move (10, 0);
                clrtoeol ();
                continue;
            }
        }
        else
            break;
    }
    ansimore2 ("etc/boardref", NA, 11, 7);
    while (1)
    {
        getdata (18, 0, "新讨论区说明: ", genbuf, 60, DOECHO, YEA);
        if (*genbuf != 0)
            strncpy (newfh.title, genbuf, sizeof (newfh.title));
        else
            break;
        if (strstr (newfh.title, "●") || strstr (newfh.title, "⊙"))
        {
            newfh.flag |= OUT_FLAG;
            break;
        }
        else if (strstr (newfh.title, "○"))
        {
            newfh.flag &= ~OUT_FLAG;
            break;
        }
        else
            prints ("\n错误的格式, 无法判断是否转信!!");
    }
    move (20, 0);
    clrtoeol ();
    move (19, 0);
    if (fh.BM[0] != '\0' && strcmp (fh.BM, "SYSOP"))
    {
        if (askyn ("修改讨论区管理员。注意：确有错误才可修改。不修改按回车",
                   NA, NA) == YEA)
        {
            getdata (19, 0,
                     "讨论区管理员(【注意】板主任免请勿改动此处！): ",
                     newfh.BM, sizeof (newfh.BM), DOECHO, YEA);
            if (newfh.BM[0] == '\0')
                strcpy (newfh.BM, fh.BM);
            else if (newfh.BM[0] == ' ')
                newfh.BM[0] = '\0';
            else
                ;
        }
    }
    else
    {
        if (askyn ("本板诚征板主吗(否，则由 SYSOP 管理)?", YEA, NA) == NA)
            strncpy (newfh.BM, "SYSOP", sizeof (newfh.BM));
        else
            strncpy (newfh.BM, "\0", sizeof (newfh.BM));
    }
    sprintf (buf, "匿名板（默认匿名） (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
    getdata (20, 0, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= ANONY_FLAG;
        else
            newfh.flag &= ~ANONY_FLAG;
    }
    sprintf (buf, "匿名板（匿名不默认） (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
    getdata (20, 0, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= POSITIVE_ANONY_FLAG;
        else
            newfh.flag &= ~POSITIVE_ANONY_FLAG;
    }
    sprintf (buf, "文章是否 不可以 回复 (Y/N)? [%c]: ", (noreply) ? 'Y' : 'N');
    getdata (20, 30, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= NOREPLY_FLAG;
        else
            newfh.flag &= ~NOREPLY_FLAG;
    }
    move (21, 0);
    if (askyn ("是否移动精华区的位置", NA, NA) == YEA)
        a_mv = 2;
    else
        a_mv = 0;
    move (22, 0);
    if (askyn ("是否更改存取权限", NA, NA) == YEA)
    {
        char ans[4];

        sprintf (genbuf, "限制 (R)阅读 或 (P)张贴 文章 [%c]: ",
                 (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
        getdata (23, 0, genbuf, ans, 2, DOECHO, YEA);
        if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
            newfh.level &= ~PERM_POSTMASK;
        else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
            newfh.level |= PERM_POSTMASK;
        clear ();
        move (2, 0);
        prints ("设定 %s '%s' 讨论区的权限\n",
                newfh.level & PERM_POSTMASK ? "张贴" : "阅读", newfh.filename);
        newfh.level = setperms (newfh.level, "权限", NUMPERMS, showperminfo);
        clear ();
        getdata (0, 0, "确定要更改吗? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
    }
    else
    {
        getdata (23, 0, "确定要更改吗? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
    }
    if (*genbuf != 'Y' && *genbuf != 'y')
    {
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "修改讨论区：%s(%s)", fh.filename, newfh.filename);
        securityreport (secu);
    }
    if (strcmp (fh.filename, newfh.filename))
    {
        char old[256], tar[256];

        a_mv = 1;
        setbpath (old, fh.filename);
        setbpath (tar, newfh.filename);
        rename (old, tar);
        sprintf (old, "vote/%s", fh.filename);
        sprintf (tar, "vote/%s", newfh.filename);
        rename (old, tar);
    }
    if (newfh.BM[0] != '\0')
        sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11, newfh.BM);
    else
        sprintf (vbuf, "%-38.38s", newfh.title + 11);
    get_grp (fh.filename);
    edit_grp (fh.filename, lookgrp, oldtitle + 11, vbuf);
    if (a_mv >= 1)
    {
        group = chgrp ();
        get_grp (fh.filename);
        strcpy (tmp_grp, lookgrp);
        if (strcmp (tmp_grp, group) || a_mv != 2)
        {
            char tmpbuf[160];

            sprintf (tmpbuf, "%s:", fh.filename);
            del_from_file ("0Announce/.Search", tmpbuf);
            if (group != NULL)
            {
                if (newfh.BM[0] != '\0')
                    sprintf (vbuf, "%-38.38s(BM: %s)", newfh.title + 11,
                             newfh.BM);
                else
                    sprintf (vbuf, "%-38.38s", newfh.title + 11);
                if (add_grp (group, cexplain, newfh.filename, vbuf) == -1)
                    prints ("\n成立精华区失败....\n");
                else
                    prints ("已经置入精华区...\n");
                sprintf (newpath, "0Announce/groups/%s/%s", group,
                         newfh.filename);
                sprintf (oldpath, "0Announce/groups/%s/%s", tmp_grp,
                         fh.filename);
                if (dashd (oldpath))
                {
                    deltree (newpath);
                }
                rename (oldpath, newpath);
                del_grp (tmp_grp, fh.filename, fh.title + 11);
            }
        }
    }
    substitute_record (BOARDS, &newfh, sizeof (newfh), pos);
    sprintf (genbuf, "更改讨论区 %s 的资料 --> %s", fh.filename,
             newfh.filename);
    report (genbuf);
    numboards = -1;		/* force re-caching */
    clear ();
    return 0;
}
#endif

int
m_mclean ()
{
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("清除私人信件");
    move (1, 0);
    prints ("清除所有已读且未 mark 的信件\n");
    if (askyn ("确定吗", NA, NA) == NA)
    {
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "清除所有使用者已读信件。");
        securityreport (secu);
    }

    cleanlog = fopen ("mailclean.log", "w");
    move (3, 0);
    prints ("请耐心等候.\n");
    refresh ();
    if (apply_record (PASSFILE, cleanmail, sizeof (struct userec)) == -1)
    {
        move (4, 0);
        prints ("apply PASSFILE err...\n");
        pressreturn ();
        clear ();
        return -1;
    }
    move (4, 0);
    fclose (cleanlog);
    prints ("清除完成! 记录档 mailclean.log.\n");
    pressreturn ();
    clear ();
    return 0;
}

/*
int
m_trace()
{
	struct stat ostatb, cstatb;
	int     otflag, ctflag, done = 0;
	char    ans[3];
	char   *msg;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return;
	}
	clear();
	stand_title("Set Trace Options");
	while (!done) {
		move(2, 0);
		otflag = stat("trace", &ostatb);
		ctflag = stat("trace.chatd", &cstatb);
		prints("目前设定:\n");
		trace_state(otflag, "一般", ostatb.st_size);
		trace_state(ctflag, "聊天", cstatb.st_size);
		move(9, 0);
		prints("<1> 切换一般记录\n");
		prints("<2> 切换聊天记录\n");
		getdata(12, 0, "请选择 (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);
 
		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "一般记录 ON";
			} else {
				rename("trace", "trace.old");
				msg = "一般记录 OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "聊天记录 ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "聊天记录 OFF";
			}
			break;
		default:
			msg = NULL;
			done = 1;
		}
		move(t_lines - 2, 0);
		if (msg) {
			prints("%s\n", msg);
			report(msg);
		}
	}
	clear();
	return 0;
}
*/

int
m_register ()
{
    FILE *fn;
    char ans[3], /*fname[20], */fname_new[30];
    int x, y, wid, len;
    char uident[STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();

    stand_title ("设定使用者注册资料");
    for (;;)
    {
        getdata (1, 0,
                 "(0)离开  (1)审查注册资料  (2)查询使用者注册资料: ",
                 ans, 2, DOECHO, YEA);
        if (ans[0] == '1' || ans[0] == '2'/* || ans[0] == '3'*/)
            break;
        else
            return 0;
    }
//    sprintf(fname, "new_register");
	sprintf(fname_new, "register_sjtubbs");
//    if (ans[0] == '1')
//    {
//        if ((fn = fopen (fname, "r")) == NULL)
//        {
//            prints ("\n\n目前并无新注册资料.");
//            pressreturn ();
//        }
//        else
//        {
//            y = 3, x = wid = 0;
//            while (fgets (genbuf, STRLEN, fn) != NULL && x < 65)
//            {
//                if (strncmp (genbuf, "userid: ", 8) == 0)
//                {
//                    move (y++, x);
//                    prints ("%s", genbuf + 8);
//                    len = strlen (genbuf + 8);
//                    if (len > wid)
//                        wid = len;
//                    if (y >= t_lines - 2)
//                    {
//                        y = 3;
//                        x += wid + 2;
//                    }
//                }
//            }
//            fclose (fn);
//            if (askyn ("设定资料吗", NA, YEA) == YEA)
//            {
//                securityreport ("设定使用者注册资料");
//                scan_register_form (fname, NULL);
//            }
//        }
//    }
    
	//else //新版本的注册单，by rwzmm @ sjtubbs
	if(ans[0] == '1')
	{
		struct stat st;
		int record_ns = 0;
		struct regform record = {0};
		if ((fn = fopen (fname_new, "r")) == NULL)
        {
            prints ("\n\n目前并无新注册资料.");
            pressreturn ();
        }

		else
		{
			stat(fname_new, &st);
			record_ns = st.st_size / sizeof(struct regform);
			if(record_ns == 0)
			{
				prints ("\n\n目前并无新注册资料.");
				pressreturn ();
			}
			else
			{
				move(3,0);
				prints("目前共有新注册资料 %d 份。", record_ns);

				y = 5, x = wid = 0;
				while ((fread (&record, sizeof(struct regform), 1, fn) >0) && (x < 65))
				{
					move (y++, x);
					prints ("%s", record.userid);
					len = IDLEN;
					if (len > wid)
						wid = len;
					if (y >= t_lines - 2)
					{
						y = 5;
						x += wid + 2;
					}
				}
				fclose (fn);
				if (askyn ("设定资料吗", NA, YEA) == YEA)
				{
					securityreport ("设定使用者注册资料");
					scan_new_register_form (fname_new, NULL);
				}
			}
		}
	}
    else
    {
        move (1, 0);
        usercomplete ("请输入要查询的代号: ", uident);
        if (uident[0] != '\0')
            if (!getuser (uident))
            {
                move (2, 0);
                prints ("错误的使用者代号...");
            }
            else
            {
                sprintf (genbuf, "home/%c/%s/register",
                         toupper (lookupuser.userid[0]), lookupuser.userid);
                if ((fn = fopen (genbuf, "r")) != NULL)
                {
                    prints ("\n注册资料如下:\n\n");
                    for (x = 1; x <= 15; x++)
                    {
                        if (fgets (genbuf, STRLEN, fn))
                            prints ("%s", genbuf);
                        else
                            break;
                    }
                }
                else
                {
                    prints ("\n\n找不到他/她的注册资料!!\n");
//                    scan_register_form ();
					scan_new_register_form(fname_new, lookupuser.userid);
                }
            }
        pressanykey ();
    }
    clear ();
    return 0;
}

int
d_board ()
{
    struct boardheader binfo;
    int bid, ans;
    char bname[STRLEN];
    extern char lookgrp[];
    extern int numboards;

    if (!HAS_PERM (PERM_BLEVELS))
    {
        return 0;
    }
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("删除讨论区");
    make_blist ();
    move (1, 0);
    namecomplete ("请输入讨论区: ", bname);
    if (bname[0] == '\0')
        return 0;
    bid = getbnum (bname);
    if (get_record (BOARDS, &binfo, sizeof (binfo), bid) == -1)
    {
        move (2, 0);
        prints ("不正确的讨论区\n");
        pressreturn ();
        clear ();
        return 0;
    }
    if (binfo.BM[0] != '\0' && strcmp (binfo.BM, "SYSOP"))
    {
        move (5, 0);
        prints ("该板还有板主，在删除本板前，请先取消板主的任命。\n");
        pressanykey ();
        clear ();
        return 0;
    }
    ans = askyn ("你确定要删除这个讨论区", NA, NA);
    if (ans != 1)
    {
        move (2, 0);
        prints ("取消删除行动\n");
        pressreturn ();
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "删除讨论区：%s", binfo.filename);
        securityreport (secu);
    }
    if (seek_in_file ("0Announce/.Search", bname))
    {
        move (4, 0);
        if (askyn ("移除精华区", NA, NA) == YEA)
        {
            get_grp (binfo.filename);
            del_grp (lookgrp, binfo.filename, binfo.title + 11);
        }
    }
    if (seek_in_file ("etc/junkboards", bname))
        del_from_file ("etc/junkboards", bname);
    if (seek_in_file ("0Announce/.Search", bname))
    {
        char tmpbuf[160];

        sprintf (tmpbuf, "%s:", bname);
        del_from_file ("0Announce/.Search", tmpbuf);
    }
    if (binfo.filename[0] == '\0')
        return -1;			/* rrr - precaution */
    sprintf (genbuf, "boards/%s", binfo.filename);
    f_rm (genbuf);
    sprintf (genbuf, "vote/%s", binfo.filename);
    f_rm (genbuf);
    sprintf (genbuf, " << '%s' 被 %s 删除 >>",
             binfo.filename, currentuser.userid);
    memset (&binfo, 0, sizeof (binfo));
    strncpy (binfo.title, genbuf, STRLEN);
    binfo.level = PERM_SYSOP;
    substitute_record (BOARDS, &binfo, sizeof (binfo), bid);

    move (4, 0);
    prints ("\n本讨论区已经删除...\n");
    pressreturn ();
    numboards = -1;
    clear ();
    return 0;
}

int
d_user (cid)
char *cid;
{
    int id, num, i;
    char secu[STRLEN];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    stand_title ("删除使用者帐号");
    move (2, 0);
    if (!gettheuserid (1, "请输入欲删除的使用者代号: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "SYSOP"))
    {
        prints ("\n对不起，你不可以删除 SYSOP 帐号!!\n");
        pressreturn ();
        clear ();
        return 0;
    }
    if (!strcmp (lookupuser.userid, currentuser.userid))
    {
        prints ("\n对不起，你不可以删除自己的这个帐号!!\n");
        pressreturn ();
        clear ();
        return 0;
    }
    prints ("\n\n以下是 [%s] 的部分资料:\n", lookupuser.userid);
    prints ("    User ID:  [%s]\n", lookupuser.userid);
    prints ("    昵   称:  [%s]\n", lookupuser.username);
    prints ("    姓   名:  [%s]\n", lookupuser.realname);
    strcpy (secu, "bTCPRD#@XWBA#VS-DOM-F0s23456789t");
    for (num = 0; num < strlen (secu); num++)
        if (!(lookupuser.userlevel & (1 << num)))
            secu[num] = '-';
    prints ("    权   限: %s\n\n", secu);

    num = getbnames (lookupuser.userid, secu, &num);
    if (num)
    {
        prints ("[%s] 目前尚担任了 %d 个板的板主: ", lookupuser.userid, num);
        for (i = 0; i < num; i++)
            prints ("%s ", bnames[i]);
        prints ("\n请先使用板主卸职功能取消其板主职务再做该操作.");
        pressanykey ();
        clear ();
        return 0;
    }

    sprintf (genbuf, "你确认要删除 [%s] 这个 ID 吗", lookupuser.userid);
    if (askyn (genbuf, NA, NA) == NA)
    {
        prints ("\n取消删除使用者...\n");
        pressreturn ();
        clear ();
        return 0;
    }
    sprintf (secu, "删除使用者：%s", lookupuser.userid);
    securityreport (secu);
    sprintf (genbuf, "mail/%c/%s", toupper (lookupuser.userid[0]),
             lookupuser.userid);
    f_rm (genbuf);
    sprintf (genbuf, "home/%c/%s", toupper (lookupuser.userid[0]),
             lookupuser.userid);
    f_rm (genbuf);
    lookupuser.userlevel = 0;
    strcpy (lookupuser.address, "");
#ifdef ALLOWGAME
	lookupuser.money = 0;
	lookupuser.medals = 0;
	lookupuser.inbank = 0;
#endif
    strcpy (lookupuser.username, "");
    strcpy (lookupuser.realname, "");
    strcpy (lookupuser.termtype, "");
    prints ("\n%s 已经被灭绝了...\n", lookupuser.userid);
    lookupuser.userid[0] = '\0';
    substitute_record (PASSFILE, &lookupuser, sizeof (lookupuser), id);
    setuserid (id, lookupuser.userid);
    pressreturn ();
    clear ();
    return 1;
}

int
x_level ()
{
    int id;
    unsigned int newlevel;

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (0, 0);
    prints ("更改使用者权限\n");
    clrtoeol ();
    move (1, 0);
    usercomplete ("输入欲更改的使用者帐号: ", genbuf);
    if (genbuf[0] == '\0')
    {
        clear ();
        return 0;
    }
    if (!(id = getuser (genbuf)))
    {
        move (3, 0);
        prints ("Invalid User Id");
        clrtoeol ();
        pressreturn ();
        clear ();
        return 0;
    }
    move (1, 0);
    clrtobot ();
    move (2, 0);
    prints ("设定使用者 '%s' 的权限 \n", genbuf);
    newlevel = setperms (lookupuser.userlevel, "权限", NUMPERMS, showperminfo);
    move (2, 0);
    if (newlevel == lookupuser.userlevel)
        prints ("使用者 '%s' 权限没有变更\n", lookupuser.userid);
    else
    {
        lookupuser.userlevel = newlevel;
        {
            char secu[STRLEN];

            sprintf (secu, "修改 %s 的权限", lookupuser.userid);
            securityreport (secu);
        }

        substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
        if (!(lookupuser.userlevel & PERM_LOGINOK))
        {
            char src[STRLEN], dst[STRLEN];

            sethomefile (dst, lookupuser.userid, "register.old");
            if (dashf (dst))
                unlink (dst);
            sethomefile (src, lookupuser.userid, "register");
            if (dashf (src))
                rename (src, dst);
        }
        prints ("使用者 '%s' 权限已经更改完毕.\n", lookupuser.userid);
    }
    pressreturn ();
    clear ();
    return 0;
}


int
x_denylevel ()
{
    int id;
    char ans[7];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (0, 0);
    prints ("更改使用者基本权限或资料\n");
    clrtoeol ();
    move (1, 0);
    usercomplete ("输入欲更改的使用者帐号: ", genbuf);
    if (genbuf[0] == '\0')
    {
        clear ();
        return 0;
    }
    if (!(id = getuser (genbuf)))
    {
        move (3, 0);
        prints ("Invalid User Id");
        clrtoeol ();
        pressreturn ();
        clear ();
        return 0;
    }
    move (1, 0);
    clrtobot ();
    move (2, 0);
    prints ("设定使用者 '%s' 的基本权限或资料 \n\n", genbuf);
    prints ("(1) 封禁发表文章权力       (A) 恢复发表文章权力\n");
    prints ("(2) 取消基本上站权力       (B) 恢复基本上站权力\n");
    prints ("(3) 禁止进入聊天室         (C) 恢复进入聊天室权力\n");
    prints ("(4) 禁止呼叫他人聊天       (D) 恢复呼叫他人聊天权力\n");
    prints ("(5) 禁止给他人发消息       (E) 恢复给他人发消息权力\n");
    prints ("(6) 修改 %s 基本资料\n", genbuf);
    getdata (12, 0, "请输入你的处理: ", ans, 3, DOECHO, YEA);
    switch (ans[0])
    {
    case '1':
        lookupuser.userlevel &= ~PERM_POST;
        break;
    case 'a':
    case 'A':
        lookupuser.userlevel |= PERM_POST;
        break;
    case '2':
        lookupuser.userlevel &= ~PERM_BASIC;
        break;
    case 'b':
    case 'B':
        lookupuser.userlevel |= PERM_BASIC;
        break;
    case '3':
        lookupuser.userlevel &= ~PERM_CHAT;
        break;
    case 'c':
    case 'C':
        lookupuser.userlevel |= PERM_CHAT;
        break;
    case '4':
        lookupuser.userlevel &= ~PERM_PAGE;
        break;
    case 'd':
    case 'D':
        lookupuser.userlevel |= PERM_PAGE;
        break;
    case '5':
        lookupuser.userlevel &= ~PERM_MESSAGE;
        break;
    case 'e':
    case 'E':
        lookupuser.userlevel |= PERM_MESSAGE;
        break;
    case '6':
        uinfo_change1 (12, &lookupuser, &lookupuser);
        break;
    default:
        prints ("\n 使用者 '%s' 基本权力和资料没有变更\n", lookupuser.userid);
        pressreturn ();
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "修改 %s 的基本权限或资料", lookupuser.userid);
        securityreport (secu);
    }
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    prints ("\n 使用者 '%s' 基本权限或资料已经更改完毕.\n", lookupuser.userid);
    pressreturn ();
    clear ();
    return 0;
}

void
a_edits ()
{
    int aborted;
    char ans[7], buf[STRLEN], buf2[STRLEN];
    int ch, num, confirm;
    extern struct BCACHE *brdshm;
    static char *e_file[] =
        { "../Welcome", "../Welcome2", "issue", "logout", "../vote/notes",
          "menu.ini", "../.badname", "../.bad_email", "../.bad_host",
          "autopost", "junkboards","no_top_ten", "sysops", "expire.ctl", "whatdate",
          "../NOLOGIN", "../NOREGISTER", "../.blockmail", "s_fill",
          "f_fill", "register", "firstlogin", "chatstation",
          "bbsnet.ini", "bbsnet2.ini", "notbackupboards",
          "DISTmaster.ini", "macroboards",  "goodbm","shop.ini", NULL
        };
    static char *explain_file[] =
        { "特殊进站公布栏", "进站画面", "进站欢迎档", "离站画面",
          "公用备忘录", "menu.ini", "不可注册的 ID", "不可确认之E-Mail",
          "不可上站之位址",
          "每日自动送信档", "不算POST数的板","不计入十大统计的板", "管理者名单", "定时砍信设定档",
          "纪念日清单", "暂停登陆(NOLOGIN)", "暂停注册(NOREGISTER)",
          "转信黑名单", "注册成功信件", "注册失败信件", "新用户注册范例",
          "用户第一次登陆公告", "国际会议厅清单", "BBSNET 转站清单",
          "BBSNET2 转站清单", "区段删除不需备份之清单", "区长名单",
          "允许使用宏定义板","优秀板主名单", "思源湖商店",NULL
        };

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (1, 0);
    prints ("编修系统档案\n\n");
    for (num = 0;
            HAS_PERM (PERM_SYSOP) ? e_file[num] != NULL
            && explain_file[num] != NULL : explain_file[num] != "menu.ini"; num++)
    {
        prints ("[[1;32m%2d[m] %s", num + 1, explain_file[num]);
        if (num < 17)
            move (4 + num, 0);
        else
            move (num - 14, 50);
    }
    prints ("[[1;32m%2d[m] 都不想改\n", num + 1);

    getdata (23, 0, "你要编修哪一项系统档案: ", ans, 3, DOECHO, YEA);
    ch = atoi (ans);
    if (!isdigit (ans[0]) || ch <= 0 || ch > num || ans[0] == '\n'
            || ans[0] == '\0')
        return;
    ch -= 1;
    sprintf (buf2, "etc/%s", e_file[ch]);
    move (3, 0);
    clrtobot ();
    sprintf (buf, "(E)编辑 (D)删除 %s? [E]: ", explain_file[ch]);
    getdata (3, 0, buf, ans, 2, DOECHO, YEA);
    if (ans[0] == 'D' || ans[0] == 'd')
    {
        sprintf (buf, "你确定要删除 %s 这个系统档", explain_file[ch]);
        confirm = askyn (buf, NA, NA);
        if (confirm != 1)
        {
            move (5, 0);
            prints ("取消删除行动\n");
            pressreturn ();
            clear ();
            return;
        }
        {
            char secu[STRLEN];

            sprintf (secu, "删除系统档案：%s", explain_file[ch]);
            securityreport (secu);
        }
        unlink (buf2);
        move (5, 0);
        prints ("%s 已删除\n", explain_file[ch]);
        pressreturn ();
        clear ();
        return;
    }
    modify_user_mode (EDITSFILE);
    aborted = vedit (buf2, NA, YEA);	/* 不添加文件头, 允许修改头部信息 */
    clear ();
    if (aborted != -1)
    {
        prints ("%s 更新过", explain_file[ch]);
        {
            char secu[STRLEN];

            sprintf (secu, "修改系统档案：%s", explain_file[ch]);
            securityreport (secu);
        }

        if (!strcmp (e_file[ch], "../Welcome"))
        {
            unlink ("Welcome.rec");
            prints ("\nWelcome 记录档更新");
        }
        else if (!strcmp (e_file[ch], "whatdate"))
        {
            brdshm->fresh_date = time (0);
            prints ("\n纪念日清单 更新");
        }
    }
    pressreturn ();
}

int
wall ()
{
    if (!HAS_PERM (PERM_SYSOP))
        return 0;
    modify_user_mode (MSG);
    move (2, 0);
    clrtobot ();
    if (!get_msg ("所有使用者", buf2, 1))
    {
        return 0;
    }
    if (apply_ulist (dowall) == -1)
    {
        move (2, 0);
        prints ("线上空无一人\n");
        pressanykey ();
    }
    prints ("\n已经广播完毕...\n");
    pressanykey ();
    return 1;
}

int
setsystempasswd ()
{
    FILE *pass;
    char passbuf[20], prepass[20];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;
    if (strcmp (currentuser.userid, "SYSOP"))
    {
        clear ();
        move (10, 20);
        prints ("对不起，系统密码只能由 SYSOP 修改！");
        pressanykey ();
        return;
    }
    getdata (2, 0, "请输入新的系统密码(直接回车则取消系统密码): ",
             passbuf, 19, NOECHO, YEA);
    if (passbuf[0] == '\0')
    {
        if (askyn ("你确定要取消系统密码吗?", NA, NA) == YEA)
        {
            unlink ("etc/.syspasswd");
            securityreport ("[32m取消系统密码[37m");
        }
        return;
    }
    getdata (3, 0, "确认新的系统密码: ", prepass, 19, NOECHO, YEA);
    if (strcmp (passbuf, prepass))
    {
        move (4, 0);
        prints ("两次密码不相同, 取消此次设定.");
        pressanykey ();
        return;
    }
    if ((pass = fopen ("etc/.syspasswd", "w")) == NULL)
    {
        move (4, 0);
        prints ("系统密码无法设定....");
        pressanykey ();
        return;
    }
    fprintf (pass, "%s\n", genpasswd (passbuf));
    fclose (pass);
    move (4, 0);
    prints ("系统密码设定完成....");
    pressanykey ();
    return;
}

int
x_csh ()
{
    char buf[PASSLEN];
    int save_pager;
    int magic;

    if (!HAS_PERM (PERM_SYSOP))
    {
        return 0;
    }
    if (!check_systempasswd ())
    {
        return;
    }
    modify_user_mode (SYSINFO);
    clear ();
    getdata (1, 0, "请输入通行暗号: ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !checkpasswd (currentuser.passwd, buf))
    {
        prints ("\n\n暗号不正确, 不能执行。\n");
        pressreturn ();
        clear ();
        return;
    }
    randomize ();
    magic = rand () % 1000;
    prints ("\nMagic Key: %d", magic * 5 - 2);
    getdata (4, 0, "Your Key : ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !(atoi (buf) == magic))
    {
        securityreport ("Fail to shell out");
        prints ("\n\nKey 不正确, 不能执行。\n");
        pressreturn ();
        clear ();
        return;
    }
    securityreport ("Shell out");
    modify_user_mode (SYSINFO);
    clear ();
    refresh ();
    reset_tty ();
    save_pager = uinfo.pager;
    uinfo.pager = 0;
    update_utmp ();
    do_exec ("csh", NULL);
    restore_tty ();
    uinfo.pager = save_pager;
    update_utmp ();
    clear ();
    return 0;
}
#endif

//add 2002.5.26 by mudboy
//mudboy add this function
int
add_to_QZ (char *userid, int num)
{
    FILE *fp, *nfp;
    char t[256], tempbuf[256], fnnew[80], buf[1024];
    char *t1, *t2, *t3;
    char datafile[80] = "etc/DISTmaster.ini";
    int counts;

    if ((fp = fopen (datafile, "r")) == NULL)
        return -1;
    sprintf (fnnew, "%s.%d", datafile, getuid ());
    if ((nfp = fopen (fnnew, "w")) == NULL)
    {
        fclose (fp);
        return -1;
    }
    counts = 0;
    while (fgets (t, 255, fp) != NULL)
    {
        strcpy (tempbuf, t);
        t1 = strtok (t, " \t\r\n");
        if (t1 == NULL || t1[0] == '#')
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        counts++;
        if (counts != num)
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        if (!isalpha (t1[0]))
        {
            fprintf (nfp, "%s\n", userid);
            continue;
        }
        t2 = strtok (NULL, " \t\r\n");
        if (t2 == NULL)
        {
            sprintf (buf, "%s\t%s", t1, userid);
            fprintf (nfp, "%s\n", buf);
            continue;
        }
        t3 = strtok (NULL, " \t\r\n");
        if (t3 == NULL)
        {
            sprintf (buf, "%s\t%s\t%s", t1, t2, userid);
            fprintf (nfp, "%s\n", buf);
            continue;
        }
        else
            return -1;
    }
    fclose (fp);
    fclose (nfp);
    return (rename (fnnew, datafile) + 1);
}

//mudboy add this function
int
m_ordainQZ ()
{
    int id, i, j, counts;
    char num[2];
    char buf[5][STRLEN];
    char secu[STRLEN];
    char title[STRLEN], msg[1024];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("任命区长\n");
    clrtoeol ();
    if (!gettheuserid (2, "输入欲任命的使用者帐号: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "guest"))
    {
        move (5, 0);
        prints ("你不能任命 guest 当区长");
        pressanykey ();
        clear ();
        return -1;
    }
    if (chkGroupsInfo () == 0)
        return -1;
    for (i = 1, j = 1; i < SectNum; i++)
    {
        if (GroupsInfo.name[i][0] == '\0')
            continue;
        move (j++, 60);
        prints ("┃(%d)%-12s\033[m┃", i, GroupsInfo.name[i]);
    }
    num[0] = 0;
    getdata (j, 0, "任命哪一分区(参考右边提示编号)？: ", num, 2, DOECHO, NA);

    if (num[0] >= 'a' && num[0] <= 'z')
        num[0] -= 32;		//小写统一到大写
    if (num[0] >= 'A' && num[0] <= 'Z')
        num[0] -= 7;		//字母统一到数字
    if (num[0] == '\0' || num[0] < '1' || num[0] >= '0' + SectNum)
        return -1;
    if (GroupsInfo.name[num[0] - '0'][0] == '\0')
        return -1;
    counts = WHICHDIST (lookupuser.userid);
    if (counts < 12)
    {
        if(counts == 10)
            prints ("%s 已经是 A 区的区长了.", lookupuser.userid);
        else if(counts == 11)
            prints ("%s 已经是 B 区的区长了.", lookupuser.userid);
        else
            prints ("%s 已经是 %d 区的区长了.", lookupuser.userid, counts);

        pressanykey ();
        clear ();
        return -1;
    }
    if(num[0] > '9')
    {
        num[0] += 7;
        prints ("\n你将任命 %s 为 %s 区的区长吗?\n", lookupuser.userid, num);
        num[0] -= 7;
    }
    else
        prints ("\n你将任命 %s 为 %s 区的区长吗?\n", lookupuser.userid, num);
    if (askyn ("你确定要任命吗?", NA, NA) == NA)
    {
        prints ("取消任命区长");
        pressanykey ();
        clear ();
        return -1;
    }
    if (add_to_QZ (lookupuser.userid, num[0] - '0') == -1)
    {
        if(num[0] > '9')
        {
            num[0] += 7;
            prints ("%s 区己经有三个区长了.", num);
            num[0] -= 7;
        }
        else
            prints ("%s 区己经有三个区长了.", num);
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';

    clear ();
    move (8, 0);
    prints ("请输入任命附言(最多五行，按 Enter 结束)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    lookupuser.userlevel |= PERM_SPECIAL7;
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    sprintf (secu, "区长任命, 给予 %s 区长权限", lookupuser.userid);
    securityreport (secu);
    move (15, 0);
    prints (secu);

    if(num[0] > '9')
    {
        num[0] += 7;
        sprintf (title, "任命 %s 为 %s 区区长", lookupuser.userid, num);
        sprintf (msg, "\n\t\t\t【 通告 】\n\n\t任命 %s 为 %s 区区长！\n", lookupuser.userid, num);
        num[0] -= 7;
    }
    else
    {
        sprintf (title, "任命 %s 为 %s 区区长", lookupuser.userid, num);
        sprintf (msg, "\n\t\t\t【 通告 】\n\n\t任命 %s 为 %s 区区长！\n", lookupuser.userid, num);

    }
    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (msg, "\n\n站长任命附言：\n");
        strcat (msg, buf[i]);
        strcat (msg, "\n");
    }
    strcpy (currboard, "announce");
    autoreport (title, msg, NULL, NULL);
    pressanykey ();
}

//mudboy add this function
int
del_from_QZ (char *userid, int num)
{
    FILE *fp, *nfp;
    char t[256], tempbuf[256], fnnew[STRLEN];
    char *t1, *t2, *t3;
    char datafile[80] = BBSHOME "/etc/DISTmaster.ini";
    int counts;

    if ((fp = fopen (datafile, "r")) == NULL)
        return -1;
    sprintf (fnnew, "%s.%d", datafile, getuid ());
    if ((nfp = fopen (fnnew, "w")) == NULL)
    {
        fclose (fp);
        return -1;
    }
    counts = 0;
    while (fgets (t, 255, fp) != NULL)
    {
        strcpy (tempbuf, t);
        t1 = strtok (t, " \t\r\n");
        if (t1 == NULL || t1[0] == '#')
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        counts++;
        if (counts != num)
        {
            fprintf (nfp, "%s", tempbuf);
            continue;
        }
        if (!isalpha (t1[0]))
        {
            return -1;
        }
        if (strcasecmp (t1, userid) != 0)
            fprintf (nfp, "%s\t", t1);
        t2 = strtok (NULL, " \t\r\n");
        if (t2 == NULL)
        {
            if (strcasecmp (t1, userid) != 0)
                return -1;
            fprintf (nfp, "/%d\n", counts);
            continue;
        }
        if (strcasecmp (t2, userid) != 0)
            fprintf (nfp, "%s\t", t2);
        t3 = strtok (NULL, " \t\r\n");
        if (t3 == NULL)
        {
            if (strcasecmp (t1, userid) != 0 && strcasecmp (t2, userid) != 0)
                return -1;
            fprintf (nfp, "\n");
            continue;
        }
        if (strcasecmp (t3, userid) != 0)
            fprintf (nfp, "%s\n", t3);
        if (strcasecmp (t1, userid) != 0 && strcasecmp (t2, userid) != 0
                && strcasecmp (t2, userid) != 0)
            return -1;
    }
    fclose (fp);
    fclose (nfp);
    return (rename (fnnew, datafile) + 1);
}

//mudboy add this function
int
m_retireQZ ()
{
    int id, i, counts;
    char buf[5][STRLEN];
    char secu[STRLEN];
    char title[STRLEN], msg[1024];

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("区长离任\n");
    clrtoeol ();
    if (!gettheuserid (2, "输入欲离任的使用者帐号: ", &id))
        return 0;
    counts = WHICHDIST (lookupuser.userid);
    if (counts == 12)
    {
        prints ("%s 不是任何一个区的区长了.", lookupuser.userid);
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\n你将取消 %s 的 区长职务\n", lookupuser.userid);
    if (askyn ("你确定要取消吗?", NA, NA) == NA)
    {
        prints ("取消区长离任");
        pressanykey ();
        clear ();
        return -1;
    }
    if (del_from_QZ (lookupuser.userid, counts) != 1)
    {
        prints ("离任操作发生错误,请检查etc/DISTmaster.ini");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    clear ();
    move (8, 0);
    prints ("请输入离任附言(最多五行，按 Enter 结束)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    lookupuser.userlevel &= ~(PERM_SPECIAL7);
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    sprintf (secu, "区长离任, 取消 %s 区长权限", lookupuser.userid);
    securityreport (secu);
    move (15, 0);
    prints (secu);

    if(counts == 10)
    {
        sprintf (title, "A 区区长 %s 离任通告", lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t【 通告 】\n\n\t经站务组讨论：\n\t同意 %s 辞去 A 区区长的职务。\n\t在此，对他曾经在 A 区的辛苦劳作表示感谢。\n\n",
                 lookupuser.userid);
    }

    else if(counts == 11)
    {
        sprintf (title, "B 区区长 %s 离任通告", lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t【 通告 】\n\n\t经站务组讨论：\n\t同意 %s 辞去 B 区区长的职务。\n\t在此，对他曾经在 B 区的辛苦劳作表示感谢。\n\n",
                 lookupuser.userid);
    }


    else
    {

        sprintf (title, "%d 区区长 %s 离任通告", counts, lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t【 通告 】\n\n\t经站务组讨论：\n\t同意 %s 辞去 %d 区区长的职务。\n\t在此，对他曾经在 %d 区的辛苦劳作表示感谢。\n\n",
                 lookupuser.userid, counts, counts);
    }


    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (msg, "\n\n站长附言:\n");
        strcat (msg, buf[i]);
        strcat (msg, "\n");
    }
    strcpy (currboard, "announce");
    autoreport (title, msg, NULL, NULL);
    pressanykey ();
}

//add end 2002.5.26 by mudboy

