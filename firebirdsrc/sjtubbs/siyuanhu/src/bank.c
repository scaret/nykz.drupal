#include "bbs.h"
#include "money.h"

//"bank/rate"

struct salary
{
	char userid[IDLEN + 2];
	int money;
};

int rate_edit()
{
	struct rate bankrate;
	char money[9];
	char sysbuf[STRLEN];
	char mail[1024];
	int fd;

	clear();
	modify_user_mode (QUERY);	
	if(fd=open("bank/rate",O_RDWR)!=-1)
	{
		fd=open("bank/rate",O_RDWR | O_APPEND);		
		lseek(fd,-sizeof(struct rate),SEEK_END);
		read(fd,&bankrate,sizeof(struct rate));
		move( 0, 0);
		sprintf (mail,"ԭ������\n");
		prints("�������ʣ�\n");
		sprintf (sysbuf,"һ�� %8f ���� %8f ���� %8f\n",bankrate.timedepo_01,bankrate.timedepo_02,bankrate.timedepo_03);
		prints(sysbuf);
		strcat (mail,sysbuf);
		sprintf (sysbuf,"���� %8f ���� %8f ���� %8f ���� %8f\n",bankrate.timedepo_04,bankrate.timedepo_05,bankrate.timedepo_06,bankrate.currdepo);
		prints(sysbuf);
		strcat (mail,sysbuf);
	}
	else
	{
		fd=open("bank/rate",O_APPEND | O_WRONLY | O_CREAT ,0644 );
		move( 2, 0);
		prints("����������\n");	
	}
	if (askyn ("Ҫ����ô", NA, NA) == YEA)
	{


		getdata(4, 0, "һ�� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_01 = atof(money);
		}			
		getdata(5, 0, "���� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_02 = atof(money);
		}
		getdata(6, 0, "���� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{	
			bankrate.timedepo_03 = atof(money);
		}
		getdata(7, 0, "���� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_04 = atof(money);
		}
		getdata(8, 0, "���� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{
			bankrate.timedepo_05 = atof(money);
		}
		getdata(9, 0, "���� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{				
			bankrate.timedepo_06 = atof(money);
		}
		getdata(10, 0, "���� :", money, 8, DOECHO, YEA);
		if (money[0] != '\0')
		{				
			bankrate.currdepo= atof(money);
		}
		if (askyn ("ȷ���޸�?", NA, NA) == YEA)
		{
			bankrate.time=time(0);
			write(fd,&bankrate,sizeof(struct rate));//��д����
			close(fd);
			strcat (mail,"\n\n�����������\n");
			sprintf (sysbuf,"һ�� %8f ���� %8f ���� %8f\n",bankrate.timedepo_01,bankrate.timedepo_02,bankrate.timedepo_03);
			strcat (mail,sysbuf);
			sprintf (sysbuf,"���� %8f ���� %8f ���� %8f ���� %8f\n",bankrate.timedepo_04,bankrate.timedepo_05,bankrate.timedepo_06,bankrate.currdepo);
			strcat (mail,sysbuf);	


			move( 12, 0);
			prints("���������Ѿ��޸�\n");
			sprintf(sysbuf, "�������ʸ���");
			securityreport(sysbuf);
			strcpy (currboard, "MoneyAdmin");
			autoreport(sysbuf,mail, NULL, NULL);
			pressanykey();
			return 0;
		}
		else
		{
			close(fd);
			move( 12, 0);
			prints("��������û���޸�\n");

			pressanykey();
			return 0;
		}

	}
}






int salary_read()
{
	char money[6];
	struct stat kkk;
	struct salary sal;
	int size;
	int fd;
	int first;
	char msg[2];
	int opt;
	char ans[2];
	char someoneID[14];
	char buf[80];

	modify_user_mode (QUERY);
	refresh();
	clear();

		move (1,0);
	prints("��ӭ�����г��칫��\n");
	//prints("\n[1]��ѯ��� [2]�������� [3]��ѯ���� [4]��������\n");
	prints("\n[1]�������� [2]�������\n");
	getdata( 4, 0,"���������:", msg, 2, DOECHO, YEA);
	if((msg[0] == '\n') || (msg[0] == '\0'))
	{
	return 0;
	}
	opt = atoi(msg);
	if (!isdigit(msg[0]) || opt <= 0 || opt > 2 || msg[0] == '\n' || msg[0] == '\0')
	{
	move(5,0);
	prints("�Բ���˼Դ�������в��ṩ�������\n");
	pressanykey();
	return 0;
	}
	if(opt == 1)
	{
		clear();
		first=0;

		usercomplete ("�����������ѯ���ʺ�: ", someoneID);

		if (!getuser (someoneID))
		{
			prints ("����ȷ��ʹ���ߴ���\n");
			pressreturn ();
			return;
		}

		fd=open("bank/bank.salary",O_RDWR | O_CREAT, 0644);
		stat("bank/bank.salary",&kkk);
		size=kkk.st_size/sizeof(struct salary);
		while (size>0)
		{
			read(fd,&sal,sizeof(struct salary));
			if(strcmp(sal.userid,someoneID)==0)
			{
				first=1;
				break;
			}
			size--;
		}

		if(first==1)
		{
			move (2,0);

			sprintf(buf,"%s �Ѿ����� Ŀǰ����Ϊ %d \n", sal.userid,sal.money);
			prints (buf);	
			getdata (3, 0, "�Ƿ���ĸ��û�����?[Y/N]: ", ans, 7, DOECHO, YEA);
			if ((*ans != 'Y') && (*ans != 'y'))
			{
				return;
			}
			getdata(4, 0, "�����������Ĺ��� :", money, 6, DOECHO, YEA);
			sal.money = atoi(money);

			if(askyn ("ȷ��", NA, NA) == YEA)
			{
				lseek(fd,-sizeof(struct salary),SEEK_CUR);
				write(fd,&sal,sizeof(struct salary));
			}
			close(fd);
		}
		else
		{
			getdata(2, 0, "������û����� :", money, 6, DOECHO, YEA);
			sal.money = atoi(money);	
			if(askyn ("ȷ��", NA, NA) == YEA)
			{
				sprintf(sal.userid,"%s", someoneID);

				write(fd,&sal,sizeof(struct salary));
			}
			close(fd);

		}
	}	
	if(opt == 2)
	{
		if(askyn ("ȷ��", NA, NA) == YEA)
		{
			unlink("bank/bank.salary");
		}
	}

}

int send_salary()
{

	struct stat kkk;
	struct salary sal;
	int id;
	int size;
	int fd;
	char buf[80];
	char mail_buf[65535];

	refresh();
	clear();
	modify_user_mode (ADMIN);
	if(askyn ("ȷ�����Ź��ʣ�", NA, NA) == YEA)
	{
		fd=open("bank/bank.salary",O_RDONLY );
		stat("bank/bank.salary",&kkk);
		size=kkk.st_size/sizeof(struct salary);
		sprintf(mail_buf,"\n˼Դʵҵ���й��ʷ���ϵͳ "
			"\n����������ϵͳ�Զ�����\n"
			);
		while (size>0)
		{
			read(fd,&sal,sizeof(struct salary));
			id = getuser(sal.userid);
			lookupuser.money+=  sal.money;
			substitute_record(PASSFILE, &lookupuser, sizeof(struct userec),id);
			sprintf(buf,"�����µĹ���Ϊ %d ", sal.money);
			bankreport("����������", buf, lookupuser.userid, NULL);
			sprintf(buf,"%s �Ĺ���Ϊ %d ", lookupuser.userid,sal.money);
			strcat (mail_buf, buf);
			size--;
		}
		close(fd);

		sprintf(buf,"���¹����Ѿ����� by %s ", currentuser.userid);
		securityreport(buf);
		strcpy (currboard, "MoneyAdmin");
		autoreport(buf,mail_buf, NULL, NULL);
	}


}



int count_all()
{
	/*	struct Money mdata;
	struct userec user;	
	struct callbill cBill;
	struct bankbill Bill;
	struct stat kkk;
	int result;
	int fd,fd1;
	long i;
	long size;
	long count;//�ܶ�
	long all_free;//�������ʽ�
	long all_principal;//�ܱ���
	long all_coin;//��Ϸ��
	double all_interest;//��Ϣ
	char temp[1024];
	//char coin_buf[655350];
	char mail_title[STRLEN]={0};
	char mail_buffer[1024];
	//	char someoneID[20];
	char fname[80];
	//	char ans[10];


	clear();
	modify_user_mode (QUERY);
	//��ʼ������
	all_interest=0;
	all_principal=0;
	all_coin=0;
	all_free=0;
	count=0;
	result=0;
	sprintf (mail_title,"%s","˼Դ������״������");
	//sprintf(coin_buf,"��Ϸ�һ���\n\nedit by pax\n\n");
	//����


	//=========================�����ʽ𡢻��ں���Ϸ��ͳ��============================

	fd=open(PASSFILE,O_RDONLY);
	stat(PASSFILE,&kkk);
	size=kkk.st_size/sizeof(struct userec);
	close(fd);
	i=0;
	while(i<size )
	{
	get_record (PASSFILE, &user,sizeof(struct userec), i+1);
	//�����ʽ�
	all_free+=user.money;
	//����ͳ��
	sprintf (fname,"bank/%s.bank", user.userid);
	if(fd1=open(fname,O_RDONLY )!=-1)
	{
	get_record (fname, &cBill,sizeof(struct callbill), 1);
	all_principal+=cBill.principal;//�����ۼ�
	all_interest+=cBill.interest;//��Ϣ�ۼ�
	}	
	//��Ϸ��ͳ��
	//	sprintf (fname, "home/%c/%s/game_money.dat", toupper (user.userid[0]),user.userid);
	sethomefile(fname, user.userid, "game_money.dat");
	if( dashf(fname) )
	{
	get_record (fname, &mdata,sizeof(struct Money), 1);
	all_coin+=(mdata.money);
	sprintf (temp,"\n%-15s%8ld",user.userid,mdata.money);
	//	strcat (coin_buf, temp);
	}

	/*	if(	strcmp(currentuser.userid, "pax")==0)
	{
	user.money += mdata.money;
	mdata.money= 0;
	substitute_record(PASSFILE, &user, sizeof(struct userec),i+1);
	sethomefile(fname, user.userid, "game_money.dat");
	fp = fopen(fname,"w");
	if(fp != NULL)
	{
	result = fwrite(&mdata, sizeof(mdata), 1, fp);
	fclose(fp);
	}
	bankreport("done",fname,"pax",NULL);
	}*
	/
	*		i++;
	}


	//bankreport("coin", coin_buf, currentuser.userid, NULL);
	sprintf (mail_buffer, "����pax\n˼Դ������״��ͳ��\n");
	sprintf (temp,"\n�ܹ��ۼ� �����ʽ� %10ld ",all_free);
	strcat (mail_buffer, temp);
	sprintf (temp,"\n�ܹ��ۼ� ��Ϸ��   %10ld ",all_coin);
	strcat (mail_buffer, temp);
	sprintf (temp,"\n�ܹ��ۼ� ���ڴ�� %10ld ��Ϣ��%10f",all_principal,all_interest);
	strcat (mail_buffer, temp);


	//*=========================����ͳ��============================
	i=0;
	fd=open("bank/bank",O_RDWR);
	stat("bank/bank",&kkk);
	size=kkk.st_size/sizeof(struct bankbill);
	while (i<size)
	{
	read(fd,&Bill,sizeof(struct bankbill));
	if(Bill.dateout==0)
	{
	count=count+Bill.money;
	}			
	i++;
	}
	sprintf (temp,"\n�ܹ��ۼ� ���ڴ�� %10ld",count);
	strcat (mail_buffer, temp);

	sprintf (temp,"\n˼Դ�����ʲ�      %10ld",count+all_principal+all_free);
	strcat (mail_buffer, temp);
	close(fd);	
	bankreport(mail_title, mail_buffer, currentuser.userid, NULL);*/
}

int update_deposit(userid)//�����Ϣ����
char *userid;
{
	char fname[80];
	struct callbill cBill;
	struct rate bankrate;
	int fd;
	double callrate=0.0005;
	if(fd=open("bank/rate",O_RDONLY)!=1)
	{
		fd=open("bank/rate",O_RDONLY);
		lseek(fd,-sizeof(struct rate),SEEK_END);
		read(fd,&bankrate,sizeof(struct rate));	
		callrate=bankrate.currdepo;
		close(fd);
	}
	sprintf (fname,"bank/%s.bank", userid);
	get_record (fname, &cBill,sizeof(struct callbill), 1);
	cBill.interest+=cBill.principal*callrate*(time(0)-cBill.date)/86400;
	cBill.date=time(0);
	substitute_record(fname, &cBill,sizeof(struct callbill), 1);
}


int house_agent_input()
{
	char msg[8];
	char fname[80];
	char mailbuf[256];
	int fd;
	int size;
	struct stat kkk;
	struct house chouse;


	clear();
	modify_user_mode (QUERY);
	usercomplete ("�������������ʺ�: ", chouse.ida);

	if (!getuser (chouse.ida))
	{
		prints ("����ȷ��ʹ���ߴ���\n");
		pressreturn ();
		return;
	}

	if(askyn ("�Ƿ�Ϊ���˾�ס��", YEA, NA) == NA)
	{
		usercomplete ("������Է����ʺ�: ", chouse.idb);
		if (!getuser (chouse.idb))
		{
			prints ("����ȷ��ʹ���ߴ���\n");
			pressreturn ();
			return;
		}
	}

	getdata(3, 0, "���뷿�ӵ���ֵ :", msg, 8, DOECHO, YEA);
	if (msg[0] != '\0')
	{	
		chouse.price=atoi(msg);	
	}
	getdata(3, 0, "���뷿�ӵĺ��� :", msg, 5, DOECHO, YEA);
	if (msg[0] != '\0')
	{	
		strcpy(chouse.num,msg);	
	}
	getdata(3, 0, "���뷿�ӵ����� :", msg, 8, DOECHO, YEA);
	if (msg[0] != '\0')
	{	
		chouse.rate=atof(msg);	
	}

	chouse.datebuy=time(0);
	chouse.interest=0.0;
	chouse.lastpay=time(0);
	chouse.payed=0;

	sprintf (fname,"./.House");
	fd=open(fname,O_WRONLY |O_APPEND | O_CREAT, 0644);	
	write(fd,&chouse,sizeof(struct house));
	stat(fname,&kkk);
	size=kkk.st_size/sizeof(struct house);
	close(fd);

	sprintf(mailbuf,"���ķ������Ϊ %d �������Ϊ %s ����/ ���ڸ����������Ϊ %d/%f", size,chouse.num,chouse.price,chouse.rate);
	strcpy (currboard, "MoneyAdmin");
	//autoreport("���ķ����Ѿ�����",mailbuf,NULL, NULL);
	autoreport("���ķ����Ѿ�����",mailbuf, chouse.ida, NULL);
	sprintf(mailbuf,"�۳����� ���Ϊ %d �������Ϊ %s ������ %s",size,chouse.num,currentuser.userid);
	securityreport(mailbuf);
}


int house_agent_pay()
{
	char fname[80];
	int fd;
	struct house chouse;
	struct stat kkk;
	int size;
	int id;
	int usernum;
	char msg[8];
	int num;

	clear();
	modify_user_mode (QUERY);


	sprintf (fname,"./.House");
	fd=open(fname,O_RDONLY);	
	stat(fname,&kkk);
	size=kkk.st_size/sizeof(struct house);
	close(fd);


	getdata(3, 0, "���뷿�����:", msg, 4, DOECHO, YEA);
	id = atoi(msg);
	if( !isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0' || id<0 || id>size)
	{	
		move(4,0);
		prints("�Բ����������\n");
		pressanykey();
		return 0;
	}


	sprintf (fname,"./.House");
	get_record (fname, &chouse,sizeof(struct house), id);	
	num=chouse.price-chouse.payed;
	if(num==0)
	{
		move(4,0);
		prints(" �����Ѿ����壡\n");
		pressanykey();
		return 0;
	}

	//��Ϣ����


	chouse.interest+=((double)(time(0)-chouse.lastpay)/86400)*chouse.rate*num;

	move(4,0);

	sprintf (fname,"�˷����Ļ���Ϊ %s ,�������Ϊ %s",chouse.ida,chouse.num);
	prints(fname);
	sprintf (fname,"\n�ܲ�ֵΪ %ld ,�Ѿ�֧�� %d ",chouse.price,chouse.payed);
	prints(fname);
	sprintf (fname,"\n����֧�� %ld ,������ϢΪ %8ld ",(int)(num+chouse.interest),(int)chouse.interest);
	prints(fname);

	getdata(10, 0, "����֧����� :", msg, 8, DOECHO, YEA);
	num = (int)atoi(msg);
	if( !isdigit(msg[0]) || msg[0] == '\n' || msg[0] == '\0' || num<0 || num>chouse.price-chouse.payed)
	{	
		move(11,0);
		prints("�Բ����������\n");
		pressanykey();
		return 0;
	}
	if(num>currentuser.money)
	{
		move(11,0);
		prints("�Բ��������ֽ��㣡\n");
		pressanykey();
		return 0;
	}



	currentuser.money-=num;

	usernum=getuser(currentuser.userid);

	substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);

	if((int)chouse.interest<num)
	{
		chouse.interest-=(int)chouse.interest;
		chouse.payed+=(num-(int)chouse.interest);
	}		
	else
	{
		chouse.interest-=num;
	}

	chouse.lastpay=time(0);
	sprintf (fname,"./.House");
	substitute_record(fname, &chouse, sizeof(struct house),id);

	sprintf (fname,"%s ֧������ %d ,%s",currentuser.userid,num,chouse.num);
	strcpy (currboard, "MoneyAdmin");
	autoreport(fname,fname, NULL, NULL);
}

/*pax@syb bbs 05-09-29*/
/*ȫվ���*/
int festival_money(){

	struct userec user;	
	char ps[STRLEN];
	char genbuf[255];
	char tmpbname[20];
	struct stat kkk;
	int fd;
	long i;
	long size;
	int money;//�ܶ�
	char fname[80];
	clear();
	modify_user_mode (ADMIN);
	getdata(10, 0, "����������� [1 - 50000] :", fname, 6, DOECHO, YEA);
	money = atoi(fname);
	if (!isdigit(fname[0]) || money <= 0 || money > 50000 || fname[0] == '\n' || fname[0] == '\0')
	{
		move( 12, 0);
		prints("�Բ��𣬺���������� 1 - 50000 ֮�䣡\n");
		pressanykey();
		return 0;
	}
	move (14, 0);
	prints ("�����뷢�����ԭ��(�� Enter ����)");
	getdata (15, 0, ": ", ps, STRLEN - 5, DOECHO, YEA);

	sprintf(genbuf,"\n�װ����û�:\n\n"
				"    �ڴ�ϲ��֮�գ�˼Դ�������и������Ϻ��һ��(%d����)��\n\n\n",money);				
	strcat(genbuf,ps);

	if (askyn ("ȷ��Ҫ��ȫվ�������", YEA, NA) == YEA)
	{
		fd=open(PASSFILE,O_RDONLY);
		stat(PASSFILE,&kkk);
		size=kkk.st_size/sizeof(struct userec);
		close(fd);
		i=1;
		while(i<=size )
		{
			get_record (PASSFILE, &user,sizeof(struct userec), i);
			
			if(((time (0) - user.lastlogin)/3600 < 720 ) && //30����û�з��ʵ�
				((time (0) - user.firstlogin)/3600 > 84))//����7������
			{
				user.money+=money;
				substitute_record(PASSFILE, &user, sizeof(struct userec),i);
				//strcpy (tmpbname, currboard);
				//strcpy (currboard, NULL);
				bankreport("���յ�һ�����",genbuf, user.userid, NULL);
				//strcpy (currboard, tmpbname);
			}
			i++;
		}
		sprintf(fname, "%s ����ȫվ��� %d Ԫ", currentuser.userid, money);
		securityreport(fname);

		move(20,0);
		prints("������ͣ�\n");
		pressanykey();
		return 0;
	}
}
