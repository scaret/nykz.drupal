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
    stand_title ("�޸�ʹ��������");
    if (!gettheuserid (1, "������ʹ���ߴ���: ", &id))
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
    stand_title ("��������\n");
    clrtoeol ();
    if (!gettheuserid (2, "������������ʹ�����ʺ�: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "guest"))
    {
        move (5, 0);
        prints ("�㲻������ guest ������");
        pressanykey ();
        clear ();
        return -1;
    }
    if (!gettheboardname
            (3, "�����ʹ���߽����������������: ", &pos, &fh, bname))
        return -1;
    if (fh.BM[0] != '\0')
    {
        if (!strncmp (fh.BM, "SYSOP", 5))
        {
            move (4, 0);
            if (askyn ("�������������� SYSOP, ��ȷ���ð���Ҫ������", NA, NA)
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
                prints ("%s ������������������", bname);
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
        prints (" %s �Ѿ���%s��İ�����", lookupuser.userid,
                find ? "��" : "����");
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\n�㽫���� %s Ϊ %s ���%s.\n", lookupuser.userid, bname,
            bm ? "��" : "��");
    if (askyn ("��ȷ��Ҫ������?", NA, NA) == NA)
    {
        prints ("ȡ����������");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    move (8, 0);
    prints ("��������������(������У��� Enter ����)");
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
        sprintf (secu, "��������, ���� %s �İ���Ȩ��", lookupuser.userid);
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
    sprintf (bmfilename, "���� %s Ϊ %s ������%s",
             lookupuser.userid, fh.filename, bm ? "����" : "�帱");
    securityreport (bmfilename);
    move (16, 0);
    prints (bmfilename);
	if(strncmp(bname, "BMTraining", 10))	//not a bmt board
	{
		sprintf (genbuf, "\n\t\t\t�� ͨ�� ��\n\n"
             "\t���� %s Ϊ %s ��%s��\n"
             "\t�� %s ���������� BMhome �屨����\n"
             "\t����δ�ܱ����ߣ���ȡ������Ȩ�ޡ�\n",
             lookupuser.userid, bname, bm ? "����" : "�帱", lookupuser.userid);
	}
	else	//bmt board
	{
		sprintf (genbuf, "\n\t\t\t�� ͨ�� ��\n\n"
             "\t���� %s Ϊ %s ��%s��\n"
             "\t�� %s ���������� District%c �屨����\n"
             "\t����δ�ܱ����ߣ���ȡ������Ȩ�ޡ�\n",
             lookupuser.userid, bname, bm ? "����" : "�帱", lookupuser.userid, bname[10]);
	}
    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (genbuf, "\n\nվ���������ԣ�\n");
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
    stand_title ("������ְ\n");
    clrtoeol ();
    if (!gettheuserid (2, "��������ְ�İ����ʺ�: ", &id))
        return -1;
    if (!gettheboardname (3, "������ð���Ҫ��ȥ�İ���: ", &pos, &fh, bname))
        return -1;
    oldbm = getbnames (lookupuser.userid, bname, &find);
    if (!oldbm || !find)
    {
        move (5, 0);
        prints (" %s %s����������д�����֪ͨ����վ����",
                lookupuser.userid, (oldbm) ? "���Ǹ�" : "û�е����κ�");
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
        ("�Բ��� %s �����������û�� %s �����д�����֪ͨ����վ����",
         bname, lookupuser.userid);
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\n�㽫ȡ�� %s �� %s ���%sְ��.\n",
            lookupuser.userid, bname, bm ? "��" : "��");
    if (askyn ("��ȷ��Ҫȡ�����ĸð����ְ����?", NA, NA) == NA)
    {
        prints ("\n�Ǻǣ���ı������ˣ� %s �������� %s �����ְ��",
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
    sprintf (genbuf, "ȡ�� %s �� %s ����������ְ��", lookupuser.userid,
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
            sprintf (secu, "����жְ, ȡ�� %s �İ���Ȩ��", lookupuser.userid);
            securityreport (secu);
            move (9, 0);
            prints (secu);
        }
    }
    prints ("\n\n");
    if (askyn ("��Ҫ����ذ��淢��ͨ����?", YEA, NA) == NA)
    {
        pressanykey ();
        return 0;
    }
    prints ("\n");
    if (askyn ("���������밴 Enter ��ȷ�ϣ���ְ�ͷ��� N ��", YEA, NA) == YEA)
        right = 1;
    else
        right = 0;
    if (right)
        sprintf (bmfilename, "%s ��%s %s ����ͨ��", bname,
                 bm ? "����" : "�帱", lookupuser.userid);
    else
        sprintf (bmfilename, "[ͨ��]���� %s ��%s %s ", bname,
                 bm ? "����" : "�帱", lookupuser.userid);
    strcpy (currboard, bname);
    if (right)
    {
        sprintf (genbuf, "\n\t\t\t�� ͨ�� ��\n\n"
                 "\t��վ�������ۣ�\n"
                 "\tͬ�� %s ��ȥ %s ���%sְ��\n"
                 "\t�ڴˣ����������� %s �������������ʾ��л��\n\n"
                 "\tϣ�����Ҳ��֧�ֱ���Ĺ���.",
                 lookupuser.userid, bname, bm ? "����" : "�帱", bname);
    }
    else
    {
        sprintf (genbuf, "\n\t\t\t����ְͨ�桿\n\n"
                 "\t��վ�������۾�����\n"
                 "\t���� %s ��%s %s ��%sְ��\n",
                 bname, bm ? "����" : "�帱", lookupuser.userid,
                 bm ? "����" : "�帱");
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    move (14, 0);
    prints ("������%s����(������У��� Enter ����)",
            right ? "��������" : "������ְ");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 15, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (genbuf, right ? "\n\n���θ��ԣ�\n" : "\n\n��ְ˵����\n");
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

    prints ("\nִ����ϣ�");
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
                //add by alt 4 ��ĸ����
                if (*ptr >= 'a' && *ptr <= 'z')
                    *ptr -= 32;	//Сдͳһ����д
                if (*ptr >= 'A' && *ptr <= 'Z')
                    *ptr -= 7;	//��ĸͳһ������
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
                buf[6] -= 32;	//Сдͳһ����д
            if (buf[6] >= 'A' && buf[6] <= 'Z')
                buf[6] -= 7;	//��ĸͳһ������
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
    prints ("\n��\033[0;1;4;33mע��\033[m��ϵͳ���� menu.ini ������"
            "���ܴ������⣬��������ټ������У�\n        ");
    prints ("��������׸���ʾ�ĺ��壬�뿽��������Ϣ����ˮվ�������⣡\n\n");
    prints ("��վĿǰ������������������£�\n\n");
    prints (" ����   ��������     �����������             ������\n");
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
            ("�������Ϣ���� menu.ini ������������Ȼ�����ִ����", NA, NA) == YEA)
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
        ("���������趨 [\033[32m%s\033[m] �İ������� [Enter-->�趨����]",
         oldfh->filename);
    }
    else
        prints ("������ʼ����һ��\033[32m��\033[m�������� [ENTER-->ȡ������]");
    while (1)
    {
        struct boardheader dh;

        getdata (3, 0, "����������(Ӣ����): ", buf, BDNAME_LEN, DOECHO, YEA);
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
            prints ("\n����! ���������Ѿ�����!!");
            if (oldfh)
                continue;
            pressanykey ();
            return -1;
        }
        strcpy (newfh.filename, buf);
        break;
    }
    prints ("\n[������˵��]�������� + �������� + ת�ű�־ + ����������\n");
    if (oldfh)
    {
        prints ("��ʼ�趨��%11s\033[32m%s\033[m\n", oldfh->title,
                oldfh->title + 11);
    }
    else
        prints ("��ʽ������a[��վ] �� \033[32m����һ��վ��������\033[m\n");
    while (1)
    {
        if (oldfh)
            strcpy (buf, oldfh->title + 11);
        else
            buf[0] = '\0';
        getdata (8, 0, "����������(������): ", buf, 41, DOECHO, NA);
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
    if (askyn ("����������Ҫ����ת����", NA, NA) == YEA)
    {
        strcpy (nntp, "��");
        newfh.flag |= OUT_FLAG;
    }
    else
    {
        strcpy (nntp, "��");
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
        prints ("��%s(%d)%-12s\033[m��",
                (i + '0' == ch) ? "\033[32m" : "", i, GroupsInfo.name[i]);
    }
    while (1)
    {
        if (ch > '0' + 9)
            ch += 7;		//���ֱ���ĸ
        num[0] = ch;
        num[1] = '\0';
        getdata (11, 0, "����������������һ����(�ο��ұ���ʾ���)��: ",
                 num, 2, DOECHO, NA);
        if (num[0] >= 'a' && num[0] <= 'z')
            num[0] -= 32;		//Сдͳһ����д
        if (num[0] >= 'A' && num[0] <= 'Z')
            num[0] -= 7;		//��ĸͳһ������
        if (num[0] == '\0' || num[0] < '0' || num[0] >= ('0' + SectNum))
            continue;
        if (GroupsInfo.name[num[0] - '0'][0] == '\0')
            continue;
        break;
    }
    prints ("\n�� %d ���ķ�����Ųο���[\033[32m%s\033[m] ���������ο���%s",
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
        getdata (14, 0, "��������������ķ������: ", chs, 2, DOECHO, NA);
        if (chs[0] == '\0')
            continue;
        if (strchr (GroupsInfo.chs[num[0] - '0'], chs[0]))
            break;
    }
    move (12, 0);
    prints ("\n�� %d ���ķ�����Ųο���[%s] ���������ο���\033[32m%s\033[m",
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
        getdata (15, 0, "��������������ķ�������: ", buf, 7, DOECHO, NA);
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
        ("\n��������  [ �������Ҫ��������ϵͳ�Զ�����Ϊ�� SYSOP ����]\n");
        if (askyn ("������Ҫ������", YEA, NA) == NA)
            strcpy (newfh.BM, "SYSOP");
        else
            newfh.BM[0] = '\0';
    }
    else
    {
        prints ("\n��������  [ Ŀǰ����İ�����: %s ]\n", oldfh->BM);
        if (askyn
                ("�����ְ������������ʱ�򣬲���Ҫ�޸Ĵ˴�����Ҫ�޸İ���������",
                 NA, NA) == YEA)
        {
            prints ("��\033[31m����\033[m���޸İ���������������ʹ�ã�"
                    "������������Ķ��˴���\n");
            strcpy (newfh.BM, oldfh->BM);
            getdata (20, 0, "��������: ", newfh.BM, sizeof (newfh.BM),
                     DOECHO, NA);
            if (newfh.BM[0] == ' ')
                newfh.BM[0] = '\0';
        }
        else
            strcpy (newfh.BM, oldfh->BM);
    }
    clear ();
    if (askyn ("����������Ҫ���� READ �� POST ������", NA, NA) == YEA)
    {
        char ans[4];

        sprintf (buf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ",
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
        prints ("�趨 %s '%s' ��������Ȩ��\n",
                newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
        newfh.level = setperms (newfh.level, "Ȩ��", NUMPERMS, showperminfo);
        clear ();
    }
    move (1, 0);
	//modified by littlesword@SJTUbbs,positive_anony
    if (askyn
            ("���ñ���Ϊ����������", newfh.flag & ANONY_FLAG ? YEA : NA, NA) == YEA)
		if (askyn ("���ñ�����������ΪĬ����", newfh.flag & ANONY_FLAG ? YEA : NA , NA) == YEA)
			newfh.flag |= ANONY_FLAG;
		else
			newfh.flag |= POSITIVE_ANONY_FLAG;
    else
	{
        newfh.flag &= ~ANONY_FLAG;
		newfh.flag &= ~POSITIVE_ANONY_FLAG;
	}
    move (2, 0);
    if (askyn ("���ñ���Ϊ���� Re ģʽ������",
               (newfh.flag & NOREPLY_FLAG) ? YEA : NA, NA) == YEA)
        newfh.flag |= NOREPLY_FLAG;
    else
        newfh.flag &= ~NOREPLY_FLAG;
    move (3, 0);
    if (oldfh != NULL)
    {
        //      if(askyn("�Ƿ��ƶ���������λ��",NA,NA)==YEA) a_mv = 2;
        //      else a_mv = 0;
        a_mv = 2;
    }
    else
    {
        if (askyn ("������Ҫ������������", YEA, NA) == YEA)
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
            prints ("��%s(%d)%s GROUP_%d\033[m",
                    (i + '0' == ch) ? "\033[32m" : "", i,
                    GroupsInfo.name[i], i);
        }
        while (1)
        {
            strcpy (chs, num);
            if (*chs > '0' + 9)
                *chs += 7;		//���ֵַ���ĸ
            getdata (j + 1, 48, "�������ֲ����ϣ���ѡ��: ", chs, 2, DOECHO, NA);
            if (chs[0] >= 'a' && chs[0] <= 'z')
                chs[0] -= 32;	//Сдͳһ����д
            if (chs[0] >= 'A' && chs[0] <= 'Z')
                chs[0] -= 7;	//��ĸͳһ������
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
        if (askyn ("��ȷ��Ҫ������", NA, NA) == NA)
            return -1;
        i = pos;
    }
    else
        i = getbnum ("");
    if (chs[0] > '0' + 9)
        chs[0] += 7;		//���ַ��뵽��ĸ
    sprintf (group, "GROUP_%c", chs[0]);
    if (oldfh == NULL)
    {
        strcpy (vbuf, "vote/");
        strcat (vbuf, newfh.filename);
        setbpath (genbuf, newfh.filename);
        if ((!dashd (genbuf) && mkdir (genbuf, 0755) == -1)
                || (!dashd (vbuf) && mkdir (vbuf, 0755) == -1))
        {
            prints ("\n��ʼ��������Ŀ¼ʱ����!\n");
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
                chs[0] -= 7;	//��ĸͳһ������
            if (add_grp
                    (group, GroupsInfo.name[chs[0] - '0'], newfh.filename,
                     vbuf) == -1)
                prints ("\n����������ʧ��....\n");
            else
                prints ("�Ѿ����뾫����...\n");
        }
    }
    else
    {				// �޸�������
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
                    chs[0] -= 7;	//��ĸͳһ������
                if (add_grp (group, GroupsInfo.name[chs[0] - '0'],
                             newfh.filename, vbuf) == -1)
                    prints ("\n����������ʧ��....\n");
                else
                    prints ("�Ѿ����뾫����...\n");
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
            sprintf (genbuf, "���������� %s ������ --> %s",
                     oldfh->filename, newfh.filename);
            report (genbuf);
        }
    }
    else if (append_record (BOARDS, &newfh, sizeof (newfh)) == -1)
    {
        prints ("\n�����°�ʧ�ܣ�\n");
        pressanykey ();
        clear ();
        return -1;
    }
    numboards = -1;
    {
        char secu[STRLEN];

        if (oldfh)
            sprintf (secu, "�޸���������%s(%s)", oldfh->filename, newfh.filename);
        else
            sprintf (secu, "�����°壺%s", newfh.filename);
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
    stand_title ("������������");
    ChangeTheBoard (NULL, 0);
    return 0;
}

void
ShowBrdSet (struct boardheader fh)
{
    int i = 4;

    move (i, 0);
    prints ("������ [%s] ���������", fh.filename);
    i += 2;
    move (i++, 0);
    prints ("��Ŀ����           ��Ŀ����");
    move (i++, 0);
    prints ("------------------------------");
    move (i++, 0);
    prints ("����������       : %s", fh.filename);
    move (i++, 0);
    prints ("����������Ա     : %s", fh.BM[0] == '\0' ? "���ް���" : fh.BM);
    move (i++, 0);
    prints ("�Ƿ�Ϊ������������ : %s",
            ((fh.flag & ANONY_FLAG) || (fh.flag & POSITIVE_ANONY_FLAG)) ? "����" : "������");
    move (i++, 0);
    prints ("�����Ƿ���Իظ� : %s",
            (fh.flag & NOREPLY_FLAG) ? "���� Re" : "����");
    move (i++, 0);
    prints ("READ/POST ����   : ");
    if (fh.level & ~PERM_POSTMASK)
    {
        prints ("%s", (fh.level & PERM_POSTMASK) ? "POST" :
                (fh.level & PERM_NOZAP) ? "ZAP" : "READ");
    }
    else
        prints ("������");
    move (i++, 0);
    prints ("��������˵��     : %s\n\n", fh.title);
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
    stand_title ("�޸���������������");
    if (!gettheboardname (2, "��������������: ", &pos, &fh, bname))
        return -1;
    ShowBrdSet (fh);
    move (15, 0);
    if (askyn ("�Ƿ���Ҫ������������", NA, NA) == YEA)
    {
        clear ();
        stand_title ("�޸���������������");
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
    stand_title ("������������");
    memset (&newboard, 0, sizeof (newboard));
    move (2, 0);
    ansimore2 ("etc/boardref", NA, 3, 7);
    while (1)
    {
        getdata (10, 0, "����������:   ", newboard.filename, BDNAME_LEN,
                 DOECHO, YEA);
        if (newboard.filename[0] != 0)
        {
            struct boardheader dh;

            if (search_record
                    (BOARDS, &dh, sizeof (dh), cmpbnames, newboard.filename))
            {
                prints ("\n����! ���������Ѿ�����!!");
                pressanykey ();
                return -1;
            }
        }
        else
            return -1;
        if (valid_brdname (newboard.filename))
            break;
        prints ("\n���Ϸ�����!!");
    }
    newboard.flag = 0;
    while (1)
    {
        getdata (11, 0, "������˵��:   ", newboard.title, 60, DOECHO, YEA);
        if (newboard.title[0] == '\0')
            return -1;
        if (strstr (newboard.title, "��") || strstr (newboard.title, "��"))
        {
            newboard.flag |= OUT_FLAG;
            break;
        }
        else if (strstr (newboard.title, "��"))
        {
            newboard.flag &= ~OUT_FLAG;
            break;
        }
        else
            prints ("����ĸ�ʽ, �޷��ж��Ƿ�ת��!!");
    }
    strcpy (vbuf, "vote/");
    strcat (vbuf, newboard.filename);
    setbpath (genbuf, newboard.filename);
    if (getbnum (newboard.filename) > 0 || mkdir (genbuf, 0755) == -1
            || mkdir (vbuf, 0755) == -1)
    {
        prints ("\n���������������!!\n");
        pressreturn ();
        clear ();
        return -1;
    }
    move (12, 0);
    if (askyn ("�������������(������ SYSOP ����)?", YEA, NA) == NA)
        strcpy (newboard.BM, "SYSOP");
    else
        newboard.BM[0] = '\0';
    move (13, 0);
    if (askyn ("�Ƿ����ƴ�ȡȨ��", NA, NA) == YEA)
    {
        getdata (14, 0, "���� Read/Post? [R]: ", ans, 2, DOECHO, YEA);
        if (*ans == 'P' || *ans == 'p')
            newboard.level = PERM_POSTMASK;
        else
            newboard.level = 0;
        move (1, 0);
        clrtobot ();
        move (2, 0);
        prints ("�趨 %s Ȩ��. ������: '%s'\n",
                (newboard.level & PERM_POSTMASK ? "POST" : "READ"),
                newboard.filename);
        newboard.level =
            setperms (newboard.level, "Ȩ��", NUMPERMS, showperminfo);
        clear ();
    }
    else
        newboard.level = 0;
    move (14, 0);
    if (askyn ("�Ƿ����������", NA, NA) == YEA)
		if (askyn ("�Ƿ���������ΪĬ��", NA, NA) == YEA)
			newboard.flag |= ANONY_FLAG;
		else
			newboard.flag |= POSITIVE_ANONY_FLAG;
    else
	{
        newboard.flag &= ~ANONY_FLAG;
		newboard.flag &= ~POSITIVE_ANONY_FLAG;
	}
    if (askyn ("�ð��ȫ�����¾������Իظ�", NA, NA) == YEA)
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
            prints ("\n����������ʧ��....\n");
        else
            prints ("�Ѿ����뾫����...\n");
    }
    numboards = -1;
    prints ("\n������������\n");
    {
        char secu[STRLEN];

        sprintf (secu, "�����°壺%s", newboard.filename);
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
    stand_title ("�޸���������Ѷ");
    if (!gettheboardname (2, "��������������: ", &pos, &fh, bname))
        return -1;
    noidboard = (fh.flag & ANONY_FLAG) || (fh.flag & POSITIVE_ANONY_FLAG);
    noreply = fh.flag & NOREPLY_FLAG;
    move (3, 0);
    memcpy (&newfh, &fh, sizeof (newfh));
    prints ("����������:     %s\n", fh.filename);
    prints ("������˵��:     %s\n", fh.title);
    prints ("����������Ա:   %s\n", fh.BM);
    prints ("������������:   %s\n", (noidboard) ? "Yes" : "No");
    prints ("���²����Իظ�: %s\n", (noreply) ? "Yes" : "No");
    strcpy (oldtitle, fh.title);
    prints ("���� %s Ȩ��: %s", (fh.level & PERM_POSTMASK) ? "POST" :
            (fh.level & PERM_NOZAP) ? "ZAP" : "READ",
            (fh.level & ~PERM_POSTMASK) == 0 ? "������" : "������");
    move (8, 0);
    if (askyn ("�Ƿ����������Ѷ", NA, NA) == NA)
    {
        clear ();
        return 0;
    }
    move (9, 0);
    prints ("ֱ�Ӱ� <Return> ���޸Ĵ�����Ѷ...");
    while (1)
    {
        getdata (10, 0, "������������: ", genbuf, BDNAME_LEN, DOECHO, YEA);
        if (*genbuf != 0)
        {
            struct boardheader dh;

            if (search_record (BOARDS, &dh, sizeof (dh), cmpbnames, genbuf))
            {
                move (2, 0);
                prints ("����! ���������Ѿ�����!!");
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
                prints ("���Ϸ�������������!");
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
        getdata (18, 0, "��������˵��: ", genbuf, 60, DOECHO, YEA);
        if (*genbuf != 0)
            strncpy (newfh.title, genbuf, sizeof (newfh.title));
        else
            break;
        if (strstr (newfh.title, "��") || strstr (newfh.title, "��"))
        {
            newfh.flag |= OUT_FLAG;
            break;
        }
        else if (strstr (newfh.title, "��"))
        {
            newfh.flag &= ~OUT_FLAG;
            break;
        }
        else
            prints ("\n����ĸ�ʽ, �޷��ж��Ƿ�ת��!!");
    }
    move (20, 0);
    clrtoeol ();
    move (19, 0);
    if (fh.BM[0] != '\0' && strcmp (fh.BM, "SYSOP"))
    {
        if (askyn ("�޸�����������Ա��ע�⣺ȷ�д���ſ��޸ġ����޸İ��س�",
                   NA, NA) == YEA)
        {
            getdata (19, 0,
                     "����������Ա(��ע�⡿������������Ķ��˴���): ",
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
        if (askyn ("�������������(������ SYSOP ����)?", YEA, NA) == NA)
            strncpy (newfh.BM, "SYSOP", sizeof (newfh.BM));
        else
            strncpy (newfh.BM, "\0", sizeof (newfh.BM));
    }
    sprintf (buf, "�����壨Ĭ�������� (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
    getdata (20, 0, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= ANONY_FLAG;
        else
            newfh.flag &= ~ANONY_FLAG;
    }
    sprintf (buf, "�����壨������Ĭ�ϣ� (Y/N)? [%c]: ", (noidboard) ? 'Y' : 'N');
    getdata (20, 0, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= POSITIVE_ANONY_FLAG;
        else
            newfh.flag &= ~POSITIVE_ANONY_FLAG;
    }
    sprintf (buf, "�����Ƿ� ������ �ظ� (Y/N)? [%c]: ", (noreply) ? 'Y' : 'N');
    getdata (20, 30, buf, genbuf, 4, DOECHO, YEA);
    if (*genbuf == 'y' || *genbuf == 'Y' || *genbuf == 'N' || *genbuf == 'n')
    {
        if (*genbuf == 'y' || *genbuf == 'Y')
            newfh.flag |= NOREPLY_FLAG;
        else
            newfh.flag &= ~NOREPLY_FLAG;
    }
    move (21, 0);
    if (askyn ("�Ƿ��ƶ���������λ��", NA, NA) == YEA)
        a_mv = 2;
    else
        a_mv = 0;
    move (22, 0);
    if (askyn ("�Ƿ���Ĵ�ȡȨ��", NA, NA) == YEA)
    {
        char ans[4];

        sprintf (genbuf, "���� (R)�Ķ� �� (P)���� ���� [%c]: ",
                 (newfh.level & PERM_POSTMASK ? 'P' : 'R'));
        getdata (23, 0, genbuf, ans, 2, DOECHO, YEA);
        if ((newfh.level & PERM_POSTMASK) && (*ans == 'R' || *ans == 'r'))
            newfh.level &= ~PERM_POSTMASK;
        else if (!(newfh.level & PERM_POSTMASK) && (*ans == 'P' || *ans == 'p'))
            newfh.level |= PERM_POSTMASK;
        clear ();
        move (2, 0);
        prints ("�趨 %s '%s' ��������Ȩ��\n",
                newfh.level & PERM_POSTMASK ? "����" : "�Ķ�", newfh.filename);
        newfh.level = setperms (newfh.level, "Ȩ��", NUMPERMS, showperminfo);
        clear ();
        getdata (0, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
    }
    else
    {
        getdata (23, 0, "ȷ��Ҫ������? (Y/N) [N]: ", genbuf, 4, DOECHO, YEA);
    }
    if (*genbuf != 'Y' && *genbuf != 'y')
    {
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "�޸���������%s(%s)", fh.filename, newfh.filename);
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
                    prints ("\n����������ʧ��....\n");
                else
                    prints ("�Ѿ����뾫����...\n");
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
    sprintf (genbuf, "���������� %s ������ --> %s", fh.filename,
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
    stand_title ("���˽���ż�");
    move (1, 0);
    prints ("��������Ѷ���δ mark ���ż�\n");
    if (askyn ("ȷ����", NA, NA) == NA)
    {
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "�������ʹ�����Ѷ��ż���");
        securityreport (secu);
    }

    cleanlog = fopen ("mailclean.log", "w");
    move (3, 0);
    prints ("�����ĵȺ�.\n");
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
    prints ("������! ��¼�� mailclean.log.\n");
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
		prints("Ŀǰ�趨:\n");
		trace_state(otflag, "һ��", ostatb.st_size);
		trace_state(ctflag, "����", cstatb.st_size);
		move(9, 0);
		prints("<1> �л�һ���¼\n");
		prints("<2> �л������¼\n");
		getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);
 
		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "һ���¼ ON";
			} else {
				rename("trace", "trace.old");
				msg = "һ���¼ OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "�����¼ ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "�����¼ OFF";
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

    stand_title ("�趨ʹ����ע������");
    for (;;)
    {
        getdata (1, 0,
                 "(0)�뿪  (1)���ע������  (2)��ѯʹ����ע������: ",
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
//            prints ("\n\nĿǰ������ע������.");
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
//            if (askyn ("�趨������", NA, YEA) == YEA)
//            {
//                securityreport ("�趨ʹ����ע������");
//                scan_register_form (fname, NULL);
//            }
//        }
//    }
    
	//else //�°汾��ע�ᵥ��by rwzmm @ sjtubbs
	if(ans[0] == '1')
	{
		struct stat st;
		int record_ns = 0;
		struct regform record = {0};
		if ((fn = fopen (fname_new, "r")) == NULL)
        {
            prints ("\n\nĿǰ������ע������.");
            pressreturn ();
        }

		else
		{
			stat(fname_new, &st);
			record_ns = st.st_size / sizeof(struct regform);
			if(record_ns == 0)
			{
				prints ("\n\nĿǰ������ע������.");
				pressreturn ();
			}
			else
			{
				move(3,0);
				prints("Ŀǰ������ע������ %d �ݡ�", record_ns);

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
				if (askyn ("�趨������", NA, YEA) == YEA)
				{
					securityreport ("�趨ʹ����ע������");
					scan_new_register_form (fname_new, NULL);
				}
			}
		}
	}
    else
    {
        move (1, 0);
        usercomplete ("������Ҫ��ѯ�Ĵ���: ", uident);
        if (uident[0] != '\0')
            if (!getuser (uident))
            {
                move (2, 0);
                prints ("�����ʹ���ߴ���...");
            }
            else
            {
                sprintf (genbuf, "home/%c/%s/register",
                         toupper (lookupuser.userid[0]), lookupuser.userid);
                if ((fn = fopen (genbuf, "r")) != NULL)
                {
                    prints ("\nע����������:\n\n");
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
                    prints ("\n\n�Ҳ�����/����ע������!!\n");
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
    stand_title ("ɾ��������");
    make_blist ();
    move (1, 0);
    namecomplete ("������������: ", bname);
    if (bname[0] == '\0')
        return 0;
    bid = getbnum (bname);
    if (get_record (BOARDS, &binfo, sizeof (binfo), bid) == -1)
    {
        move (2, 0);
        prints ("����ȷ��������\n");
        pressreturn ();
        clear ();
        return 0;
    }
    if (binfo.BM[0] != '\0' && strcmp (binfo.BM, "SYSOP"))
    {
        move (5, 0);
        prints ("�ð廹�а�������ɾ������ǰ������ȡ��������������\n");
        pressanykey ();
        clear ();
        return 0;
    }
    ans = askyn ("��ȷ��Ҫɾ�����������", NA, NA);
    if (ans != 1)
    {
        move (2, 0);
        prints ("ȡ��ɾ���ж�\n");
        pressreturn ();
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "ɾ����������%s", binfo.filename);
        securityreport (secu);
    }
    if (seek_in_file ("0Announce/.Search", bname))
    {
        move (4, 0);
        if (askyn ("�Ƴ�������", NA, NA) == YEA)
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
    sprintf (genbuf, " << '%s' �� %s ɾ�� >>",
             binfo.filename, currentuser.userid);
    memset (&binfo, 0, sizeof (binfo));
    strncpy (binfo.title, genbuf, STRLEN);
    binfo.level = PERM_SYSOP;
    substitute_record (BOARDS, &binfo, sizeof (binfo), bid);

    move (4, 0);
    prints ("\n���������Ѿ�ɾ��...\n");
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
    stand_title ("ɾ��ʹ�����ʺ�");
    move (2, 0);
    if (!gettheuserid (1, "��������ɾ����ʹ���ߴ���: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "SYSOP"))
    {
        prints ("\n�Բ����㲻����ɾ�� SYSOP �ʺ�!!\n");
        pressreturn ();
        clear ();
        return 0;
    }
    if (!strcmp (lookupuser.userid, currentuser.userid))
    {
        prints ("\n�Բ����㲻����ɾ���Լ�������ʺ�!!\n");
        pressreturn ();
        clear ();
        return 0;
    }
    prints ("\n\n������ [%s] �Ĳ�������:\n", lookupuser.userid);
    prints ("    User ID:  [%s]\n", lookupuser.userid);
    prints ("    ��   ��:  [%s]\n", lookupuser.username);
    prints ("    ��   ��:  [%s]\n", lookupuser.realname);
    strcpy (secu, "bTCPRD#@XWBA#VS-DOM-F0s23456789t");
    for (num = 0; num < strlen (secu); num++)
        if (!(lookupuser.userlevel & (1 << num)))
            secu[num] = '-';
    prints ("    Ȩ   ��: %s\n\n", secu);

    num = getbnames (lookupuser.userid, secu, &num);
    if (num)
    {
        prints ("[%s] Ŀǰ�е����� %d ����İ���: ", lookupuser.userid, num);
        for (i = 0; i < num; i++)
            prints ("%s ", bnames[i]);
        prints ("\n����ʹ�ð���жְ����ȡ�������ְ�������ò���.");
        pressanykey ();
        clear ();
        return 0;
    }

    sprintf (genbuf, "��ȷ��Ҫɾ�� [%s] ��� ID ��", lookupuser.userid);
    if (askyn (genbuf, NA, NA) == NA)
    {
        prints ("\nȡ��ɾ��ʹ����...\n");
        pressreturn ();
        clear ();
        return 0;
    }
    sprintf (secu, "ɾ��ʹ���ߣ�%s", lookupuser.userid);
    securityreport (secu);
    sprintf (genbuf, "mail/%c/%s", toupper (lookupuser.userid[0]),
             lookupuser.userid);
    f_rm (genbuf);
    sprintf (genbuf, "home/%c/%s", toupper (lookupuser.userid[0]),
             lookupuser.userid);
    f_rm (genbuf);
    lookupuser.userlevel = 0;
    strcpy (lookupuser.address, "");
    strcpy (lookupuser.username, "");
    strcpy (lookupuser.realname, "");
    strcpy (lookupuser.termtype, "");
    prints ("\n%s �Ѿ��������...\n", lookupuser.userid);
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
    prints ("����ʹ����Ȩ��\n");
    clrtoeol ();
    move (1, 0);
    usercomplete ("���������ĵ�ʹ�����ʺ�: ", genbuf);
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
    prints ("�趨ʹ���� '%s' ��Ȩ�� \n", genbuf);
    newlevel = setperms (lookupuser.userlevel, "Ȩ��", NUMPERMS, showperminfo);
    move (2, 0);
    if (newlevel == lookupuser.userlevel)
        prints ("ʹ���� '%s' Ȩ��û�б��\n", lookupuser.userid);
    else
    {
        lookupuser.userlevel = newlevel;
        {
            char secu[STRLEN];

            sprintf (secu, "�޸� %s ��Ȩ��", lookupuser.userid);
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
        prints ("ʹ���� '%s' Ȩ���Ѿ��������.\n", lookupuser.userid);
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
    prints ("����ʹ���߻���Ȩ�޻�����\n");
    clrtoeol ();
    move (1, 0);
    usercomplete ("���������ĵ�ʹ�����ʺ�: ", genbuf);
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
    prints ("�趨ʹ���� '%s' �Ļ���Ȩ�޻����� \n\n", genbuf);
    prints ("(1) �����������Ȩ��       (A) �ָ���������Ȩ��\n");
    prints ("(2) ȡ��������վȨ��       (B) �ָ�������վȨ��\n");
    prints ("(3) ��ֹ����������         (C) �ָ�����������Ȩ��\n");
    prints ("(4) ��ֹ������������       (D) �ָ�������������Ȩ��\n");
    prints ("(5) ��ֹ�����˷���Ϣ       (E) �ָ������˷���ϢȨ��\n");
    prints ("(6) �޸� %s ��������\n", genbuf);
    getdata (12, 0, "��������Ĵ���: ", ans, 3, DOECHO, YEA);
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
        prints ("\n ʹ���� '%s' ����Ȩ��������û�б��\n", lookupuser.userid);
        pressreturn ();
        clear ();
        return 0;
    }
    {
        char secu[STRLEN];

        sprintf (secu, "�޸� %s �Ļ���Ȩ�޻�����", lookupuser.userid);
        securityreport (secu);
    }
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    prints ("\n ʹ���� '%s' ����Ȩ�޻������Ѿ��������.\n", lookupuser.userid);
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
          "autopost", "junkboards", "sysops", "expire.ctl", "whatdate",
          "../NOLOGIN", "../NOREGISTER", "../.blockmail", "s_fill",
          "f_fill", "register", "firstlogin", "chatstation",
          "bbsnet.ini", "bbsnet2.ini", "notbackupboards",
          "DISTmaster.ini", "macroboards",  "goodbm", NULL
        };
    static char *explain_file[] =
        { "�����վ������", "��վ����", "��վ��ӭ��", "��վ����",
          "���ñ���¼", "menu.ini", "����ע��� ID", "����ȷ��֮E-Mail",
          "������վ֮λַ",
          "ÿ���Զ����ŵ�", "����POST���İ�", "����������", "��ʱ�����趨��",
          "�������嵥", "��ͣ��½(NOLOGIN)", "��ͣע��(NOREGISTER)",
          "ת�ź�����", "ע��ɹ��ż�", "ע��ʧ���ż�", "���û�ע�᷶��",
          "�û���һ�ε�½����", "���ʻ������嵥", "BBSNET תվ�嵥",
          "BBSNET2 תվ�嵥", "����ɾ�����豸��֮�嵥", "��������",
          "����ʹ�ú궨���","�����������", NULL
        };

    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
    {
        return;
    }
    clear ();
    move (1, 0);
    prints ("����ϵͳ����\n\n");
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
    prints ("[[1;32m%2d[m] �������\n", num + 1);

    getdata (23, 0, "��Ҫ������һ��ϵͳ����: ", ans, 3, DOECHO, YEA);
    ch = atoi (ans);
    if (!isdigit (ans[0]) || ch <= 0 || ch > num || ans[0] == '\n'
            || ans[0] == '\0')
        return;
    ch -= 1;
    sprintf (buf2, "etc/%s", e_file[ch]);
    move (3, 0);
    clrtobot ();
    sprintf (buf, "(E)�༭ (D)ɾ�� %s? [E]: ", explain_file[ch]);
    getdata (3, 0, buf, ans, 2, DOECHO, YEA);
    if (ans[0] == 'D' || ans[0] == 'd')
    {
        sprintf (buf, "��ȷ��Ҫɾ�� %s ���ϵͳ��", explain_file[ch]);
        confirm = askyn (buf, NA, NA);
        if (confirm != 1)
        {
            move (5, 0);
            prints ("ȡ��ɾ���ж�\n");
            pressreturn ();
            clear ();
            return;
        }
        {
            char secu[STRLEN];

            sprintf (secu, "ɾ��ϵͳ������%s", explain_file[ch]);
            securityreport (secu);
        }
        unlink (buf2);
        move (5, 0);
        prints ("%s ��ɾ��\n", explain_file[ch]);
        pressreturn ();
        clear ();
        return;
    }
    modify_user_mode (EDITSFILE);
    aborted = vedit (buf2, NA, YEA);	/* ������ļ�ͷ, �����޸�ͷ����Ϣ */
    clear ();
    if (aborted != -1)
    {
        prints ("%s ���¹�", explain_file[ch]);
        {
            char secu[STRLEN];

            sprintf (secu, "�޸�ϵͳ������%s", explain_file[ch]);
            securityreport (secu);
        }

        if (!strcmp (e_file[ch], "../Welcome"))
        {
            unlink ("Welcome.rec");
            prints ("\nWelcome ��¼������");
        }
        else if (!strcmp (e_file[ch], "whatdate"))
        {
            brdshm->fresh_date = time (0);
            prints ("\n�������嵥 ����");
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
    if (!get_msg ("����ʹ����", buf2, 1))
    {
        return 0;
    }
    if (apply_ulist (dowall) == -1)
    {
        move (2, 0);
        prints ("���Ͽ���һ��\n");
        pressanykey ();
    }
    prints ("\n�Ѿ��㲥���...\n");
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
        prints ("�Բ���ϵͳ����ֻ���� SYSOP �޸ģ�");
        pressanykey ();
        return;
    }
    getdata (2, 0, "�������µ�ϵͳ����(ֱ�ӻس���ȡ��ϵͳ����): ",
             passbuf, 19, NOECHO, YEA);
    if (passbuf[0] == '\0')
    {
        if (askyn ("��ȷ��Ҫȡ��ϵͳ������?", NA, NA) == YEA)
        {
            unlink ("etc/.syspasswd");
            securityreport ("[32mȡ��ϵͳ����[37m");
        }
        return;
    }
    getdata (3, 0, "ȷ���µ�ϵͳ����: ", prepass, 19, NOECHO, YEA);
    if (strcmp (passbuf, prepass))
    {
        move (4, 0);
        prints ("�������벻��ͬ, ȡ���˴��趨.");
        pressanykey ();
        return;
    }
    if ((pass = fopen ("etc/.syspasswd", "w")) == NULL)
    {
        move (4, 0);
        prints ("ϵͳ�����޷��趨....");
        pressanykey ();
        return;
    }
    fprintf (pass, "%s\n", genpasswd (passbuf));
    fclose (pass);
    move (4, 0);
    prints ("ϵͳ�����趨���....");
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
    getdata (1, 0, "������ͨ�а���: ", buf, PASSLEN, NOECHO, YEA);
    if (*buf == '\0' || !checkpasswd (currentuser.passwd, buf))
    {
        prints ("\n\n���Ų���ȷ, ����ִ�С�\n");
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
        prints ("\n\nKey ����ȷ, ����ִ�С�\n");
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
    stand_title ("��������\n");
    clrtoeol ();
    if (!gettheuserid (2, "������������ʹ�����ʺ�: ", &id))
        return 0;
    if (!strcmp (lookupuser.userid, "guest"))
    {
        move (5, 0);
        prints ("�㲻������ guest ������");
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
        prints ("��(%d)%-12s\033[m��", i, GroupsInfo.name[i]);
    }
    num[0] = 0;
    getdata (j, 0, "������һ����(�ο��ұ���ʾ���)��: ", num, 2, DOECHO, NA);

    if (num[0] >= 'a' && num[0] <= 'z')
        num[0] -= 32;		//Сдͳһ����д
    if (num[0] >= 'A' && num[0] <= 'Z')
        num[0] -= 7;		//��ĸͳһ������
    if (num[0] == '\0' || num[0] < '1' || num[0] >= '0' + SectNum)
        return -1;
    if (GroupsInfo.name[num[0] - '0'][0] == '\0')
        return -1;
    counts = WHICHDIST (lookupuser.userid);
    if (counts < 12)
    {
        if(counts == 10)
            prints ("%s �Ѿ��� A ����������.", lookupuser.userid);
        else if(counts == 11)
            prints ("%s �Ѿ��� B ����������.", lookupuser.userid);
        else
            prints ("%s �Ѿ��� %d ����������.", lookupuser.userid, counts);

        pressanykey ();
        clear ();
        return -1;
    }
    if(num[0] > '9')
    {
        num[0] += 7;
        prints ("\n�㽫���� %s Ϊ %s ����������?\n", lookupuser.userid, num);
        num[0] -= 7;
    }
    else
        prints ("\n�㽫���� %s Ϊ %s ����������?\n", lookupuser.userid, num);
    if (askyn ("��ȷ��Ҫ������?", NA, NA) == NA)
    {
        prints ("ȡ����������");
        pressanykey ();
        clear ();
        return -1;
    }
    if (add_to_QZ (lookupuser.userid, num[0] - '0') == -1)
    {
        if(num[0] > '9')
        {
            num[0] += 7;
            prints ("%s ������������������.", num);
            num[0] -= 7;
        }
        else
            prints ("%s ������������������.", num);
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';

    clear ();
    move (8, 0);
    prints ("��������������(������У��� Enter ����)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    lookupuser.userlevel |= PERM_SPECIAL7;
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    sprintf (secu, "��������, ���� %s ����Ȩ��", lookupuser.userid);
    securityreport (secu);
    move (15, 0);
    prints (secu);

    if(num[0] > '9')
    {
        num[0] += 7;
        sprintf (title, "���� %s Ϊ %s ������", lookupuser.userid, num);
        sprintf (msg, "\n\t\t\t�� ͨ�� ��\n\n\t���� %s Ϊ %s ��������\n", lookupuser.userid, num);
        num[0] -= 7;
    }
    else
    {
        sprintf (title, "���� %s Ϊ %s ������", lookupuser.userid, num);
        sprintf (msg, "\n\t\t\t�� ͨ�� ��\n\n\t���� %s Ϊ %s ��������\n", lookupuser.userid, num);

    }
    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (msg, "\n\nվ���������ԣ�\n");
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
    stand_title ("��������\n");
    clrtoeol ();
    if (!gettheuserid (2, "���������ε�ʹ�����ʺ�: ", &id))
        return 0;
    counts = WHICHDIST (lookupuser.userid);
    if (counts == 12)
    {
        prints ("%s �����κ�һ������������.", lookupuser.userid);
        pressanykey ();
        clear ();
        return -1;
    }
    prints ("\n�㽫ȡ�� %s �� ����ְ��\n", lookupuser.userid);
    if (askyn ("��ȷ��Ҫȡ����?", NA, NA) == NA)
    {
        prints ("ȡ����������");
        pressanykey ();
        clear ();
        return -1;
    }
    if (del_from_QZ (lookupuser.userid, counts) != 1)
    {
        prints ("���β�����������,����etc/DISTmaster.ini");
        pressanykey ();
        clear ();
        return -1;
    }
    for (i = 0; i < 5; i++)
        buf[i][0] = '\0';
    clear ();
    move (8, 0);
    prints ("���������θ���(������У��� Enter ����)");
    for (i = 0; i < 5; i++)
    {
        getdata (i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
        if (buf[i][0] == '\0')
            break;
    }
    lookupuser.userlevel &= ~(PERM_SPECIAL7);
    substitute_record (PASSFILE, &lookupuser, sizeof (struct userec), id);
    sprintf (secu, "��������, ȡ�� %s ����Ȩ��", lookupuser.userid);
    securityreport (secu);
    move (15, 0);
    prints (secu);

    if(counts == 10)
    {
        sprintf (title, "A ������ %s ����ͨ��", lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t�� ͨ�� ��\n\n\t��վ�������ۣ�\n\tͬ�� %s ��ȥ A ��������ְ��\n\t�ڴˣ����������� A ��������������ʾ��л��\n\n",
                 lookupuser.userid);
    }

    else if(counts == 11)
    {
        sprintf (title, "B ������ %s ����ͨ��", lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t�� ͨ�� ��\n\n\t��վ�������ۣ�\n\tͬ�� %s ��ȥ B ��������ְ��\n\t�ڴˣ����������� B ��������������ʾ��л��\n\n",
                 lookupuser.userid);
    }


    else
    {

        sprintf (title, "%d ������ %s ����ͨ��", counts, lookupuser.userid);
        sprintf (msg,
                 "\n\t\t\t�� ͨ�� ��\n\n\t��վ�������ۣ�\n\tͬ�� %s ��ȥ %d ��������ְ��\n\t�ڴˣ����������� %d ��������������ʾ��л��\n\n",
                 lookupuser.userid, counts, counts);
    }


    for (i = 0; i < 5; i++)
    {
        if (buf[i][0] == '\0')
            break;
        if (i == 0)
            strcat (msg, "\n\nվ������:\n");
        strcat (msg, buf[i]);
        strcat (msg, "\n");
    }
    strcpy (currboard, "announce");
    autoreport (title, msg, NULL, NULL);
    pressanykey ();
}

//add end 2002.5.26 by mudboy

