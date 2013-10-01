#include "bbs.h"

/*����״̬����*/
int x_urgent_start();
/*����״̬�ر�*/
int x_urgent_stop();

int x_urgent_start()
{
	char buf[80];
    char bname[STRLEN];
	char mailbuf[256];
    int pos = 0;
    struct boardheader fh;
	FILE *fp;


	clear();
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    stand_title ("�������״̬����\n");
    if (!gettheboardname (3, "���������: ", &pos, &fh, bname))
	{
		prints("���治���ڣ�\n");
		pressanykey();
		return;
	}

    setbfile(buf, bname, "board.allow");
    if(!dashf(buf))
    {
		setbfile(buf, bname, "board.reject");
		if(!dashf(buf))
		{
			move(5,10);
			prints("���治�����������");
	        pressanykey();
		    return;
		}
    }

	setbfile(buf, bname, "board.urgent");

	if(dashf(buf))
	{
        move(5,10);
		prints("�Բ��𣬽��������Ѿ�������\n");
		pressanykey();
		return;
	} 


	if(askyn ("ȷ�Ͽ�����", YEA, NA) == YEA)
	{
		fp = fopen(buf,"w+");
		fclose(fp);

		sprintf(mailbuf,"�������״̬���� %s ������ %s",bname,currentuser.userid);
		securityreport(mailbuf);
		pressanykey();
	}

	return 1;
}



int x_urgent_stop()
{
	char buf[80];
	char mailbuf[256];
    char bname[STRLEN];
    int  pos = 0;
    struct boardheader fh;
	FILE *fp;


	clear();
    modify_user_mode (ADMIN);
    if (!check_systempasswd ())
        return;

    clear ();
    stand_title ("�������״̬��ֹ\n");
    if (!gettheboardname (3, "���������: ", &pos, &fh, bname))
	{
		prints("���治���ڣ�\n");
		pressanykey();
		return;
	}
	
	setbfile(buf, bname, "board.urgent");
	if(!dashf(buf))
	{
        move(5,10);
		prints("�Բ��𣬽�������û�п�����\n");
		pressanykey();
		return;
	} 

	if(askyn ("ȷ����ֹ��", YEA, NA) == YEA)
	{
		setbfile(buf, bname, "board.urgent");
		unlink(buf);

		sprintf(mailbuf,"�������״̬��ֹ %s ������ %s",bname,currentuser.userid);
		securityreport(mailbuf);
		pressanykey();
	}
	return 1;
}

