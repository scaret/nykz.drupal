#include "bbs.h"
#include "rpg.h"
#include <math.h>

char host1[20][40], host2[20][40], ip[20][40];

int place_monster[20];
int place_rate[20];
int place_time_start[20];
int place_time_end[20];
int place_block[20];
	
int basic_h_damage[20], counts = 0;
int plus_h_damage[20];
int basic_m_damage[20];
int plus_m_damage[20];
int n_life[20];
int n_magic[20];
int m_type[20];
int m_element[20];

struct  mud_place m_place;
int direct[8];
	
typedef struct {
int year, month, day, hour, weekday;
int leap;	/* the lunar month is a leap month */
} Date;

Date solar, lunar, gan, zhi, gan2, zhi2, lunar2;

char str[] =
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
char datafile[80];
char userid[80] = "unknown.";

char *
n_horoscope (month, day)
char month, day;
{
	char *c_sign[28] = {
		"ţ��","Ů��","����","Σ��","����","����","����",
			"¦��","θ��","����","����","����","����","����",
			"����","����","����","����","����","����","����",
			"����","ص��","����","����","β��","����","����"    
	};
	switch (month)
	{
	case 1:
		if (day < 20)
			return( c_sign[0]);
		else if(day < 29)
			return( c_sign[1]);

		else 
			return( c_sign[2]);
	case 2:
		if (day < 9)
			return( c_sign[2]);
		else if(day < 19)
			return( c_sign[3]);

		else 
			return( c_sign[4]);
	case 3:
		if (day <6)
			return( c_sign[4]);
		else if(day < 21)
			return( c_sign[5]);

		else 
			return( c_sign[6]);
	case 4:        
		if (day < 5)
			return( c_sign[6]);
		else if(day < 20)
			return( c_sign[7]);

		else 
			return( c_sign[8]);
	case 5:
		if (day < 11)
			return( c_sign[9]);
		else if(day < 21)
			return( c_sign[10]);

		else 
			return( c_sign[11]);
	case 6: 
		if (day < 6)
			return( c_sign[11]);
		else if(day < 22)
			return( c_sign[12]);

		else 
			return( c_sign[13]);
	case 7:
		if (day < 8)
			return( c_sign[13]);
		else if(day < 23)
			return( c_sign[14]);

		else 
			return( c_sign[15]); 
	case 8:
		if (day < 2)
			return( c_sign[15]);
		else if(day < 12)
			return( c_sign[16]);
		else  if (day < 23)
			return( c_sign[17]);
		else if(day < 27)
			return( c_sign[18]);


		else 
			return( c_sign[19]);
	case 9:
		if (day < 23)
			return( c_sign[19]);
		else
			return( c_sign[20]);
	case 10:
		if (day < 8)
			return( c_sign[21]);
		else if(day < 24)
			return( c_sign[22]);

		else 
			return( c_sign[23]);
	case 11:

		if (day < 3)
			return( c_sign[23]);
		else if(day < 13)
			return( c_sign[24]);

		else if(day < 23)
			return( c_sign[25]);

		else 
			return( c_sign[26]);
	case 12:
		if (day < 8)
			return( c_sign[26]);
		else if(day < 22)
			return( c_sign[27]);

		else 
			return( c_sign[0]);      
	}

	return ("");
}

int id_detail()
{
	char someoneID[20];
	clear();

	modify_user_mode (QUERY);
	usercomplete ("�����������ѯ���ʺ�: ", someoneID);

	if (!getuser (someoneID))
	{
		prints ("����ȷ��ʹ���ߴ���\n");
		pressreturn ();
		return;
	}
	clear();

	id_detail_show(someoneID);
}


int id_detail_show(char  *someoneID)
{
	struct relative rela;
	//struct rpg u_rpg;
	//struct article n_arit;
	//struct magi n_magi;
	struct gift cgift;
	int size;
	struct stat kkk;
	struct person t_person;
	char fname[80];
	char sub[80];
	int i,j;
	int fd;
	
	i = (lookupuser.gender == 'F') ? 5 : 6;
	sprintf (sub, "[\033[1;3%dm%s�� %s\033[m]\n ", i,horoscope (lookupuser.birthmonth, lookupuser.birthday),n_horoscope (lookupuser.birthmonth, lookupuser.birthday));
	prints(sub);

	if ((strcmp(lookupuser.userid,currentuser.userid)==0)||(strcmp(currentuser.userid, PK_ADMIN_A)==0)||(strcmp(currentuser.userid,PK_ADMIN_B)==0)){
		sethomefile(fname, lookupuser.userid, PK_PERSON_FILE);
		
		if(dashf(fname)){
		get_record (fname, &t_person, sizeof (t_person), 1);
		prints("����%d/%d,����%d/%d,������%d/%d",t_person.LiT,t_person.LiTMax,t_person.LiN,t_person.LiNMax,t_person.LiS,t_person.LiSMax);
		}
	}
		
	/*
	//����	   
	sethomefile(fname, lookupuser.userid, "pk.article");
	if(	fd=open(fname,O_RDONLY)!=-1)
	{
		fd=open(fname,O_RDONLY);
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct article);
		close(fd);	
		i=0;
		{prints ("װ������:");}
		while (i<size)
		{
			get_record (fname, &n_arit, sizeof (struct article), i+1);	
			if((i%4)==0)
			{prints ("\n%s (%s)",n_arit.name,articleposition(n_arit.type));}
			else
			{prints ("%s (%s)",n_arit.name,articleposition(n_arit.type));}
			i++;
		}
	}
	//ħ��		   
	sethomefile(fname, lookupuser.userid, "pk.magic");
	if(fd=open(fname,O_RDONLY)!=-1)
	{
		fd=open(fname,O_RDONLY  );
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct magi);	
		i=0;
		{prints ("\n��������ħ��:");}
		while (i<size)
		{
			read(fd,&n_magi,sizeof(struct magi));
			if((i%6)==0)
			{prints ("\n%s ",n_magi.name);}
			else
			{prints ("%s ",n_magi.name);}
			i++;
		}
		close(fd);
	}*/

	//���ݹ�ϵ��ȡ
	sethomefile(fname, lookupuser.userid, "relative");
	if(	fd=open(fname,O_RDONLY)!=-1)
	{
		fd=open(fname,O_RDONLY  );
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct relative);	
		i=0;
		j=0;
		while (i<size)
		{
			read(fd,&rela,sizeof(struct relative));
			if(strcmp(rela.userid,"") != 0)
			{
				if((j%5)==0)
				{prints ("\n%s��%s ",rela.userid,rela.relation);}
				else
				{prints ("%s��%s ",rela.userid,rela.relation);}
				j++;
			}
			i++;
		}
		close(fd);
	}
	else
	{
		prints ("\nû������");

	}
	//�����ȡ
	sethomefile(fname,lookupuser.userid,"gift.dat");
	if (fd=open(fname,O_RDONLY)!=-1)
	{
		fd=open(fname,O_RDONLY);
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct gift);
		close(fd);
		i=0;
		{prints ("\n��������:");}
		while (i<size)
		{
			get_record (fname, &cgift, sizeof (struct gift), i+1);	
			if((i%3)==0)
			{prints ("\n%-12s (%s��)",cgift.name,cgift.id);}
			else
			{prints ("%-12s (%s��)",cgift.name,cgift.id);}
			i++;
		}
	}


	pressreturn ();
	return;
}

int id_detail_input()
{
	struct relative rela;
	char fname[80];
	int opt;
	char msg[3];
	int fd;
	char *relative_list[24]={
		"����","ĸ��","���","�ܵ�","���","����",
			"�˾�","����","����","����","ֶ��","����",
			"�ɷ�","����","����","Ů��","үү","�⹫","����","����",
			"����","��Ů","����","����Ů"
	};

	//-----------------------------------------------------------------
	clear();
	modify_user_mode (QUERY);
	if((strcmp(currentuser.userid, ARCHIVE_ADMIN_A)==0)||(strcmp(currentuser.userid,ARCHIVE_ADMIN_B)==0))
	{
		usercomplete ("�������ʺ�: ", rela.userid);

		if (!getuser (rela.userid))
		{
			prints ("����ȷ��ʹ���ߴ���\n");
			pressreturn ();
			return;
		}
		sethomefile(fname, lookupuser.userid, "relative");
		usercomplete ("������Է��ʺ�: ", rela.userid);

		if (!getuser (rela.userid))
		{
			prints ("����ȷ��ʹ���ߴ���\n");
			pressreturn ();
			return;
		}
		move(3,0);
		prints("1 �� 2 ĸ 3 �� 4  �� 5 �� 6 �� 7 ��  8 �� 9 �� 10 �� \n11 ֶ 12 �� 13 �� 14 �� 15 �� 16 Ů 17 үү 18 �⹫ 19 ���� 20 ����\n 21 ���� 22 ��Ů 23 ���� 24 ����Ů");
		getdata( 6, 0,"������:", msg, 3, DOECHO,  YEA);
		opt = atoi(msg);
		if (!isdigit(msg[0]) || opt <= 0 || opt > 24 || msg[0] == '\n' || msg[0] == '\0')
		{
			move(7,0);
			prints("�Բ����������!\n");
			pressanykey();
			return 0;
		}

		sprintf (rela.relation, relative_list[opt-1]);
		rela.regonizedate=time(0);	
		fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644 );
		write(fd,&rela,sizeof(struct relative));
		close(fd);}	
	else{
		prints ("������˼Դ�����ݹݳ�������Ȩ������Ϣ!\n");
		pressreturn ();
		return;
	}
}

int id_detail_release()
{
	char szIDA[STRLEN];
	char szIDB[STRLEN];
	struct relative rela;
	char fname[80];
	char buf[255];
	int opt;
	char msg[3];
	int fd;
	struct stat kkk;	
	int size;
	int i;

	//-----------------------------------------------------------------
	clear();
	modify_user_mode (QUERY);
	if((strcmp(currentuser.userid,ARCHIVE_ADMIN_A)!=0)&&(strcmp(currentuser.userid,ARCHIVE_ADMIN_B)!=0))
	{
		prints ("������˼Դ�����ݹݳ�������Ȩ������Ϣ!\n");
		pressreturn ();
		return;
	}

	usercomplete ("�������ʺż�: ", szIDA);

	if (!getuser (szIDA))
	{
		prints ("����ȷ��ʹ���ߴ���\n");
		pressreturn ();
		return;
	}	
	
	usercomplete ("�������ʺ���: ", szIDB);

	if (!getuser (szIDB))
	{
		prints ("����ȷ��ʹ���ߴ���\n");
		pressreturn ();
		return;
	}

	sethomefile(fname, szIDA, "relative");
	if(	fd=open(fname,O_RDONLY)!=-1)
	{
		fd=open(fname,O_RDONLY  );
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct relative);	
		i=0;
		while (i<size)
		{
			read(fd,&rela,sizeof(struct relative));
			if(strcmp(rela.userid,szIDB) == 0)
			{
				prints ("%s �� %s��%s ",szIDA,rela.userid,rela.relation);
			}
			i++;
		}
		close(fd);
	}
	else
	{
		prints ("%s �� %s û�й�ϵ");
		pressreturn ();
		return;
	}


	if(askyn("ȷ��Ҫ�����ϵ��",NA,NA) == NA ) {
		clrtoeol();
		prints("����������");
		pressanykey();
		return 0;
	}


	sethomefile(fname, szIDA, "relative");
	if(	fd=open(fname,O_RDWR)!=-1)
	{
		fd=open(fname,O_RDWR);
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct relative);	
		i=0;
		while (i<size)
		{
			read(fd,&rela,sizeof(struct relative));
			if(strcmp(rela.userid,szIDB) == 0)
			{
				lseek(fd, -sizeof(struct relative), SEEK_CUR);
				strcpy(rela.userid,"");
				write(fd,&rela,sizeof(struct relative));
			}
			i++;
		}
		close(fd);
	}
	else
	{
		prints ("����ʧ��");
		pressreturn ();
		return;
	}

	sethomefile(fname, szIDB, "relative");
	if(	fd=open(fname,O_RDWR)!=-1)
	{
		fd=open(fname,O_RDWR);
		stat(fname,&kkk);
		size=kkk.st_size/sizeof(struct relative);	
		i=0;
		while (i<size)
		{
			read(fd,&rela,sizeof(struct relative));
			if(strcmp(rela.userid,szIDA) == 0)
			{
				lseek(fd, -sizeof(struct relative), SEEK_CUR);
				strcpy(rela.userid,"");
				write(fd,&rela,sizeof(struct relative));
			}
			i++;
		}
		close(fd);
	}
	else
	{
		prints ("����ʧ��");
		pressreturn ();
		return;
	}
	sprintf(fname,"%s ����� %s �� %s �Ĺ�ϵ",currentuser.userid,szIDA,szIDB);
	securityreport(fname);
	
	
	sprintf(buf,"������%s��%s�Ĺ�ϵ�����\n"                           
	            "                    \n"
	            "                  ˼Դ������  ������ %s  ",szIDA,szIDB,currentuser.userid);

	autoreport (fname, buf, szIDA, TRAIN_ANNOUNCE_BOARDNAME);
	autoreport (fname, buf, szIDB, NULL);

	prints ("�����ɹ�");
	pressreturn ();
	return;
}

/*
int
pk_victory (struct user_info *userinfo)
{
	int id, ind,i;
	struct user_info uin;
	struct userec kuinfo;
	char kickuser[40], buffer[40];

	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND)
	{
		strcpy (kickuser, lookupuser.userid);
		id = getuser (kickuser);

		search_record (PASSFILE, &kuinfo, sizeof (kuinfo), cmpuids, kickuser);
		ind = search_ulist (&uin, t_cmpuids, id);
	}
	else
	{
		uin = *userinfo;
		strcpy (kickuser, uin.userid);
		ind = YEA;
	}
	if (!ind || !uin.active || (uin.pid && kill (uin.pid, 0) == -1))
	{
		if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE
			&& uinfo.mode != FRIEND)
		{
			move (3, 0);
			prints ("�û� [%s] ��������", kickuser);
			clrtoeol ();
			pressreturn ();
			clear ();
		}
		return 0;
	}
	if(is_web_user(&uin))
	{
		for(i=0;i<MAXACTIVE;i++)
			if(!strcasecmp(uin.userid,utmpshm->uinfo[i].userid))
				break;
		if(i!=MAXACTIVE)
			kick_web_user(&(utmpshm->uinfo[i]));
	}
	else
		kill (uin.pid, SIGHUP);
	sprintf (buffer, "kicked %s", kickuser);
	report (buffer);
	sprintf (genbuf, "%s (%s)", kuinfo.userid, kuinfo.username);
	log_usies ("KICK ", genbuf);
	move (12, 0);
	if (uinfo.mode != LUSERS && uinfo.mode != OFFLINE && uinfo.mode != FRIEND)
	{
		prints ("�û� [%s] �Ѿ���������վ.\n", kickuser);

	}
	return 1;
}
*/


//�ж��Ƿ���Թ���
int allow_attack()
{
	struct stat kkk;
	struct pkrecord pk_record;
	char fname[80];
	int tmp;
	int fd;
	int size;
	//�ж�˫���Ƿ��ʼ��������
	if(if_authorize()==-1) return 0;

	//�ж϶Է��ϴα������ļ���Ƿ�������С���

	tmp=recover_life();



	if (tmp<MINTIME)
	{
		move(8,0);
		prints("����������̡��޷������Է���\n");
		pressanykey();
		return 0;
	}

	//�ж��Լ������ļ���Ƿ�������С���
	//	sprintf (fname, "home/%c/%s/pk.record", toupper (currentuser.userid[0]),currentuser.userid);	 
	sethomefile(fname, currentuser.userid, "pk.record");
	fd=open(fname,O_RDONLY  );
	stat(fname,&kkk);
	size=kkk.st_size/sizeof(struct pkrecord);	
	get_record (fname, &pk_record, sizeof (struct pkrecord), size);	
	if (time(0)-pk_record.date<MINTIME)
	{
		move(8,0);
		prints("����������δ�ָ����޷������Է���\n");
		pressanykey();
		return 0;
	}
	return 1;
}


//�ж��Ƿ���Թ���
int allow_steal()
{
	struct stat kkk;
	struct pkrecord pk_record;
	char fname[80];
	//int tmp;
	int fd;
	int size;
	/*�ж�˫���Ƿ��ʼ��������*/
	if(if_authorize()==-1) return 0;

	/*�ж϶Է��ϴα������ļ���Ƿ�������С���*/

	/*�ж��Լ������ļ���Ƿ�������С���*/			
	//	sprintf (fname, "home/%c/%s/pk.record", toupper (currentuser.userid[0]),currentuser.userid);	 
	sethomefile(fname, currentuser.userid, "pk.record");
	fd=open(fname,O_RDONLY  );
	stat(fname,&kkk);
	size=kkk.st_size/sizeof(struct pkrecord);	
	get_record (fname, &pk_record, sizeof (struct pkrecord), size);	
	if (time(0)-pk_record.date<MIN_STEAL_TIME)
	{
		move(8,0);
		prints("����������û�ָ���\n");
		pressanykey();
		return 0;
	}
	return 1;
}

//��¼PK���
int 
record_pk_info(int damage,char *ida,char *idb, int mode)
{
	struct pkrecord pk_record;
	char fname[80];
	//int tmp;
	int fd;


	//�����Լ��ļ�¼
	//	sprintf (fname, "home/%c/%s/pk.record", toupper (currentuser.userid[0]),currentuser.userid);	 
	sethomefile(fname, currentuser.userid, "pk.record");
	if(fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644 )==-1)
	{
		return -1;		
	}
	fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644);
	pk_record.damage=damage;
	strcpy (pk_record.id_a,ida);
	strcpy (pk_record.id_b,idb);
	pk_record.mode=mode;
	pk_record.date=time(0);
	write(fd,&pk_record, sizeof (struct pkrecord));
	close(fd);
	//���ĶԷ��ļ�¼
	//sprintf (fname, "home/%c/%s/pk.record", toupper (lookupuser.userid[0]),lookupuser.userid);	 
	sethomefile(fname, lookupuser.userid, "pk.record");
	if(fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644 )==-1)
	{
		return -1;		
	}
	fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644);
	pk_record.damage=damage;
	strcpy (pk_record.id_a,idb);
	strcpy (pk_record.id_b,ida);
	pk_record.mode=mode+1;
	pk_record.date=time(0);
	write(fd,&pk_record, sizeof (struct pkrecord));
	close(fd);
	//����ϵͳ��¼
	sprintf (fname, "./PKRecord");	 
	if(fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644 )==-1)
	{
		return -1;		
	}
	fd=open(fname,O_APPEND | O_WRONLY | O_CREAT ,0644);
	pk_record.damage=damage;
	strcpy (pk_record.id_a,ida);
	strcpy (pk_record.id_b,idb);
	pk_record.mode=mode;
	pk_record.date=time(0);
	write(fd,&pk_record, sizeof (struct pkrecord));
	close(fd);
	if(mode>=60)
		sprintf(fname,"%s͵ȡ%s,͵��%d,%d",currentuser.userid,lookupuser.userid,damage,mode);
	else
		sprintf(fname,"%s����%s,�˺�%d,%d",currentuser.userid,lookupuser.userid,damage,mode);
	strcpy (currboard, "PKAdmin");
	autoreport(fname,fname, NULL, NULL);
	return 1;
}

int
if_block()
{
	/*if(lookuppker.block>(time(0)%100))
	{		
		return 1;
	}else
		return 0;*/return 0;		
}


int 
if_authorize()
{
	char fname[80];
	int fd;
	/*�ж϶Է��Ƿ��ʼ��������*/
	//sprintf (fname, "home/%c/%s/rpg.result", toupper (lookupuser.userid[0]),lookupuser.userid);	 
	sethomefile(fname, lookupuser.userid, "rpg.battle");
	if(fd=open(fname,O_RDONLY  )==-1) 
	{
		move(8,0);
		prints("�Է�����û�г�ʼ�����޷�������\n");
		pressanykey();
		return -1;
	}
	/*�ж��Լ��Ƿ��ʼ��������*/
	//	sprintf (fname, "home/%c/%s/rpg.result", toupper (currentuser.userid[0]),currentuser.userid);	 
	sethomefile(fname, currentuser.userid, "rpg.battle");
	if(fd=open(fname,O_RDONLY  )==-1) 
	{
		move(8,0);
		prints("�������û�г�ʼ�����޷�������\n");
		pressanykey();
		return -1;
	}
}


int 
create_article()
{
/*	int fd;
	int mode;
	char msg[5];
	char fname[80];
	char buf[256];
	struct article n_arit;
	//	struct rpg u_rpg;
	clear();
	refresh();


	modify_user_mode(ARTICLE); 
	mode=0;

	if((strcmp(currentuser.userid, ARTICLE_ADMIN_A)!=0)&&(strcmp(currentuser.userid,ARTICLE_ADMIN_B)!=0))
		return -1;

	usercomplete ("�����봴���ߵ��ʺ�(������SYSOP��Ϊ���б���): ",n_arit.creator);
	if (strcmp(n_arit.creator,"SYSOP")==0)
		mode=1;

	if ((!getuser (n_arit.creator)) )
	{
		prints ("����ȷ��ʹ���ߴ���\n");
		pressreturn ();
		return;
	}
	move(2,0);

	usercomplete ("�����뱦������: ",n_arit.name);
	move(3,0);
	prints ("1 �� 2 �� 3 ذ�� 4 �� 5 ����6 ���� 7 ͷ�� \n 8 ���� 9 ���� 10 ѥ�� 11 ���� 12 ��ָ");
	getdata( 5, 0,"�������������� :", msg, 2, DOECHO,  YEA);
	n_arit.type= atoi(msg);
	if (!isdigit(msg[0]) || n_arit.type <= 0 || n_arit.type > 12 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}
	//	if(n_arit.type==4) lookuppker.arti

	getdata( 5, 0,"���������ӵĹ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.att= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵķ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.def= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵĸ񵲣�������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.block= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵ���־��������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.power= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵ��˺���������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.damage= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵ���������������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.lifemax= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵ�ħ��ֵ��������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.magicmax= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"���������ӵĻ�ϵ�ֿ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.antifire= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}
	getdata( 5, 0,"���������ӵ�ˮϵ�ֿ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.antiwater= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}	

	getdata( 5, 0,"���������ӵ���ϵ�ֿ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.antiearth= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}


	getdata( 5, 0,"���������ӵķ�ϵ�ֿ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.antiwind= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}


	getdata( 5, 0,"���������ӵĹ�ϵ�ֿ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.antilight= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}


	getdata( 5, 0,"���������ӵİ�ϵ�ֿ�����������ձ��������뽫�ٷ�������256:", msg, 4, DOECHO,  YEA);
	n_arit.antidark= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"�����뱦��ĵȼ�:", msg, 3, DOECHO,  YEA);
	n_arit.level= atoi(msg);
	if (!isdigit(msg[0]) || n_arit.level <= 0 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}
	n_arit.exp=0;

	if(mode==0)
	{
		//sprintf (fname, "home/%c/%s/pk.article", toupper (n_arit.creator[0]),n_arit.creator);
		sethomefile(fname, n_arit.creator, "pk.article");
		get_article(n_arit.creator,&n_arit);
	}
	else
	{
		sprintf (fname, "./pk.article");
	}
	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	write(fd,&n_arit,sizeof(n_arit ));
	close(fd);

	sprintf (fname,"%s Ϊ %s ������һ������",currentuser.userid, n_arit.creator);
	sprintf(buf,
		"\n��������: %s	�������� %d"
		"\n���� %d 	���� %d	�� %d	��־ %d"
		"\n�˺� %d	������ %d	ħ��ֵ %d"
		"\n��ϵ�ֿ��� %d"
		"\nˮϵ�ֿ��� %d"
		"\n��ϵ�ֿ��� %d"
		"\n��ϵ�ֿ��� %d"
		"\n��ϵ�ֿ��� %d"
		"\n��ϵ�ֿ��� %d"
		"\n����ĵȼ� %d",n_arit.name,n_arit.type,n_arit.att,n_arit.def,n_arit.block,n_arit.power,n_arit.damage,n_arit.lifemax,n_arit.magicmax,n_arit.antifire,n_arit.antiwater,n_arit.antiearth,n_arit.antiwind,n_arit.antilight,n_arit.antidark,n_arit.level);
	strcpy (currboard, "PKAdmin");
	autoreport(fname,buf, n_arit.creator, NULL);
	securityreport(fname);
*/
}


int
get_article(char *id,struct article *arti)
{
/*	struct rpg u_rpg;//ԭ��������
	struct rpg t_rpg;//��ʱ������
	char fname[80];
	//	sprintf (fname, "home/%c/%s/rpg.result", toupper (id[0]),id);
	sethomefile(fname, id, PK_PERSON_FILE);//��result�ж�ȡ��Ϣ
	get_record (fname, &u_rpg,sizeof(struct rpg), 1);
	sethomefile(fname, id, "rpg.battle");//��battle�ж�ȡ��Ϣ
	get_record (fname, &t_rpg,sizeof(struct rpg), 1);	
	t_rpg.att+=addper(u_rpg.att,arti->att);
	t_rpg.def+=addper(u_rpg.def,arti->def);
	t_rpg.block+=addper(u_rpg.block,arti->block);
	t_rpg.damage+=addper(u_rpg.damage,arti->damage);
	t_rpg.power+=addper(u_rpg.power,arti->power);
	t_rpg.lifemax+=addper(u_rpg.lifemax,arti->lifemax);
	t_rpg.magicmax+=addper(u_rpg.magicmax,arti->magicmax);
	t_rpg.antifire+=arti->antifire;
	t_rpg.antiwater+=arti->antiwater;
	t_rpg.antiwind+=arti->antiwind;
	t_rpg.antiearth+=arti->antiearth;
	t_rpg.antilight+=arti->antilight;
	t_rpg.antidark+=arti->antidark;
	if (arti->type==4)
	{
		t_rpg.arti=	t_rpg.arti | PK_DISTANCE;
	}

	sethomefile(fname, id, "rpg.battle");
	substitute_record(fname, &t_rpg, sizeof(struct rpg),1);*/
}

int 
addper(int a,int b)
{
	if (b>256)
		return ((int)((double)(b-256)/100*a));
	else
		return b;
}

int 
create_magic()
{	
/*	int fd;
	int mode;
	char msg[5];
	char fname[80];
	char buf[256];
	struct magi n_magi;


	if((strcmp(currentuser.userid, MAGIC_ADMIN_A)!=0)&&(strcmp(currentuser.userid,MAGIC_ADMIN_B)!=0))
		return -1;

	clear();
	refresh();
	mode=0;


	modify_user_mode(ARTICLE); 

	usercomplete ("�����봴���ߵ��ʺ�(������SYSOP��Ϊ����ħ��): ",n_magi.creator);
	if (strcmp(n_magi.creator,"SYSOP")==0)
		mode=1;

	if (!getuser (n_magi.creator))
	{
		prints ("����ȷ��ʹ���ߴ���\n");
		pressreturn ();
		return;
	}
	move(3,0);
	usercomplete ("������ħ��������: ",n_magi.name);

	getdata( 5, 0,"������ħ�������� 1 ���� 2 ���� 3 ����:", msg, 2, DOECHO,  YEA);
	n_magi.type= atoi(msg);
	if (!isdigit(msg[0]) || n_magi.type <= 0 || n_magi.type > 3 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"������ħ�������� 1 �� 2 ˮ 3 �� 4 �� 5 ���� 6 �ڰ�:", msg, 2, DOECHO,  YEA);
	n_magi.element= atoi(msg);
	if (!isdigit(msg[0]) || n_magi.element <= 0 || n_magi.element > 6 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"��������������Ļ����˺�/����:", msg, 4, DOECHO,  YEA);
	n_magi.basehpdamage= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}
	getdata( 5, 0,"ÿ��һ�����������ĸ����˺�/����:", msg, 4, DOECHO,  YEA);
	n_magi.plushpdamage= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"�������ħ��ֵ�Ļ����˺�/����:", msg, 4, DOECHO,  YEA);
	n_magi.basempdamage= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"ÿ��һ����ħ��ֵ�ĸ����˺�/����:", msg, 4, DOECHO,  YEA);
	n_magi.plusmpdamage= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"�������ħ��ֵ������:", msg, 4, DOECHO,  YEA);
	n_magi.requestMP= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}

	getdata( 5, 0,"�������������������:", msg, 4, DOECHO,  YEA);
	n_magi.requestHP= atoi(msg);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		move(6,0);
		prints("�Բ����������!\n");
		pressanykey();
		return 0;
	}
	if(mode==0){
		//sprintf (fname, "home/%c/%s/pk.magic", toupper (n_magi.creator[0]),n_magi.creator);
		sethomefile(fname, n_magi.creator, "pk.magic");
	}

	else{
		sprintf (fname, "./pk.magic");
		//sethomefile(fname, n_magi.creator, "pk.magic");
	}

	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	write(fd,&n_magi,sizeof(n_magi));
	close(fd);

	sprintf (fname,"%s Ϊ %s ������һ��ħ��",currentuser.userid, n_magi.creator);
	sprintf(buf,
		"\nħ������: %s	ħ������ %d"
		"\n�������Ļ����˺�/���� %d"
		"\nÿ��һ���ĸ����˺�/���� %d"
		"\n��ħ���Ļ����˺�/���� %d"
		"\nÿ��һ���ĸ����˺�/���� %d"
		"\n�������Ե�ħ�� %d"
		"\n��ħ��ֵ������ %d"
		"\n������ֵ������ %d",
		n_magi.name,
		n_magi.type,n_magi.basehpdamage,n_magi.plushpdamage,
		n_magi.basempdamage,
		n_magi.plusmpdamage,
		n_magi.element,
		n_magi.requestMP,
		n_magi.requestHP);
	strcpy (currboard, "PKAdmin");
	autoreport(fname,buf, n_magi.creator, NULL);
	securityreport(fname);*/
}

/*char *articleposition(int mode)
{
	char *name[]={"��","��","ذ��","��","����","����","ͷ��","����","����","ѥ��","����","��ָ"};
	return(name[mode-1]);
}
*/
int recover_life()
{/*int fd;

int tmp;
char fname[80];
struct pkrecord pk_record;
struct stat kkk;
//	int size;

sethomefile(fname, lookupuser.userid, "pk.record");
fd=open(fname,O_RDONLY);
stat(fname,&kkk);
//size=	
get_record (fname, &pk_record, sizeof (struct pkrecord), kkk.st_size/sizeof(struct pkrecord));	

sethomefile(fname, lookupuser.userid, "rpg.battle");
get_record (fname, &lookuppker, sizeof (struct person), 1);
tmp=time(0)-pk_record.date;
if(lookuppker.life+RECOVER*tmp<lookuppker.lifemax)
{
	lookuppker.life+=RECOVER*tmp;
}
else
{
	lookuppker.life=lookuppker.lifemax;
}
sethomefile(fname, lookupuser.userid, "rpg.battle");
substitute_record(fname, &lookuppker,sizeof(struct person), 1);
return tmp;*/return 0;
}




init_data_magic ()
{
	int size;
	int fd;
	struct stat kkk;
	struct magi magic;
	counts=0;

	sethomefile(datafile, currentuser.userid, "pk.magic");
	if(	fd=open(datafile, O_RDONLY )!=-1)
	{
		fd=open(datafile, O_RDONLY );
		stat(datafile,&kkk);
		size=kkk.st_size/sizeof(struct magi);
		while(counts<size)
		{
			get_record (datafile, &magic,sizeof(struct magi), counts+1);
			strncpy (host1[counts], magic.name, 16);
			//	bankreport(magic.name,magic.name,"pax",NULL);
			//  strncpy (host2[counts], t2, 36);
			//  strncpy (ip[counts], t3, 36);
			basic_h_damage[counts] = magic.basehpdamage;
			plus_h_damage[counts] = magic.plushpdamage;
			basic_m_damage[counts] = magic.basempdamage;
			plus_m_damage[counts] = magic.plusmpdamage;
			n_life[counts] = magic.requestHP;
			n_magic[counts] = magic.requestMP;
			m_type[counts] = magic.type;
			m_element[counts] = magic.element;
			counts++;

		}
		close(fd);
	}

}

sh (int n)
{
	//char	*perp[6]={"ˮ","��","��","��","ʥ","��"};

	static oldn = -1;

	if (n >= counts)
		return;
	if (oldn >= 0)
	{
		locate (oldn);
		printf ("\033[1;32m %c.\033[m%s", str[oldn], host1[oldn]);
	}
	oldn = n;
	printf
		("\033[22;3H\033[1;37m�����˺�(����/����): \033[1;33m%d/%d  \033[22;32H\033[1;37m ÿ�������˺�: \033[1;33m%d/%d  \033[1;37m����: \033[1;33m%d/%d       \r\n",
		basic_h_damage[n] ,plus_h_damage[n],basic_m_damage[n],
		plus_m_damage[n],n_life[n],n_magic[n]);
	printf (
		"\033[23;3H\033[1;37mħ������: \033[1;33m%s  \033[1;37mħ������: \033[1;33m%s    \033[21;1H", 
		magic_type[m_type[n]-1] , perp[m_element[n]-1]);
	locate (n);
	printf ("[%c]\033[1;42m%s\033[m", str[n], host1[n]);
}

show_all_magic ()
{
	int n;

	printf ("\033[H\033[2J\033[m");
	printf

		("��������������������������������\033[1;35m ħ  ��  ��  �� \033[m��������������������������������\r\n");
	for (n = 1; n < 22; n++)
	{
		printf
			("��                                                                            ��\r\n");
	}
	printf
		("��                                                               \033[1;36m��\033[1;33mCtrl+C\033[1;36m�˳�\033[m ��\r\n");
	printf
		("��������������������������������������������������������������������������������");
	printf
		("\033[21;3H����������������������������������������������������������������������������");
	for (n = 0; n < counts; n++)
	{
		locate (n);
		printf ("\033[1;32m %c.\033[m%s", str[n], host1[n]);
	}
}

locate (int n)
{
	int x, y;
	char buf[20];

	if (n >= counts)
		return;
	y = n / 3 + 3;
	x = n % 3 * 25 + 4;

	sprintf (buf, "\033[%d;%dH", y, x);
	printf (buf);
}

int
getch ()
{
	int c, d, e;
	static lastc = 0;

	c = getchar ();
	if (c == 10 && lastc == 13)
		c = getchar ();
	lastc = c;
	if (c != 27)
		return c;
	d = getchar ();
	e = getchar ();
	if (d == 27)
		return 27;
	if (e == 'A')
		return 257;
	if (e == 'B')
		return 258;
	if (e == 'C')
		return 259;
	if (e == 'D')
		return 260;
	return 0;
}


void
QuitTime ()
{
	reset_tty ();
	exit (0);
}

void
SetQuitTime ()
{
	signal (SIGALRM, QuitTime);
	alarm (60);
}

/*
static void
break_check ()
{
	int i, result;
	unsigned char buf[2048];
	struct timeval tv;
	fd_set readfds;

	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO (&readfds);
	FD_SET (0, &readfds);
	result = select (1, &readfds, NULL, NULL, &tv);
	if (FD_ISSET (0, &readfds))
	{
		result = read (0, buf, 80);
		for (i = 0; i < result; i++)
		{
			if (buf[i] == 3 || buf[i] == 4)
			{
				printf ("\r\nBreak\r\n");
				QuitTime (0);
			}
		}
	}
	alarm (1);
}*/


main_loop ()
{
	int p = 0;
	int c, n;

L:
	show_all_magic ();
	sh (p);
	fflush (stdout);
	while (1)
	{
		c = getch ();
		alarm (60);
		if (c == 3 || c == 4 || c == 27 || c < 0)
			break;
		if (c == 257)		//����
		{
			if (p < 3)
			{
				p += 3 * ((counts - 1) / 3 - (p > ((counts - 1) % 3)));
			}
			else
				p -= 3;
		}
		if (c == 258)		//����
		{
			if (p + 3 > counts - 1)
				p %= 3;
			else
				p += 3;
		}
		if (c == 259)		//����
		{
			p++;
			if (p > counts - 1)
				p = 0;
		}
		if (c == 260)		//����
		{
			p--;
			if (p < 0)
				p = counts - 1;
		}
		if (c == 13 || c == 10)
		{
			alarm (0);
			clear();
			magic_attack(p);
			SetQuitTime ();
			goto L;
		}
		for (n = 0; n < counts; n++)
			if (str[n] == c)
				p = n;
		sh (p);
		fflush (stdout);
	}
}
void magic_attack(int i)
{
	return 0;
}
/*
void magic_attack(int i)
{
	struct user_info *pageinfo;
	struct magi pk_magic;
	//    char uident[13];
	//	char msg[3];
	char sub[256];
	char fname[80];
	//	int opt;
	//	int tmp;
	int mode;
	int magic_block;
	int hp_damage;
	int mp_damage;
	double rate;

	//double attack_rate_nation[5]={1,1,1.5,0.5,1.5};
	//double attack_rate_carrer[8]={0.9,0.7,1.3,1.3,1.1,1,1,1.2};
	//1 �� 2 ˮ 3 �� 4 �� 5 ���� 6 �ڰ�
	double magic_re_zz[5][6]={
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.5,0.7},
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.2,1.2,1.2,1.2,1.2,1.2},
		{1.0,1.0,1.0,1.0,0.7,1.5}
	};//"����", "��ʹ", "����", "����","ħ��"

	double magic_re_zy[8][6]={
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{0.9,0.9,0.9,0.9,0.9,0.9},
		{1.2,1.2,1.2,1.2,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.2,1.2},
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.1,1.1,1.1,1.1,1.1,1.1}
	};//"��ʿ","ȭ����","ħ��ʦ","���","������","����","����","����"

	double magic_add_zz[5][6]={
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.5,0.7},
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{0.8,0.8,0.8,0.8,0.8,0.8},
		{1.0,1.0,1.0,1.0,0.7,1.5}
	};//"����", "��ʹ", "����", "����","ħ��"

	double magic_add_zy[8][6]={
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{0.8,0.8,0.8,0.8,0.8,0.8},
		{1.2,1.2,1.2,1.2,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.2,1.2},
		{1.0,1.0,1.0,1.0,1.0,1.0},
		{1.0,1.0,1.0,1.0,1.0,1.1},
		{1.0,1.0,1.0,1.0,1.0,1.1},
		{1.1,1.1,1.1,1.1,1.1,1.1}
	};//"��ʿ","ȭ����","ħ��ʦ","���","������","����","����","����"

	/*
	��������֮��Ĺ�ϵ����
	����                                      ����
	ʥ  ��  ����  ��  ˮ  ��  ľ  ��     ʥ  ��  ����  ��  ˮ  ��  ľ  ��
	ʥ  -   +   +     0   0   0   0   0      ++  -   +     +   +   +   +   +
	��  +   -   0     0   0   0   0   0      -   ++  +     +   +   +   +   +
	����-   -   0     0   0   0   0   0      --  0   0     0   0   -   0   0
	��  -   -   0     -   +   0   0   --     0   0   0     +   ++  0   0   -
	ˮ  -   -   0     --  -   +   0   0      0   0   0     -   +   ++  0   0
	��  -   -   +     0   --  -   +   0      0   0   0     0   -   +   ++  0
	ľ  -   -   0     0   0   --  -   +      0   0   0     0   0   -   +   ++
	��  -   -   0     +   0   0   --  -      0   0   0     ++  0   0   -   +


	* //	����1 �� 2 ˮ 3 �� 4 �� 5 ���� 6 �ڰ�
	//����*perp[6]={"ˮ","��","��","��","ʥ","��"};
	//��Ҫ�޸� 1 ���ջ� 2 ˮ 3 �� 4 �� 5 ���� 6 �ڰ����� 2 ����
	double magic_att[6][6]={
		{1.0,1.5,0.7,1.0,1.0,1.0},
		{1.5,0.7,1.0,1.0,1.0,1.0},
		{1.0,1.0,1.5,0.7,1.0,1.0},
		{1.0,1.0,1.0,1.0,0.7,1.5},
		{1.0,1.0,1.0,1.0,1.5,0.7},
		{0.7,1.0,1.0,1.5,1.0,1.0}

	};	
	double magic_def[6][6]={
		{2.0,1.5,0.7,1.0,1.0,1.0},
		{1.5,0.7,1.0,2.0,1.0,1.0},
		{1.0,2.0,1.5,0.7,1.0,1.0},
		{1.2,1.2,1.2,1.2,0.7,1.5},
		{1.2,1.2,1.2,1.2,1.5,0.7},
		{0.7,1.0,2.0,1.5,1.0,1.0}

	};

	if(!allow_attack())	return;

	sethomefile(fname, lookupuser.userid, "rpg.battle");
	get_record (fname, &lookuppker, sizeof (struct rpg), 1);

	sethomefile(fname, currentuser.userid, "pk.magic");
	get_record (fname, &pk_magic, sizeof (struct magi), i);

	//	tmp=0;

	currentpker.magic-=pk_magic.requestMP;
	currentpker.life-=pk_magic.requestHP;
	sethomefile(fname, lookupuser.userid, "rpg.battle");
	substitute_record(fname, &currentpker, sizeof(currentpker),1);


	//��ȡħ����Ϣ
	sethomefile(fname, currentuser.userid, "pk.magic");
	if(!dashf(fname)) return;



	get_record (fname, &pk_magic, sizeof (pk_magic), i+1);
	//	sprintf (sub, "%d %d %d %d %d ",i,currentpker.power,pk_magic.basehpdamage,currentpker.level,pk_magic.plushpdamage);
	//	bankreport("report",sub,"pax",NULL);
	//	�� 2 ˮ 3 �� 4 �� 5 ���� 6 �ڰ�
	switch (pk_magic.element)
	{
	case 1:
		magic_block=lookuppker.antifire;
		break;
	case 2:
		magic_block=lookuppker.antiwater;
		break;

	case 3:
		magic_block=lookuppker.antiwind;
		break;
	case 4:
		magic_block=lookuppker.antiearth;
		break;
	case 5:
		magic_block=lookuppker.antilight;
		break;
	case 6:
		magic_block=lookuppker.antidark;
		break;
	default:
		magic_block=0;
		break;		
	}
	if(magic_block>100) magic_block=100;

	//���� ���� ְҵ
	rate=magic_add_zz[currentpker.zz][pk_magic.element]*magic_add_zy[currentpker.zy][pk_magic.element]/(magic_re_zz[lookuppker.zz][pk_magic.element]*magic_re_zy[lookuppker.zy][pk_magic.element]);
	rate=(1-(double)magic_block/100);
	//����-ħ������
	rate*=magic_att[currentpker.per][pk_magic.element];
	//����-ħ������
	rate/=magic_def[lookuppker.per][pk_magic.element];
	//�����˺� ħ���˺�

	//	sprintf (sub, "%8f",rate);
	//	bankreport(sub,sub,"pax",NULL);
	hp_damage=rate*((double)currentpker.power/8)*(pk_magic.basehpdamage+currentpker.level*pk_magic.plushpdamage);
	mp_damage=rate*((double)currentpker.power/8)*(pk_magic.basempdamage+currentpker.level*pk_magic.plusmpdamage);
	//hp_damage=mp_damage=0;
	//	rate*=magic_add_zz[currentpker.zz][pk_magic.element]*magic_add_zy[currentpker.zy][pk_magic.element]/(magic_re_zz[lookuppker.zz][pk_magic.element]*magic_re_zy[lookuppker.zy][pk_magic.element]);
	//			rate*=magic_att[currentpker.per][pk_magic.element];
	//			rate/=magic_def[lookuppker.per][pk_magic.element];
	//end
	//	sprintf (sub, "  �˺�%d",hp_damage);
	//	prints (sub);
	sprintf (sub, "  ԭ������%d",lookuppker.life);
	prints (sub);
	//	tmp=(int)rate;
	lookuppker.life-=hp_damage;
	sprintf (sub, "  ����˺�%d",hp_damage);
	prints (sub);		
	sprintf (sub, "  ʣ������%d",lookuppker.life);
	prints (sub); 

	//	sprintf (sub, "\n  �˺�%d",hp_damage);
	//	prints (sub);
	sprintf (sub, "\n  ԭ��ħ��%d",lookuppker.magic);
	prints (sub);
	//	tmp=(int)rate;
	lookuppker.magic-=mp_damage;
	sprintf (sub, "  ����˺�%d",mp_damage);
	prints (sub);		
	sprintf (sub, "  ʣ��ħ��%d",lookuppker.magic);
	prints (sub); 
	//sprintf (fname, "home/%c/%s/rpg.result", toupper (lookupuser.userid[0]),lookupuser.userid);	 
	sethomefile(fname, lookupuser.userid, "rpg.battle");
	substitute_record(fname, &lookuppker, sizeof(lookuppker),1);
	currentpker.magic-=pk_magic.requestMP;
	currentpker.life-=pk_magic.requestHP;
	sethomefile(fname, currentuser.userid, "rpg.battle");
	substitute_record(fname, &currentpker, sizeof(currentpker),1);
	sprintf (fname, "���ܵ� %s ��%s����,��ʧHP/MP %d/%d ��ʣHP/MP %d/%d��", currentuser.userid,pk_magic.name,hp_damage,mp_damage,lookuppker.life,lookuppker.magic);
	mode=10;		
	pageinfo = t_search (lookupuser.userid, NA);
	do_sendmsg (pageinfo, fname, 2, pageinfo->pid);

	if(lookuppker.life<0)
	{	
		pk_victory(lookupuser.userid);

		sprintf (fname,"����%s������!",currentuser.userid);
		bankreport(fname, fname,lookupuser.userid, NULL);
		mode=14;
	}

	record_pk_info(hp_damage,currentuser.userid,lookupuser.userid, mode);
}
*/
int 
Level_Up()
{


}
/*
int array_show(int i){
clear();
refresh();
move(5,i);
prints(">>>��������>");
}

int pk_visible()
{
struct user_info *uin;
int (*fptr) ();
int tuid;
int i, num;
char sub[80];

struct UTMPFILE *utmpshm = NULL;

bankreport(lookupuser.userid,"start","pax",NULL);
resolve_utmp ();
if (!(tuid = searchuser (lookupuser.userid)) || tuid == usernum)
return -1;
sprintf (sub, "%d",tuid);
bankreport(lookupuser.userid,sub,"pax",NULL);
resolve_utmp ();
num = 0;
for (i = 1; i < USHM_SIZE; i++)
{

sprintf (sub, "%d",i);
bankreport(sub,sub,"pax",NULL);

//	*uin = utmpshm->uinfo[i];

bankreport(sub,sub,"pax",NULL);
if ((*fptr) (tuid, uin))
{
bankreport(sub,sub,"pax",NULL);
if (!uin->active || !uin->pid || isreject (uin))
continue;
bankreport(sub,sub,"pax",NULL);

if (uin->invisible)
num=1;

sprintf (sub, "%d",num);
bankreport(lookupuser.userid,sub,"pax",NULL);

}
}
return num;
}
*/




int attack(){
	struct user_info *pageinfo;
	char uident[13];
	//char msg[3];
	//char sub[80];
	char fname[80];
	//int opt;
	int tmp;
	int mode;
	int id;
	//int usernum;
	//	int magic_block;
	//double rate;
	//"����", "��ʹ", "����", "����","ħ��"
	//double attack_rate_nation[5]={1,1,0.8,1.2,1.2};
	//"��ʿ","ȭ����","ħ��ʦ","���","������","����","����","����"
	//double attack_rate_carrer[8]={1.2,1.3,0.7,0.7,0.8,1,1,0.8};
	//Զ�̹���
	//double shoot_rate_nation[5]={1,1,1.2,1,1};
	//double shoot_rate_carrer[8]={1,1,1,1,2,1.3,1,1};

	id = getuser(currentuser.userid);
	clear ();
	modify_user_mode (QUERY);
	move (1, 0);


	//start pk

	move(1,0);
    if( (lookupuser.lastlogout >= lookupuser.lastlogin))
    {
        move (5, 0);
        prints ("�Է�������...");
        pressanykey ();
        return 0;
    }
    if(!allow_attack()) return 0;

    sethomefile(fname, lookupuser.userid, "rpg.battle");
    get_record (fname, &lookuppker, sizeof (struct person), 1);

    tmp=0;
    if(!if_block())
    {
    }
    else
    {
        prints ("��Ĺ������Է��񵲵��ˣ�");
        sprintf (fname, "���ܵ� %s �Ĺ���,�������ɫ�ĸ񵲵��˶Է��Ĺ���", currentuser.userid);
        mode=2;
    }

    pageinfo = t_search (lookupuser.userid, NA);
    do_sendmsg (pageinfo, fname, 2, pageinfo->pid);

    /*if(lookuppker.life<0)
    {
        pk_victory(lookupuser.userid);
        mode=4;
    }*/

    record_pk_info(tmp,currentuser.userid,lookupuser.userid, mode);
	
	pressanykey ();
	return 0;	
}

int rpg_pesonailt()
{
	int fd;
	int year, month, day, hour;
	struct person u_person;
	char fname[STRLEN];


	char ans[3];
	int answ;
	char sub[256];	
	int i;

	clear();
	modify_user_mode(CAL); 
	sethomefile(fname, currentuser.userid, PK_PERSON_FILE);
	if(dashf(fname))
	{
		/*prints ("���Ѿ���ɳ�ʼ��"); 			
		pressanykey ();
		return 0;*/
		StartMud();
		return 0;
	}
		move( 1, 0);
		getdata(5, 0, "�������������ڼ���?[0 - 23] :", ans, 3, DOECHO, YEA);
		hour = atoi(ans);
		if (!isdigit(ans[0]) )
		{
			move( 7, 0);
			prints("�Բ���Сʱ������[0 - 23]֮�䣡\n");
			pressanykey();
			return 0;
		}
		else
		{
			hour=hour/2;
		}
	
    year=currentuser.birthyear+1900;
	month=currentuser.birthmonth;
	day=currentuser.birthday;

	solar.year = year;
	solar.month = month;
	solar.day = day;
	solar.hour = hour;
	Solar2Lunar();
	TianGan[gan2.year]+=100;
	TianGan[gan2.month]+=100;	
	TianGan[gan2.day]+=100;
	TianGan[gan2.hour]+=100;
	i=0;
	for (i = 0; i < 10; i++){
		TianGan[i]+=FenTianGan[i][zhi2.year];
	}
	for (i = 0; i < 10; i++){
		TianGan[i]+=FenTianGan[i][zhi2.month];
	}
	for (i = 0; i < 10; i++){
		TianGan[i]+=FenTianGan[i][zhi2.day];
	}
	for (i = 0; i < 10; i++){
		TianGan[i]+=FenTianGan[i][zhi2.hour];
	}
	for (i = 0; i < 5; i++){
		WuXing[(i+2) % 5]=(TianGan[2*i]+TianGan[2*i+1])*ZhuYue[zhi2.month][i];	
	}
	/*
��Ϊ��ľ	��Ϊ��ľ
��Ϊ����	��Ϊ����
��Ϊ����	��Ϊ����
��Ϊ����	��Ϊ����
��Ϊ��ˮ	��Ϊ��ˮ

*/
/*
�� 134
ľ 0
ˮ 687
�� 394
�� 544
���� 1159
���� 227
���� 120

pax (��ƽ֮��) ����վ 4556 ��  ����[547]��
�� �� ��: [2006��10��30��21:21:01 ����һ] �� [202.158.177.134] ����վһ�Ρ�
Ŀǰ����: [ѶϢ��: (��) ������: (��)] ����ֵ: [125](̫������) ����: [  ]
�����ʽ�: [61878Ԫ] ���д��: [195632Ԫ](��Ԫ��!!) ����ֵ: [24606](��������)��
�� �� ��: [19322](��̳����) ������: [1](�е����) ������: [999]

	*/

u_person.LiTMax=currentuser.numlogins/20;
u_person.LiNMax=(int)sqrt((double)currentuser.stay)/6;
u_person.LiSMax=Max_TiLi;
u_person.LTongMax=Max_TongShuai;
u_person.LWuMax=Max_WuLi;
u_person.LZhiMax=Max_ZhiLi;
u_person.LMeiMax=Max_MeiLi;
u_person.LiT=u_person.LiTMax;
u_person.LiN=u_person.LiNMax;
u_person.LiS=u_person.LiSMax;
u_person.LTong=Max_TongShuai;
u_person.LWu=Max_WuLi;
u_person.LZhi=Max_ZhiLi;
u_person.LMei=Max_MeiLi;
u_person.spe=0;
u_person.block=0;
u_person.J=(int)WuXing[0];
u_person.M=(int)WuXing[2];
u_person.S=(int)WuXing[1];
u_person.H=(int)WuXing[3];
u_person.T=(int)WuXing[4];
u_person.antiJ=(int)WuXing[0];
u_person.antiM=(int)WuXing[2];
u_person.antiS=(int)WuXing[1];
u_person.antiH=(int)WuXing[3];
u_person.antiT=(int)WuXing[4];
u_person.antiMagi=0;
u_person.level=0;
   
   prints ("�� %d\n",u_person.J); 	
   prints ("ľ %d\n",u_person.M); 	
   prints ("ˮ %d\n",u_person.S); 	
   prints ("�� %d\n",u_person.H); 	
   prints ("�� %d\n",u_person.T); 		
   prints ("���� %d\n",u_person.LiNMax); 	
   prints ("���� %d\n",u_person.LiTMax); 
   prints ("���� %d\n",u_person.LiSMax); 	

	sethomefile(fname, currentuser.userid, PK_PERSON_FILE);
	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	write(fd,&u_person,sizeof(struct person));//��д�ļ�
	close(fd);
	pressanykey();
	return 0;
}

int 
modify_pker(){
	int fd;
	char uident[20];
	char msg[5];
	char fname[80];
	char buf[256];
	struct  person m_peroson;
	clear();
	refresh();

	modify_user_mode(ADMIN); 

	if((strcmp(currentuser.userid, ARTICLE_ADMIN_A)!=0)&&(strcmp(currentuser.userid,ARTICLE_ADMIN_B)!=0))
		return -1;
	move(1,0);
	usercomplete ("�����ʺ�: ", uident);
        if (uident[0] == '\0')
            return 0;
        if (!getuser (uident)){
                move (2, 0);
                prints ("�����ʹ���ߴ���...");
				return;
            }

	 sethomefile(fname, lookupuser.userid, PK_PERSON_FILE);
	 if(!dashf(fname))
	   	return -1;
	 
	 get_record (fname, &m_peroson, sizeof (struct person), 1);
	move(4,0);
	prints("��������%d",m_peroson.LiNMax);	   
	getdata( 20, 0,"����ֵ:", msg, 6, DOECHO,  YEA);
	if (!(!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'))
	{
		m_peroson.LiNMax= atoi(msg);
	}
	move(5,0);
	prints("��������%d",m_peroson.LiTMax);	   
	getdata( 20, 0,"����ֵ:", msg, 6, DOECHO,  YEA);
	if (!(!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'))
	{
		m_peroson.LiTMax= atoi(msg);
	}
	move(6,0);
	prints("��������%d",m_peroson.LiSMax);	   
	getdata( 20, 0,"����ֵ:", msg, 6, DOECHO,  YEA);
	if (!(!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'))
	{
		m_peroson.LiSMax= atoi(msg);
	}
	move(7,0);
	prints("����%d",m_peroson.LiN);	   
	getdata( 20, 0,"����ֵ:", msg, 6, DOECHO,  YEA);
	if (!(!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'))
	{
		m_peroson.LiN= atoi(msg);
	}
	move(8,0);
	prints("����%d",m_peroson.LiT);	   
	getdata( 20, 0,"����ֵ:", msg, 6, DOECHO,  YEA);
	if (!(!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'))
	{
		m_peroson.LiT= atoi(msg);
	}
	move(9,0);
	prints("����%d",m_peroson.LiS);	   
	getdata( 20, 0,"����ֵ:", msg, 6, DOECHO,  YEA);
	if (!(!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'))
	{
		m_peroson.LiS= atoi(msg);
	}

	sethomefile(fname, lookupuser.userid, PK_PERSON_FILE);
	unlink(fname);
	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	write(fd,&m_peroson,sizeof(struct person));//��д�ļ�
	close(fd);
	pressanykey();
	return 0;
}

int 
create_Place()
{
/*	int id;
	int fd;
	int mode;
	char msg[5];
	char fname[80];
	char buf[256];
	struct stat kkk;
	//struct  mud_place m_place;
	clear();
	refresh();


	modify_user_mode(ADMIN); 
	mode=0;

	if((strcmp(currentuser.userid, ARTICLE_ADMIN_A)!=0)&&(strcmp(currentuser.userid,ARTICLE_ADMIN_B)!=0))
		return -1;
	getdata( 2, 0,"������ID:", msg, 20, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		id= 0;
	}else{
	id= atoi(msg);
	}
	
	

	getdata( 2, 0,"��������������:", m_place.name_chn, 20, DOECHO,  YEA);
	getdata( 3, 0,"input the English name pls:", m_place.name_eng, 20, DOECHO,  YEA);
	getdata( 4, 0,"input the shortname pls:", m_place.shortName, 10, DOECHO,  YEA);


	getdata( 5, 0,"���붫��ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.E= 0;
	}else{
	m_place.E= atoi(msg);
	}
	getdata( 6, 0,"��������ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.S= 0;
	}else{
	m_place.S= atoi(msg);
	}
	getdata( 7, 0,"��������ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.W= 0;
	}else{
	m_place.W= atoi(msg);
	}
	getdata( 8, 0,"���뱱��ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.N= 0;
	}else{
	m_place.N= atoi(msg);
	}
	getdata( 9, 0,"���붫����ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.ES= 0;
	}else{
	m_place.ES= atoi(msg);
	}
	getdata( 10, 0,"����������ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.WS= 0;
	}else{
	m_place.WS= atoi(msg);
	}
	getdata( 11, 0,"����������ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.WN= 0;
	}else{
	m_place.WN= atoi(msg);
	}
	getdata( 12, 0,"���붫����ص���:", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.EN= 0;
	}else{
	m_place.EN= atoi(msg);
	}
	
	getdata( 13, 0,"���뽨��(1)���ǿյ�(0):", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_place.EN= 0;
	}else{
	m_place.EN= atoi(msg);
	}

	
	
	sprintf (fname, "MUD/%s/%s", FILE_PLACE,FILE_PLACE);

	if (0==id){
		fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
		stat(fname,&kkk);
		m_place.id=kkk.st_size/sizeof(struct mud_place)+1;
		write(fd,&m_place,sizeof(struct mud_place));
		close(fd);
	}else{
		m_place.id=id;
		substitute_record(fname, &m_place, sizeof(struct mud_place),id-1);
	}

	sprintf (fname,"%s ������%s,���%d",currentuser.userid, m_place.name_chn,m_place.id);
	//securityreport(fname);
	
	//sprintf (fname, "  %d,%d",m_place.id,m_place.E);
	//move(20,0);
	//prints (fname);
	pressanykey();
	return 0;*/

}



int 
create_monster()
{
	int fd;
	int mode;
	char msg[10];
	char fname[80];
	char buf[256];
	struct stat kkk;
	struct  mud_monster m_monster;
	clear();
	refresh();


	modify_user_mode(ADMIN); 
	mode=0;

	if((strcmp(currentuser.userid, ARTICLE_ADMIN_A)!=0)&&(strcmp(currentuser.userid,ARTICLE_ADMIN_B)!=0))
		return -1;

	getdata( 2, 0,"��������������:", m_monster.name_chn, 20, DOECHO,  YEA);
	getdata( 3, 0,"input the English name pls:", m_monster.name_eng, 20, DOECHO,  YEA);
	getdata( 4, 0,"input the shortname pls:", m_monster.shortName, 10, DOECHO,  YEA);

	getdata( 5, 0,"��������ֵ", msg, 6, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.health= 0;
	}else{
	m_monster.health= atoi(msg);
	}
	
	getdata( 6, 0,"���빥��", msg, 5, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.att= 0;
	}else{
	m_monster.att= atoi(msg);
	}
	
	getdata( 7, 0,"�������", msg, 5, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.def= 0;
	}else{
	m_monster.def= atoi(msg);
	}
	
	getdata( 8, 0,"����㶨�˺�", msg, 5, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.d_base= 0;
	}else{
	m_monster.d_base= atoi(msg);
	}

	getdata( 9, 0,"����ɸ�Ӹ���", msg, 2, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.d_num= 0;
	}else{
	m_monster.d_num= atoi(msg);
	}

	getdata( 10, 0,"����ɸ������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.d_face= 0;
	}else{
	m_monster.d_face= atoi(msg);
	}

	getdata( 11, 0,"�����ؼ�", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.spe= 0;
	}else{
	m_monster.spe= atoi(msg);
	}

	getdata( 12, 0,"�����", msg,3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.block= 0;
	}else{
	m_monster.block= atoi(msg);
	}

	getdata( 13, 0,"���������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.J= 0;
	}else{
	m_monster.J= atoi(msg);
	}
	

	getdata( 14, 0,"����ľ����", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.M= 0;
	}else{
	m_monster.M= atoi(msg);
	}



	getdata( 15, 0,"����ˮ����", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.S= 0;
	}else{
	m_monster.S= atoi(msg);
	}

	
	getdata( 16, 0,"���������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.H= 0;
	}else{
	m_monster.H= atoi(msg);
	}

	
	getdata( 17, 0,"����������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.T= 0;
	}else{
	m_monster.T= atoi(msg);
	}

	getdata( 13, 40,"�������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.antiJ= 0;
	}else{
	m_monster.antiJ= atoi(msg);
	}
	

	getdata( 14, 40,"����ľ����", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.antiM= 0;
	}else{
	m_monster.antiM= atoi(msg);
	}



	getdata( 15, 40,"����ˮ����", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.antiS= 0;
	}else{
	m_monster.antiS= atoi(msg);
	}

	
	getdata( 16, 40,"�������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.antiH= 0;
	}else{
	m_monster.antiH= atoi(msg);
	}

	
	getdata( 17, 40,"����������", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.antiT= 0;
	}else{
	m_monster.antiT= atoi(msg);
	}
	
	getdata( 18, 0,"���뿹ħ", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.antiMagi= 0;
	}else{
	m_monster.antiMagi= atoi(msg);
	}		
	
	getdata( 19, 0,"����ȼ�", msg, 3, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		m_monster.level= 1;
	}else{
	m_monster.level= atoi(msg);
	}	
	

	sprintf (fname, "MUD/%s/%s", FILE_MONSTER,FILE_MONSTER);

	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	stat(fname,&kkk);
	m_monster.id=kkk.st_size/sizeof(m_monster)+1;		
	write(fd,&m_monster,sizeof(m_monster ));
	close(fd);
	
	sprintf (fname,"%s �����˹���%s,���%d",currentuser.userid, m_monster.name_chn,m_monster.id);
	securityreport(fname);
	pressanykey();
	return 0;

}


int 
create_Place_monster()
{
/*	int fd;
	int mode;
	char msg[10];
	char fname[80];
	//char buf[256];
	struct stat kkk;
	struct  mud_place_monster m_place_monster;
	clear();
	refresh();


	modify_user_mode(ADMIN); 
	mode=0;

	if((strcmp(currentuser.userid, ARTICLE_ADMIN_A)!=0)&&(strcmp(currentuser.userid,ARTICLE_ADMIN_B)!=0))
		return -1;

	getdata( 2, 0,"������ص���", msg, 6, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		 m_place_monster.id_place= 0;
	}else{
	 m_place_monster.id_place= atoi(msg);
	}
	getdata( 2, 0,"�����������", msg, 6, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		 m_place_monster.id_monster= 0;
	}else{
	 m_place_monster.id_monster= atoi(msg);
	}
		getdata( 2, 0,"��������ָ���(ǧ�ֱ�)", msg, 6, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		 m_place_monster.rate= 0;
	}else{
	 m_place_monster.rate= atoi(msg);
	}

	getdata( 2, 0,"��������ʼ����ʱ��(HHMMSS)", msg, 8, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		 m_place_monster.time_start= 0;
	}else{
	 m_place_monster.time_start= atoi(msg);
	}
	getdata( 2, 0,"�������������ʱ��(HHMMSS)", msg, 8, DOECHO,  YEA);
	if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
	{
		 m_place_monster.time_end= 0;
	}else{
	 m_place_monster.time_end= atoi(msg);
	}
	
	m_place_monster.block=0;

	sprintf (fname, "MUD/%s/%s", FILE_MONSTER,FILE_MONSTER);
	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	stat(fname,&kkk);
	m_place_monster.id=kkk.st_size/sizeof(m_place_monster)+1;		
	write(fd,&m_place_monster,sizeof(m_place_monster ));
	close(fd);
	
	sprintf (fname,"%s��%d����˹���%d,���ǵ�%d������",currentuser.userid, m_place_monster.id_place,m_place_monster.id_monster,m_place_monster.id);
	securityreport(fname);
	pressanykey();
	return 0;*/

}


//#define PROMPT "ʱ���Ѿ���ȥ��������\n\a" 

//char *prompt=PROMPT; 
//unsigned int len; 
/*
void prompt_info(int signo) 
{ 
write(STDERR_FILENO,prompt,len); 
} 

void init_sigaction(buf,m_monster,direct_id,i)
char *buf;
struct mud_monster m_monster;
int direct_id;
int i;
{ 
struct sigaction act; 
act.sa_handler=prompt_info; 
act.sa_flags=0; 
sigemptyset(&act.sa_mask); 
sigaction(SIGPROF,&act,NULL); 
} 

void init_time() 
{ 
struct itimerval value; 
value.it_value.tv_sec=2; 
value.it_value.tv_usec=0; 
value.it_interval=value.it_value; 
setitimer(ITIMER_PROF,&value,NULL); 
} */

int StartMud() 
{ 

	int fd,size,place_id,i;
	char fname[80];
	struct stat kkk;
	place_id=1;

	sethomefile(fname, currentuser.userid, PK_PERSON_FILE);		
	if(!dashf(fname)) return -1;
	
	sprintf (fname, "MUD/%s.ini", FILE_PLACE);
	if(!dashf(fname)) return -1;

	
	if(currentpker.LiS<1) return -1;

	
	clear();
	refresh();


	modify_user_mode(ADMIN); 
	
	while(place_id>0){
		i=ShowPlace(place_id);
		place_id=i;
	}
	clear();
	move (18,0);
	prints ("�˳�");
	
	sethomefile(fname, currentuser.userid, PK_PERSON_FILE);
	unlink(fname);
	fd=open(fname,O_RDWR|O_CREAT|O_APPEND, 0644);
	write(fd,&currentpker,sizeof(struct person));//��д�ļ�
	close(fd);
	
	pressreturn();
} 

int 
ShowPlace(int place_id){	
	int fd,size,i,monster_id,hurt,counts,tmp_life;
	char fname[256];
	char buf[256];
    	FILE *fp;
	char msg[8];
	struct stat kkk;
	struct mud_monster m_monster;
	
	clear();
	refresh();

	i=getPlace(place_id);
	
	if (i<1) 
		return 0;
	
	
	//��ʾ�ص�
	i=0;
	sprintf (fname, "");
	if (m_place.E>0) {
		i++;
		sprintf (buf, "%d)����  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.E;
	}
	if (m_place.S>0) {
		i++;
		sprintf (buf, "%d)����  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.S;
	}
	if (m_place.W>0) {
		i++;
		sprintf (buf, "%d)����  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.W;
	}
	if (m_place.N>0) {
		i++;
		sprintf (buf, "%d)����  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.N;
	}
	if (m_place.ES>0) {
		i++;
		sprintf (buf, "%d)������  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.ES;
	}
	if (m_place.WS>0) {
		i++;
		sprintf (buf, "%d)������  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.WS;
	}	
	if (m_place.WN>0) {
		i++;
		sprintf (buf, "%d)������  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.WN;
	}
	if (m_place.EN>0) {
		i++;
		sprintf (buf, "%d)������  ", i);
		strcat (fname, buf);
		direct[i-1]=m_place.EN;
	}
	
	
	//�Ƿ��й���	
		
	monster_id=getMonsterID(m_place.name_eng);
	if (monster_id>0){
		sprintf (buf, "MUD/%s/%s", FILE_MONSTER,FILE_MONSTER);
		get_record (buf, &m_monster, sizeof (m_monster), monster_id);
		sprintf (buf, "���ֹ���%s",m_monster.name_chn);
		strcat (fname, "9)���� 0)�˳�");
	}else{
		sprintf (buf, "��Χ�����ĵġ�");
		strcat (fname, "0)�˳�");
	}
	move (3,0);
	prints (buf);	
		
	tmp_life=m_monster.health;
	while(1){
		if (monster_id>0){
			hurt=0;
			for(counts=0;counts<m_monster.d_num;counts++)	{
				hurt=hurt+(time(0)%m_monster.d_face);
			}

			hurt=hurt+m_monster.d_base;
			currentpker.LiS=currentpker.LiS-hurt;
			move (5,0);
			//sprintf (buf, "%s�������![%d/\033[1;31m%d\033[m]\n",m_monster.name_chn,currentpker.LiSMax,currentpker.LiS);
			sprintf (buf, "%s�������!���ܵ�����%d��\n",m_monster.name_chn,hurt);
			if (currentpker.LiS<1 ){
				currentpker.LiS=0;
				strcat (buf, "�㱻������!");
				prints (buf);
				pressanykey();
				return 0;
			}	
			else{
				prints (buf);
			}		
		}
		
		sprintf (buf, "������%s ��Ŀǰ������%d\nǰ������%s\n",m_place.name_chn,currentpker.LiS,fname);
		move (18,0);
		prints (buf);
		//prints("��ѡ��");
		getdata( 20, 0,"��ѡ��:", msg, 3, DOECHO,  YEA);

		
		if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0')
		{
			move (21,0);
			prints ("�������!");
		}else{
			if ((monster_id>0) && (9==atoi(msg))){
				hurt=currentpker.LiT-m_monster.def;
				m_monster.health=m_monster.health-hurt;
				sprintf(buf, "����%s����![%d/%d]",m_monster.name_chn,tmp_life,m_monster.health);
				move (6,0);		

				if (m_monster.health<1 ){
					m_monster.health=0;
					strcat (buf, "���ﱻɱ����!");
					monster_id=0;
				}	
				prints (buf);	
			}
			else{
				if (0==atoi(msg)) return 0;
				move(21,0);
				i=atoi(msg)-1;	
				return direct[i];
			}
		}
	}
	return 0;
/*
len=strlen(prompt); 
i=init_sigaction(buf,m_monster,i); 
init_time(); 
//while(1); 
//exit(0); */
}

/*
int 
BattleWithMonster(buf,m_monster,i)
char *buf;
struct mud_monster m_monster;
int i;
{
	char fname[256];
	char msg[8];
	
	while(1){
		sprintf (buf, "������%s \nǰ������%s",m_place.name_chn,fname);
		move (18,0);
		prints (buf);
		getdata( 20, 0,"��ѡ��:", msg, 3, DOECHO,  YEA);
		if (!isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0'||atoi(msg)>i)
		{
			move (21,0);
			prints ("�������!");
		}else{
			if (0==atoi(msg)) return 0;
			move(21,0);
			 direct_id=direct[(atoi(msg)-1)];
			return direct_id;
		}
	}
	return 0 ;
		
}
*/
int 
getMonsterID(place_name)
char *place_name;
{
	char msg[10];
	char fname[80];
	int i;
	//char buf[256];
    FILE *fp;
    char t[256], *t1, *t2, *t3, *t4,*t5;
	counts=0;

	
sprintf (fname, "MUD/%s/%s", FILE_PLACE_MONSTER,place_name);
	
    fp = fopen (fname, "r");
    if (fp == NULL)
        return;
    while (fgets (t, 255, fp) && counts <= MAX_MONSTER_PLACE)
    {
        t1 = strtok (t, " \t");
        t2 = strtok (NULL, " \t\n");
        t3 = strtok (NULL, " \t\n");
        t4 = strtok (NULL, " \t\n");
        t5 = strtok (NULL, " \t\n");
        if (t1[0] == '#' || t1 == NULL || t2 == NULL || t3 == NULL|| t4 == NULL)
            continue;
	place_monster[counts]=t1 ? atoi (t1) : 0;
	place_rate[counts]=t2 ? atoi (t2) : 0;
	place_time_start[counts]=t3 ? atoi (t3) : 0;
	place_time_end[counts]=t4 ? atoi (t4) : 0;
	place_block[counts]=t5 ? atoi (t5) : 0;
	
	/*sprintf (fname, "%d/%d", place_monster[counts],place_rate[counts]);
	move (7,0);
	prints (fname);	
		pressreturn ();
	*/
	if (i=MonsterAppear(place_rate[counts] ))
	{
		fclose (fp);
		return place_monster[counts];	}
       counts++;
    }
	
    fclose (fp);
	return 0;
}

int
getPlace(int place_id){
	char fname[256];
    	char t[256], *t1, *t2, *t3, *t4,*t5,*t6,*t7,*t8,*t9,*t10,*t11,*t12;
	int tmp_id,counts;
    	FILE *fp;
	counts=0;

	sprintf (fname, "MUD/%s.ini", FILE_PLACE);
   	fp = fopen (fname, "r");
	 
   	if (fp == NULL)
        return 0;
	
   while (fgets (t, 255, fp) && counts <=MAX_PLACE)
    {
        t1 = strtok (t, " \t");
        t2 = strtok (NULL, " \t\n");
        t3 = strtok (NULL, " \t\n");
        t4 = strtok (NULL, " \t\n");
        t5 = strtok (NULL, " \t\n"); 
        t6 = strtok (NULL, " \t\n");
        t7 = strtok (NULL, " \t\n");
        t8 = strtok (NULL, " \t\n");
        t9 = strtok (NULL, " \t\n");
        t10 = strtok (NULL, " \t\n");
        t11 = strtok (NULL, " \t\n");
        t12 = strtok (NULL, " \t\n");


         if (t1[0] == '#' || t1 == NULL || t2 == NULL || t3 == NULL|| t4 == NULL)
            continue;
	tmp_id=t1 ? atoi (t1) : 0;
	if (place_id==tmp_id )
	{			
		sprintf (m_place.name_chn, "%s",t2);
		sprintf (m_place.name_eng, "%s",t3);
		m_place.E=t4?atoi(t4):0;
		m_place.ES=t5?atoi(t5):0;
		m_place.S=t6?atoi(t6):0;
		m_place.WS=t7?atoi(t7):0;
		m_place.W=t8?atoi(t8):0;
		m_place.WN=t9?atoi(t9):0;
		m_place.N=t10?atoi(t10):0;
		m_place.EN=t11?atoi(t11):0;
		m_place.type=t12?atoi(t12):0;
		fclose (fp);
		return place_id;	
		}
       counts++;
    }
			
	fclose (fp);
	return 0;

}

int 
MonsterAppear(int id_range){
int i,k ,range;
int min,max;
double j;

min=1;
max=1000;
range=max-min;
i=rand();

i=i*range/RAND_MAX;

//i=(int)(j*(double)range);
i+=min;

if (i<id_range){
	return 1;
	}
else{
	return 0;
	}
}
