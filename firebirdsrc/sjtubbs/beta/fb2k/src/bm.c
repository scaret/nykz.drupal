#include "bbs.h"

//next func change by alt@02.04.06 for auto del denyuser
int
addtodeny (char *uident, char *msg, int ischange, int info_type)
{
    char buf[50], strtosave[256], buf2[50], result[256]; //modifided by proto@sjtu 2006.10.29
    int fAutoUndeny = 1;
    time_t nowtime;
    int day, seek;
    char *tmp;
	int district, dist; //added by proto@sjtubbs 2006.10.29
	
    seek = SeekInNameList (uident);
    if (seek && !ischange)
    {
        move (3, 0);
        prints (" %s �Ѿ��������С�", uident);
        pressanykey ();
        return -1;
    }
    if (ischange && !seek)
    {
        move (3, 0);
        prints (" %s ���������С�", uident);
        pressreturn ();
        return -1;
    }
    while (1)
    {
        getdata (3, 0, "����˵��: ", buf, 40, DOECHO, YEA);
        if (killwordsp (buf) == 0)
            continue;
        tmp = buf;
        while (*tmp != '\0')
        {
            if (*tmp == ' ')
                *tmp = '.';
            tmp++;
        }
        break;
    }
    nowtime = time (0);
    getdatestring (nowtime, NA);
    getdata (4, 0,
             "������ʱ��[ȱʡΪ 1 ��, 0 Ϊ����, �ֶ������ʱ��ǰ��'.']: ",
             buf2, 4, DOECHO, YEA);
    if (buf2[0] == '\0')
        day = 1;
	/* modified by rwzmm @ sjtubbs, only SPECIAL7 can manmal deny*/
    else if (buf2[0] == '.')
    {
		if(HAS_PERM(PERM_SPECIAL7))
        {
			fAutoUndeny = 0;
		}
		else
		{
			fAutoUndeny = 1;
		}
		day = atoi (buf2 + 1);
    }
    else
    {
        fAutoUndeny = 1;
        day = atoi (buf2);
    }
    if (day <= 0)
        return -1;
    if (day >= 30)
        day = 30;
    nowtime += day * 86400;
    getdatestring (nowtime, NA);
    sprintf (strtosave, "%-12s %-40s %s%16.16s���", uident, buf,
             fAutoUndeny ? "(a) " : "(n) ", datestring);
// modified by proto@sjtubbs to announce better denypost announcements 2006.10.29
    dist = board_in_which_dist(currboard);
	if ((dist <= 0) || ((currboard[0] == 'D') && (currboard[1] == 'i') && (currboard[2] == 's')))
		sprintf(result, "\n");
	else if ((dist <= 9) && (dist > 0))
	{
		district = dist + '0';
		sprintf(result, 
		        "\t������Դ˷�������飬���� %s ��վ���ż����ߣ�\n"
				"\t���� 3 ������ District%c �����Ͷ�ߡ�\n\n",
		        currentuser.userid, district);
	}
	else if (dist>=10)
	{
		district = dist - 10 + 'A';
		sprintf(result, 
		        "\t������Դ˷�������飬���� %s ��վ���ż����ߣ�\n"
				"\t���� 3 ������ District%c �����Ͷ�ߡ�\n\n",
		        currentuser.userid, district);
	}
	
    if (!ischange)
    {
        if (!fAutoUndeny)
            sprintf (msg,
                     "\n  %s ����: \n\n"
                     "\t�ܱ�Ǹ��֪ͨ�������Ѿ��� %s ��ͣ�� %s ��ġ����ġ�Ȩ�ޡ�\n\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵�ԭ���ǣ� [%s]��\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵������ǣ� [%d] �졣\n"
					 "\t������ [%14.14s] ���� %s ��վ���ż���\n"
					 "\t����ָ����ڸð���ķ���Ȩ�ޡ�\n\n"
                     "%s"
                     "\tлл���Ա�վ��������������ϣ�\n\n",
                     uident, currentuser.userid, currboard, buf, day, datestring, 
					 currentuser.userid, result);
        else
            sprintf (msg,
                     "\n  %s ����: \n\n"
                     "\t�ܱ�Ǹ��֪ͨ�������Ѿ��� %s ��ͣ�� %s ��ġ����ġ�Ȩ�ޡ�\n\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵�ԭ���ǣ� [%s]��\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵������ǣ� [%d] �졣\n"
                     "\t[%d] ���ϵͳ���Զ��ָ����ڸð���ķ���Ȩ�ޡ�\n\n"
					 "%s"
                     "\tлл���Ա�վ��������������ϣ�\n\n",
                     uident, currentuser.userid, currboard, buf, day, day, result);

    }
    else
    {
        if (!fAutoUndeny)
            sprintf (msg,
                     "\n  %s ����: \n\n"
                     "\t�������� %s �屻��ͣ�����ġ�Ȩ�����⣬�ֱ�����£�\n\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵�ԭ���ǣ� [%s]��\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵������ǣ� [%d] �졣\n"
					 "\t������ [%14.14s] ���� %s ��վ���ż���\n"
					 "\t����ָ����ڸð���ķ���Ȩ�ޡ�\n\n"
					 "%s"
                     "\tлл���Ա�վ��������������ϣ�\n\n",
                     uident, currboard, buf, day, datestring, currentuser.userid, result);

        else
            sprintf (msg,
                     "\n  %s ����: \n\n"
                     "\t�������� %s �屻��ͣ�����ġ�Ȩ�����⣬�ֱ�����£�\n\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵�ԭ���ǣ� [%s]��\n"
                     "\t������ͣ�ڸð��淢��Ȩ�޵������ǣ� [%d] �졣\n"
                     "\t[%d] ���ϵͳ���Զ��ָ����ڸð���ķ���Ȩ�ޡ�\n\n"
					 "%s"
                     "\tлл���Ա�վ��������������ϣ�\n\n",
                     uident, currboard, buf, day, day, result);
    }
    if (info_type < 0)
    {
        if (!ischange)
        {
        	if(!fAutoUndeny)
    		{
				sprintf (msg,
						 "\n  %s ����: \n\n"
						 "\t�ܱ�Ǹ��֪ͨ�������Ѿ��� %s ��ͣ��ȫվ���ĵ�Ȩ�ޡ�\n\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵�ԭ���ǣ� [%s]��\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵������ǣ� [%d] �졣\n"
						 "\t������ [%22.22s] ֮���� sysop �巢�������⡣\n\n"
						 "\t������Դ˷�������飬���� %s ��վ���ż����ߣ�\n"
						 "\t���� 3 ������ sysop �����Ͷ�ߡ�\n\n"
						 "\tлл���Ա�վ��������������ϣ�\n\n",
						 uident, currentuser.userid, buf, day, datestring, currentuser.userid);
    		}
			else
			{
				sprintf (msg,
						 "\n  %s ����: \n\n"
						 "\t�ܱ�Ǹ��֪ͨ�������Ѿ��� %s ��ͣ��ȫվ���ĵ�Ȩ�ޡ�\n\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵�ԭ���ǣ� [%s]��\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵������ǣ� [%d] �졣\n"
						 "\t[%d] ���ϵͳ���Զ��ָ�����ȫվ�ķ���Ȩ�ޡ�\n\n"
						 "\t������Դ˷�������飬���� %s ��վ���ż����ߣ�\n"
						 "\t���� 3 ������ sysop �����Ͷ�ߡ�\n\n"
						 "\tлл���Ա�վ��������������ϣ�\n\n",
						 uident, currentuser.userid, buf, day, day, currentuser.userid);
			}
        }
        else
        {
        	if(!fAutoUndeny)
    		{
				sprintf (msg,
						 "\n  %s ����: \n\n"
						 "\t��������ȫվ����Ȩ�ޱ���ͣ�����⣬�ֱ������:\n\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵�ԭ���ǣ� [%s]��\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵������ǣ� [%d] �졣\n"
						 "\t������ [%22.22s] ֮���� sysop �巢�������⡣\n\n"
						 "\t������Դ˷�������飬���� %s ��վ���ż����ߣ�\n"
						 "\t���� 3 ������ sysop �����Ͷ�ߡ�\n\n"
						 "\tлл���Ա�վ��������������ϣ�\n\n",
						 uident, buf, day, datestring, currentuser.userid);
    		}
			else
			{
				sprintf (msg,
						 "\n  %s ����: \n\n"
						 "\t��������ȫվ����Ȩ�ޱ���ͣ�����⣬�ֱ������:\n\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵�ԭ���ǣ� [%s]��\n"
						 "\t������ͣ�ڱ�վ����Ȩ�޵������ǣ� [%d] �졣\n"
						 "\t[%d] ���ϵͳ���Զ��ָ�����ȫվ�ķ���Ȩ�ޡ�\n\n"
						 "\t������Դ˷�������飬���� %s ��վ���ż����ߣ�\n"
						 "\t���� 3 ������ sysop �����Ͷ�ߡ�\n\n"
						 "\tлл���Ա�վ��������������ϣ�\n\n",
						 uident, buf, day, day, currentuser.userid);
			}
        }
// modify end by proto@sjtubbs 2006.10.29
        sprintf (genbuf, "boards/deny_users_%d", info_type * (-1));
    }
    else
        setbfile (genbuf, currboard, "deny_users");
    if (ischange)
        del_from_file (genbuf, uident);
    return add_to_file (genbuf, strtosave);
}

/* Modify End. */

int
deldeny (char *uident, int info_type)
{
    char fn[STRLEN];

    if (info_type < 0)
        sprintf (fn, "boards/deny_users_%d", info_type * (-1));
    else if (info_type == 3)
        setbfile (fn, currboard, "board.allow");
    else
        setbfile (fn, currboard, "deny_users");
    return del_from_file (fn, uident);
}

#ifdef BOARD_CONTROL
int
control_user (void)
{
    char boardallow[STRLEN];
    int cnt;
	char buf[STRLEN];

    setbfile (boardallow, currboard, "board.allow");
    if (!HAS_PERM (PERM_SYSOP))
    {
        if (!chk_currBM (currBM))
            return DONOTHING;
        if (!dashf (boardallow))
            return DONOTHING;
    }
    while (1)
    {
        cnt = ListFileEdit (boardallow, "�༭�������Ա������", 1);
        if (cnt == 0)
        {
            prints ("\n�����Ա����Ϊ�գ�ϵͳ���Զ�ȡ������ĳ�Ա��������\n");
            if (askyn ("��ȷ��Ҫȡ������ĳ�Ա����������", NA, NA) == YEA)
            {
                unlink (boardallow);
				//add by rwzmm @ sjtubbs, security report of board control
				sprintf(buf, "ɾ�� %s ������Ա����", currboard);
				securityreport(buf);
                break;
            }
        }
        else
		{
			sprintf(buf, "�༭ %s ������Ա����", currboard);
			securityreport (buf);
            break;
		}

    }
    return FULLUPDATE;
}
#endif
int
deny_user (void)
{
    char uident[STRLEN], ans[8], repbuf[STRLEN];
    char msgbuf[512]; //modified by proto@sjtubbs 2006.10.29
    int count, info_type;
    //added by hongliang on Nov. 23th, 2003 for display multiple pages of deny_users
    static int	currentpage;
    static char	tmpbname[STRLEN];

    if ( strcmp(tmpbname, currBM) )
    {
        //The current board is not the last board, so we want the beginning of the deny_users list
        currentpage = 0;
        strcpy(tmpbname, currBM);
    }

    if (!chk_currBM (currBM))
        return DONOTHING;
	if(digestmode != 0)
		return DONOTHING;
    clear ();
    /*noted by pax ����7վҪ�󣬰������������������ 2006-05-17
	if (check_anonyboard (currboard) == YEA && !HAS_PERM (PERM_SYSOP))
    {
        prints ("\n�����������֪�����������, �ʲ�Ӧ֪��������˭��������\n");
        prints ("����趨�޷� Post ������֮Ȩ�޲����Ÿ�����ʹ��\n");
        prints ("������Ҫ�趨��������, ����֪ͨsysop����\n");
        pressreturn ();
        clear ();
        return FULLUPDATE;
    }
	*/
    info_type = 1;
    while (1)
    {
        clear ();
        prints ("�趨����� [��ֹ����] ����\n");
        setbfile (genbuf, currboard, "deny_users");
        //modified by hongliang
        count = listfilecontent2 (genbuf, 3, currentpage);
        if (count)
        {
            getdata (1, 0,
                     "(A)���� (D)ɾ�� (C)�ı� (R)ȫ��ɾ�� [J] ��һҳ [K] ��һҳ or (E)�뿪[E]: ",
                     ans, 7, DOECHO, YEA);
        }
        else
        {
            getdata (1, 0, "(A)���� (R)ɾ������ [J] ��һҳ [K] ��һҳ or (E)�뿪 [E]: ",
                     ans, 7, DOECHO, YEA);
        }
        if (*ans == 'A' || *ans == 'a')
        {
            move (2, 0);
            usercomplete ("����׼������������ʹ����ID: ", uident);
            if (*uident != '\0' && getuser (uident))
            {
                if (addtodeny (uident, msgbuf, 0, info_type) == 1)
                {
                    sprintf (repbuf, "%s����ͣ��%s��ķ���Ȩ��",
                             uident, currboard);
                    securityreport (repbuf);
                    if (msgbuf[0] != '\0')
                    {
#ifdef DENY_POST_BOARDNAME	//added by aegis@010930
                      if(strncmp(currboard, "BMTraining", 10) == 0)
					  {
						autoreport (repbuf, msgbuf, uident,TRAIN_ANNOUNCE_BOARDNAME);
					  }
					  else
					  {
						autoreport (repbuf, msgbuf, uident,DENY_POST_BOARDNAME);
					  }
					
#else

                        autoreport (repbuf, msgbuf, uident, NULL);
#endif

                    }

                }
            }
        }
        else if ((*ans == 'C' || *ans == 'c'))
        {
            move (2, 0);
            namecomplete ("�����뱻�޸��� ID: ", uident);
            if (*uident != '\0')
            {
                if (addtodeny (uident, msgbuf, 1, info_type) == 1)
                {
                    sprintf (repbuf, "�޸Ķ�%s����ͣ����Ȩ�޵Ĵ���", uident);
                    securityreport (repbuf);
                    if (msgbuf[0] != '\0')
                    {
#ifdef DENY_POST_BOARDNAME	//added by aegis@010930
						if(strncmp(currboard, "BMTraining", 10) == 0)
						{
							autoreport (repbuf, msgbuf, uident, TRAIN_ANNOUNCE_BOARDNAME);
						}
						else
						{
							autoreport (repbuf, msgbuf, uident, DENY_POST_BOARDNAME);
						}
#else

                        autoreport (repbuf, msgbuf, uident, NULL);
#endif

                    }

                }
            }
        }
        else if ((*ans == 'D' || *ans == 'd') && count)
        {
            move (2, 0);
            namecomplete ("�����뱻��ͣ�� ID: ", uident);
            if (uident[0] != '\0' && SeekInNameList (uident))
            {
                if (deldeny (uident, info_type))
                {
                    sprintf (repbuf, "�ָ�%s��%s��ķ���Ȩ��",
                             uident, currboard);
                    securityreport (repbuf);
                    sprintf (msgbuf, "\n  %s ���ѣ�\n\n"
                             "\t����ʱ���ѹ����ָֻ����� [%s] ��ķ�������Ȩ�ޡ�\n\n"
                             "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                             uident, currboard, currentuser.userid);
#ifdef DENY_POST_BOARDNAME	//added by aegis@010930
					if(strncmp(currboard, "BMTraining", 10) == 0)
					{
				    	autoreport (repbuf, msgbuf, uident, TRAIN_ANNOUNCE_BOARDNAME);
					}
                    else
					{
						autoreport (repbuf, msgbuf, uident, DENY_POST_BOARDNAME);
					}
#else

                    autoreport (repbuf, msgbuf, uident, NULL);
#endif

                }
            }
        }
        else if (*ans == 'R' || *ans == 'r')
        {
            setbfile (genbuf, currboard, "deny_users");
            unlink (genbuf);
            sprintf (repbuf, "ɾ�� [%s ���ֹ����]����", currboard);
            securityreport (repbuf);

#ifdef DENY_POST_BOARDNAME	//added by aegis@010930

            sprintf (msgbuf, "\n\n\t\t\t\t\t\t\t\t\t\t\t\t\t\tBy %s",
                     currentuser.userid);
			if (strncmp(currboard, "BMTraining", 10) == 0)
			{
				autoreport (repbuf, msgbuf, NULL, TRAIN_ANNOUNCE_BOARDNAME);
			}
            else
			{
				autoreport (repbuf, msgbuf, NULL, DENY_POST_BOARDNAME);
			}
#endif

            return FULLUPDATE;
        }
        else if ( *ans == 'J' || *ans == 'j' )
        {
            currentpage	+= 20;
        }
        else if ( *ans == 'K' || *ans == 'k' )
        {
            currentpage	-= 20;
        }
        else
            break;
    }
    clear ();
    return FULLUPDATE;
}

int
Rangefunc (int ent, struct fileheader *fileinfo, char *direct)
{
    struct fileheader fhdr;
    char annpath[512];
    char buf[STRLEN], ans[8], info[STRLEN], bname[STRLEN], dbname[STRLEN];
    char items[8][7] =
        { "����", "��ժ", "����RE", "ɾ��", "������", "�ݴ�", "ת��",
          "�ָ�"
        };
    int type, num1, num2, i, max = 8, full = 0;
    int fdr, ssize;
    extern int SR_BMDELFLAG;
    extern char quote_file[120], quote_title[120], quote_board[120];

    //����վ, nextline add by mudboy
    if (digestmode != 0)
        return DONOTHING;
    if (uinfo.mode != READING)
        return DONOTHING;
    if (!chk_currBM (currBM))
        return DONOTHING;
    saveline (t_lines - 1, 0);
    if (strcmp (currboard, "deleted") && strcmp (currboard, "junk"))
        max = 7;
    strcpy (info, "����:");
    for (i = 0; i < max; i++)
    {
        sprintf (buf, " %d)%s", i + 1, items[i]);
        strcat (info, buf);
    }
    strcat (info, " [0]: ");
    getdata (t_lines - 1, 0, info, ans, 2, DOECHO, YEA);
    type = atoi (ans);
    if (type <= 0 || type > max)
    {
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    if ((digestmode && (type == 2 || type == 3 || type == 7 || type == 8))
            || (digestmode > 1 && type != 5 && type != 6))
    {
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    move (t_lines - 1, 0);
    clrtoeol ();
    prints ("���β���: %s", items[type - 1]);
    getdata (t_lines - 1, 20, "��ƪ���±��: ", ans, 6, DOECHO, YEA);
    num1 = atoi (ans);
    if (num1 > 0)
    {
        getdata (t_lines - 1, 40, "ĩƬ���±��: ", ans, 6, DOECHO, YEA);
        num2 = atoi (ans);
    }
    if (num1 <= 0 || num2 <= 0 || num2 <= num1)
    {
        move (t_lines - 1, 60);
        prints ("��������...");
        egetch ();
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    if (type != 7)
    {
        sprintf (info, "���� [%s] ������Χ [ %d -- %d ]��ȷ����",
                 items[type - 1], num1, num2);
        if (askyn (info, NA, YEA) == NA)
        {
            saveline (t_lines - 1, 1);
            return DONOTHING;
        }
    }
    if (type == 5)
    {
        FILE *fn;
		int opt;
		char msg[2];
		char fname[STRLEN];
		getdata(t_lines - 1, 0,"���������˿·��� 1-4(Ĭ��Ϊ��һ˿·):", msg, 2, DOECHO,  YEA);

		if (msg[0] == '\n' || msg[0] == '\0')
		msg[0] = '1';
		opt = atoi(msg)-1;
		if(!isdigit(msg[0]) || opt <0 || opt > 3 )
		{
			presskeyfor ("�������, �밴<Enter>����...",t_lines - 1);
			saveline (t_lines - 1, 1);
			return DONOTHING;
		}
		else
		{
			if(opt==0)
			{
				sprintf(fname,".announcepath");	
			}
			else
			{
				sprintf(fname,".announcepath%d",opt);	
			}	/*end*/

			sethomefile (annpath, currentuser.userid, fname);
			if ((fn = fopen (annpath, "r")) == NULL)
			{
				presskeyfor ("�Բ���, ��û���趨˿·. ������ f �趨˿·.",
							 t_lines - 1);
				saveline (t_lines - 1, 1);
				return DONOTHING;
			}
			fscanf (fn, "%s", annpath);
			fclose (fn);
			if (!dashd (annpath))
			{
				presskeyfor ("���趨��˿·�Ѷ�ʧ, �������� f �趨.", t_lines - 1);
				saveline (t_lines - 1, 1);
				return DONOTHING;
			}
		}
    }
    else if (type == 6)
    {
        if (askyn ("[�����ݴ浵ģʽ: һ��/����] ���þ���ʽ��", YEA, YEA)
                == NA)
            full = 1;
    }
    else if (type == 7)
    {
        clear ();
        prints ("\n\n������������ת�ء�ת�ط�Χ�ǣ�[%d -- %d]\n", num1, num2);
        prints ("��ǰ�����ǣ�[ %s ] \n", currboard);
        if (!get_a_boardname (bname, "������Ҫת��������������: "))
            return FULLUPDATE;
        if (!strcmp (bname, currboard) && uinfo.mode != RMAIL)
        {
            prints ("\n\n�Բ��𣬱��ľ�����Ҫת�صİ����ϣ���������ת�ء�\n");
            pressreturn ();
            clear ();
            return FULLUPDATE;
        }
        if (askyn ("ȷ��Ҫת����", NA, NA) == NA)
            return FULLUPDATE;
    }
    if ((fdr = open (direct, O_RDONLY, 0)) == -1)
    {
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    ssize = sizeof (struct fileheader);
    if (lseek (fdr, (num1 - 1) * ssize, SEEK_SET) == -1)
    {
        close (fdr);
        saveline (t_lines - 1, 1);
        return DONOTHING;
    }
    while (read (fdr, &fhdr, ssize) == ssize)
    {
        if (num1 > num2)
            break;
        switch (type)
        {
        case 1:
            mark_post (num1, &fhdr, direct);
            break;
        case 2:
            digest_post (num1, &fhdr, direct);
            break;
        case 3:
            underline_post (num1, &fhdr, direct);
            break;
        case 4:
            {
                int result;

                SR_BMDELFLAG = YEA;
                result = del_post (num1, &fhdr, direct);
                SR_BMDELFLAG = NA;
                if (result == -1)
                {
                    close (fdr);
                    return DIRCHANGED;
                }
                if (result != DONOTHING)
                {
                    lseek (fdr, (-1) * ssize, SEEK_CUR);
                    num1--;
                    num2--;
                }
                break;
            }
        case 5:
			fhdr.accessed[0] |= FILE_VISIT;  /* ����־��λ */
			substitute_record(direct, &fhdr, sizeof(fhdr), num1);
            a_Import ("0Announce", currboard, &fhdr, annpath, YEA);
            break;
        case 6:
            a_Save ("0Announce", currboard, &fhdr, YEA, full);
            break;
        case 7:
            if (uinfo.mode != RMAIL)
                sprintf (genbuf, "boards/%s/%s/%s", currboard, getfolderstr(fhdr.filename), fhdr.filename);
            else
                sprintf (genbuf, "mail/%c/%c/%s/%s",
                         toupper (currentuser.userid[0]), toupper (currentuser.userid[1]), currentuser.userid,
                         fhdr.filename);
            strncpy (quote_file, genbuf, sizeof (quote_file));
            quote_file[sizeof (quote_file) - 1] = '\0';
            strcpy (quote_title, fhdr.title);
            strcpy (quote_board, currboard);
            strcpy (dbname, currboard);
            strcpy (currboard, bname);
            post_cross ('l', 0);
            strcpy (currboard, dbname);
            break;
        case 8:
            SR_BMDELFLAG = YEA;
            UndeleteArticle (num1, &fhdr, direct);
            SR_BMDELFLAG = NA;
            break;
        }
        num1++;
    }
    close (fdr);
    saveline (t_lines - 1, 1);
    return DIRCHANGED;
}
