/* ˼Դ�� �����г����� 2005.07.19 */
#include <string.h>
#include <stdlib.h>
#include <math.h>



#include "bbs.h"
#include "money.h"



//#ifdef ALLOWGAME

#define CHANGE_MAX	1000000

//pax@sjtubbs 05-07
//int  BANK_CURRDEPO_RATE[6]={3,7,11,15,19,24};//�������� 
double  BANK_CURRDEPO_RATE[6]={2.5,5.25,8.5,12.0,16.0,20.0};//�������� 
//2.5?������5.25��������8.25���ĸ���11.5
//5����15�� 6����18.75

double callrate=0.0005;
//int  CALL_RATE��2;//��������
char mail_title[STRLEN]={0};
char mail_buffer[1024] = {0};//pax@sjtubbs 05 07 16
char bankdatetime[20];
//end by pax;





int inGame(void)
{
	FILE *fp;
	char buf[STRLEN];
	extern int NowInGame ;
	
	if (!strcmp("guest", currentuser.userid)) return 1;
	setuserfile(buf,"inGame");
	if(dashf(buf)){
		clear();
		move(12,18);
		prints("�Բ������Ѿ�������һ��������ʹ�ñ�վ����Ϸ���ܡ�\n");
		pressanykey();
		return 1;
	} 
	NowInGame = YEA;
	fp = fopen(buf,"w+");
	fclose(fp);
	return 0;
}

int gotomarket(char *title)
{
	if(inGame()==1) return 1;
	modify_user_mode(MARKET);
	clear();
	set_safe_record();
	move(2,0);
	prints("��ӭ���� [[32m%s[37m]....\n\n",title);
	return 0;
}

void outgame(void)
{
	char buf[STRLEN];
	extern int NowInGame ;
	setuserfile(buf,"inGame");
	if(dashf(buf))unlink(buf);
	NowInGame = NA;
}



int withdraw()
{
	char sysbuf[STRLEN];
	//,uident[13];
	char msg[3];
	char money[7];
	int opt;
	int num_money;
	int id;
	time_t now;	
	//pax
	struct bankbill Bill;
	struct callbill cBill;
	struct rate bankrate ;
	long size;
	struct stat kkk;
	char fname[80];
	int fd;
	int fd_rate;
	char length[3];
	int num_length;//������Ĵ��ʱ�䳤��
	char BillNo[9];
	int num_bill;
	//end by pax*/
	
	FILE *fp;
	struct Money mdata;
	char buf[STRLEN]; 
	int result=0;
	id = getuser(currentuser.userid);
	
	// get game coin
	/*	sethomefile(buf,currentuser.userid,"game_money.dat");
	if(dashf(buf)) update_game_money();
	fp = fopen(buf,"r");
	if(fp==NULL) return 0;
	result = fread(&mdata,sizeof(mdata),1,fp);
	fclose(fp);
	*/
	// modified by pax 05-08-02 
	//û����Ϸ�Ҽ�¼��ID�Զ�������Ϸ���ļ�
	sethomefile(buf,currentuser.userid,"game_money.dat");
	if(dashf(buf)) update_game_money();
	fp = fopen(buf,"r");
	
	if(fp==NULL) 
	{
		mdata.money=0;
	}
	else
	{
		result = fread(&mdata,sizeof(mdata),1,fp);
		fclose(fp);
	}
	
	// get end	
	
	if(gotomarket("˼Դ��������")) 
		return 0;
	prints("��ӭ���� ˼Դ��������\n");
	sprintf(fname,"��ĿǰЯ���ֽ� %d Ԫ�������ʻ����� %d Ԫ����Ϸ�� %ld Ԫ��\n", currentuser.money, currentuser.inbank, mdata.money);

	prints(fname);

		if ((currentuser.inbank<0) || (currentuser.money<0 ))
		{
			move(8,0);
			prints("�Բ������Ľ��������쳣�����ϵͳ����Ա��ϵ��\n");
			pressanykey();
			outgame();
			return 0;
		}
//	prints("\n[1]���ڴ�� [2]����ȡ�� [3]������Ϸ�� [4]������Ϸ�� [5]���ڴ�� [6]����ȡ��\n");
	prints("\n[1]���ڴ�� [2]����ȡ�� [3]������Ϸ�� [4]���ڴ�� [5]����ȡ��\n");
	
	getdata( 8, 0,"���������:", msg, 2, DOECHO, /*NULL,*/ YEA);
	if((msg[0] == '\n') || (msg[0] == '\0'))
	{
		outgame();
		return 0;
	}
	opt = atoi(msg);
//	if (!isdigit(msg[0]) || opt <= 0 || opt > 6 || msg[0] == '\n' || msg[0] == '\0')
	if (!isdigit(msg[0]) || opt <= 0 || opt > 5 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(10,0);
		prints("�Բ���˼Դ�������в��ṩ�������\n");
		pressanykey();
		outgame();
		return 0;
	}
	
	if(opt == 2)
	{
		update_deposit(currentuser.userid);//������Ϣ pax@2005-8-8

		/*�ж��Ƿ����ļ��Ƿ����*/
		sprintf (fname,"bank/%s.bank", currentuser.userid);
		if(fd=open(fname,O_RDWR )==-1)
		{
			prints("�Բ�������δ��ͨ���ڴ���ʻ�\n");
			pressanykey();
			outgame();
			return 0;		
		}
	    get_record (fname, &cBill,sizeof(struct callbill), 1);	//modified
		move( 9, 0);

		/*��ʾ������ȡ���ʽ��Լ���Ϣ*/
		sprintf (mail_buffer,"���ڿ���ȡ��ʵ�ʽ�%8d(������Ϣ��%8d)\n",(int)(cBill.principal+cBill.interest),(int)cBill.interest);
		prints (mail_buffer);
		
		/*�ж�ȡ�����Ŀ�ͺϷ���*/
		getdata(10, 0, "��������Ҫȡ��Ľ�� [1 - 50000] (����Ϊ����):", money, 6, DOECHO, YEA);
		num_money = (int)atoi(money);
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ���ȡ��������� 1 - 50000 ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if((cBill.principal+(int)cBill.interest) < num_money)
		{
			move(12, 0);
			prints("�Բ�������˼Դʵҵ�����ʻ�����͸֧��\n");
			pressanykey();
			outgame();
			return 0;
		}

		/*����.bank�ļ�*/
		/*����Ϣ�ŵ������˻��ͱ����ϣ���Ϣ��գ���󱾽��п۳�ȡ����Ŀ*/
			cBill.principal+=(int)cBill.interest;
			currentuser.inbank += (int)cBill.interest;
			cBill.interest-=(int)cBill.interest;	
			cBill.principal-=num_money;		
			cBill.date=time(0);
			substitute_record(fname, &cBill,sizeof(struct callbill), 1);
		/*����PASSFILE�ļ�*/
		currentuser.money += num_money;//���ӱ������Ϣ
		currentuser.inbank -= num_money;//�۳�����
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		/*����֪ͨ*/
		getdatestring (cBill.date, NA);
		sprintf (mail_title, "%s %.23s ȡ�� %d Ԫ", currentuser.userid,datestring + 6,num_money);
		securityreport(mail_title);		
		//pax@sjtubbs 05-07 ȡ���
		if (askyn ("Ҫ�����������ż���", YEA, NA) == YEA)//pax@05-07 ȷ��
		{
			sprintf (mail_buffer,
				"����վ: ˼Դʵҵ���з���ϵͳ\n\n"
				"\n�װ����û�%s�����ڡ�%.23s��\n\n"
				"\n ��˼Դ�����л���ȡ�� %d Ԫ",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		//end by pax
		move(14,0);
		prints("���Ĵ�������Ѿ����\n");
		pressanykey();
		outgame();
		return 1;
		
	}
	if(opt == 1)
	{
		update_deposit(currentuser.userid);//������Ϣ pax@2005-8-8
		getdata(10, 0, "����������Ľ�� [1 - 50000] (����Ϊ����):", money, 6, DOECHO, YEA);
		num_money = (int)atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ��𣬴��������� 1 - 50000 ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("�Բ��������ֽ��㣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		sprintf (fname,"bank/%s.bank", currentuser.userid);
		if(fd=open(fname,O_RDWR )==-1)//��һ�δ��
		{
			fd=open(fname,O_RDWR | O_CREAT ,0644 );		
			cBill.date=time(0);			
			cBill.principal=num_money;	
			cBill.interest=0;
			write(fd,&cBill,sizeof(struct callbill));//��д�ļ�
			close(fd);		
		}
		else{

			get_record (fname, &cBill,sizeof(struct callbill), 1);
			cBill.principal+=(int )cBill.interest;
			cBill.interest-=(int )cBill.interest;	
			cBill.principal+=num_money;		
			cBill.date=time(0);

			substitute_record(fname, &cBill,sizeof(struct callbill), 1);
		}
		currentuser.money -= num_money;
		currentuser.inbank += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		getdatestring (cBill.date, NA);		
		sprintf (mail_title, "%s %.23s ��� %d Ԫ", currentuser.userid,datestring + 6,num_money);
		securityreport(mail_title);		
		//pax@sjtubbs 05-07 ����
		if (askyn ("Ҫ�����������ż���", YEA, NA) == YEA)//pax@05-07 ȷ��
		{

			sprintf (mail_buffer,
				"����վ: ˼Դʵҵ���з���ϵͳ\n\n"
				"\n�װ����û�%s�����ڡ�%.23s��\n\n"
				"\n ��˼Դ�����л��ڴ�� %d Ԫ",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		//end by pax
		move(14,0);
		prints("���Ĵ�������Ѿ����\n");
		pressanykey();
		outgame();
		return 1;
		
	}
	if(opt == 3)//buy
	{
		getdata(10, 0, "����������Ҫ�������Ϸ����Ŀ [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ��𣬹������������� 1 - 50000 ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("�Բ��������ֽ��㣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		currentuser.money -= num_money;
		mdata.money += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		fp = fopen(buf,"w");
		if(fp != NULL)
		{
			result = fwrite(&mdata, sizeof(mdata), 1, fp);
			fclose(fp);
		}		
		move(12,0);
		prints("������Ϸ�Ҷһ������Ѿ����\n");

		sprintf(sysbuf, "%s ������Ϸ�� %d Ԫ", currentuser.userid, num_money);
		securityreport(sysbuf);
		
		//pax@sjtubbs 05-07 ������Ϸ��
		if (askyn ("Ҫ�����������ż���", YEA, NA) == YEA)//pax@05-07 ȷ��
		{
			now = time (0);	
			getdatestring (now, NA);
			sprintf (mail_title, "%.23s ������Ϸ�� %d Ԫ", datestring + 6,num_money);
			sprintf (mail_buffer,
				"\n����վ: ˼Դʵҵ���з���ϵͳ\n\n"
				"\n�װ����û�%s�����ڡ�%.23s��\n\n"
				"\n ������Ϸ�� %d Ԫ",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		pressanykey();
		outgame();
		return 1;
	}
	
/*	if(opt == 4)//sell
	{
		getdata(10, 0, "����������Ҫ���۵���Ϸ����Ŀ [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ��𣬳������������� 1 - 50000 ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		if(mdata.money < num_money)
		{
			move(12, 0);
			prints("�Բ�����û����ô����Ϸ�����֣�\n");
			pressanykey();
			outgame();
			return 0;
		}
		mdata.money -= num_money;		
		currentuser.money += (num_money/10);
		
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		fp = fopen(buf,"w");
		if(fp != NULL)
		{
			result = fwrite(&mdata, sizeof(mdata), 1, fp);
			fclose(fp);
		}		
		move(12,0);
		prints("������Ϸ�Ҷһ������Ѿ����\n");
		sprintf(sysbuf, "%s ������Ϸ�� %d Ԫ", currentuser.userid, num_money);
		securityreport(sysbuf);
		//pax@sjtubbs 05-07 ������Ϸ��
		if (askyn ("Ҫ�����������ż���", YEA, NA) == YEA)//pax@05-07 ȷ��
		{
			now = time (0);
			getdatestring (now, NA);
			sprintf (mail_title, "%.23s ������Ϸ�� %d Ԫ", datestring + 6,num_money);
			sprintf (mail_buffer,
				"\n����վ: ˼Դʵҵ���з���ϵͳ\n\n"
				"\n�װ����û�%s�����ڡ�%.23s��\n\n"
				"\n ������Ϸ�� %d Ԫ",currentuser.userid, datestring + 6,num_money);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		//end by pax
		pressanykey();
		outgame();
		return 1;
	}
*/	
	if(opt == 4)
			//if(opt == 5)
	{
		getdata(10, 0, "����������Ľ�� [1 - 50(��λ ǧԪ)] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ��𣬴��������� 1 - 50 (ǧԪ) ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		num_money=num_money*1000;
		if(currentuser.money < num_money)
		{
			move(12, 0);
			prints("�Բ��������ֽ��㣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		
		getdata(12, 0, "��������˴δ���ʱ�䳤�� [1 - 6]����λ���£� :", length, 2, DOECHO, YEA);
		num_length = atoi(length);
		
		if (!isdigit(length[0]) || num_length <= 0 || num_length > 6 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 14, 0);
			prints("�Բ��𣬴���ʱ�䳤�ȱ����� 1 - 6 ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}
		
		fd=open("bank/bank",O_APPEND | O_WRONLY | O_CREAT ,0644 );
		stat("bank/bank",&kkk);
		size=kkk.st_size/sizeof(struct bankbill);
		strcpy(Bill.userid,currentuser.userid);
		Bill.usernum=size+1;
		Bill.datein=time(0);			
		Bill.money=num_money;		
		Bill.dateout=0;	
		Bill.length=num_length;
		write(fd,&Bill,sizeof(struct bankbill));
		close(fd);
		
		currentuser.money -= num_money;
		currentuser.inbank += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
		//���ڴ��ǿ�Ʒ���
		//		if (askyn ("Ҫ�����������ż���", YEA, NA) == YEA)//pax@05-07 ȷ��
		//		{
		now = time (0);
		getdatestring (now, NA);
		sprintf (mail_title, "%s ���� %d���ڴ�� %d Ԫ", currentuser.userid,num_length,num_money);
		sprintf (mail_buffer,
			"����վ: ˼Դʵҵ���з���ϵͳ\n"
			"\n�װ����û�%s�����ڡ�%.23s��\n"
			"\n ��˼Դ�����д�� %d Ԫ������Ϊ %d ����\n"
			"\n�ʵ���Ϊ��%ld",currentuser.userid, datestring + 6,num_money ,num_length,size+1);
		bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		//		}
		move(12,0);
		prints("���Ĵ�������Ѿ����\n");
		sprintf(sysbuf, "%s ����%d ���ڴ�� %d Ԫ���ʵ���Ϊ %ld", currentuser.userid,num_length, num_money, Bill.usernum);
		securityreport(sysbuf);
		pressanykey();
		outgame();
		return 1;
		
	}
	if(opt == 5)//	if(opt == 6)
	{
		getdata(10, 0, "������������ʵ��� :", BillNo, 8, DOECHO, YEA);
		num_bill = atoi(BillNo);
		
		if (!isdigit(BillNo[0]) || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ�����������ȷ�ʵ��ţ�\n");
			pressanykey();
			outgame();
			return 0;
		}
		sprintf (fname,"bank/bank");
		get_record (fname, &Bill,sizeof(struct bankbill), num_bill);
		
		if(strcmp("pax",currentuser.userid)!=0)//�ж��Ƿ�Ϊ��ǰ�û����˻����ж��Ƿ��Ѿ���ȡ����
		{
			if((strcmp(Bill.userid,currentuser.userid)!=0) || (Bill.dateout!=0))
			{
				move( 12, 0);
				prints("�Բ�����������ȷ�ʵ��ţ�\n");
				pressanykey();
				outgame();
				return 0;
			}
		}
		Bill.dateout=time(0);//�趨ȡ��ʱ��
		if(Bill.dateout<Bill.datein+Bill.length*86400*30)
		{
			if(askyn ("��ȷ��Ҫ���������", NA, NA) != YEA)
			{
				move (2, 0);
				prints ("ȡ������\n");
				pressreturn ();
				outgame();
				return 0;
			}
			else
			{
				callrate=0.0005;
				if(fd_rate=open("bank/rate",O_RDONLY)!=1)
				{
					fd_rate=open("bank/rate",O_RDONLY);
					stat("bank/rate",&kkk);
					size=kkk.st_size/sizeof(struct rate);
					read(fd_rate,&bankrate,sizeof(struct rate));	
					while((size>1) && (bankrate.time<Bill.datein))
					{
						read(fd_rate,&bankrate,sizeof(struct rate));
						callrate=bankrate.currdepo;
						size--;
					}
					close(fd_rate);
				}
				num_money=(int)Bill.money*(1+callrate*ceil((Bill.dateout-Bill.datein)/86400));//���ջ��ڵ���
			}
		}
		else
		{
			if(fd_rate=open("bank/rate",O_RDONLY)!=1)
			{
				fd_rate=open("bank/rate",O_RDONLY);
				stat("bank/rate",&kkk);
				size=kkk.st_size/sizeof(struct rate);
				read(fd_rate,&bankrate,sizeof(struct rate));
    			while((size>1) && (bankrate.time<Bill.datein))
				{
					read(fd_rate,&bankrate,sizeof(struct rate));	
					BANK_CURRDEPO_RATE[0]=bankrate.timedepo_01;
					BANK_CURRDEPO_RATE[1]=bankrate.timedepo_02;
					BANK_CURRDEPO_RATE[2]=bankrate.timedepo_03;
					BANK_CURRDEPO_RATE[3]=bankrate.timedepo_04;
					BANK_CURRDEPO_RATE[4]=bankrate.timedepo_05;
					BANK_CURRDEPO_RATE[5]=bankrate.timedepo_06;
					size--;
				}	
				close(fd_rate);
			}
			num_money=(int)Bill.money*(1+BANK_CURRDEPO_RATE[Bill.length-1]/100);//���ն��ڵ���
		}
		sprintf (fname,"bank/bank");
		substitute_record(fname, &Bill,sizeof(struct bankbill), num_bill);
		currentuser.money += num_money;//�ֽ���ϱ������Ϣ
		currentuser.inbank -= Bill.money;//�ʻ�ֻ��ȥ����
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);
//		close(fd);
		
		//pax@sjtubbs 05-07 ����
		if (askyn ("Ҫ�����������ż���", YEA, NA) == YEA)//pax@05-07 ȷ��
		{
			getdatestring (Bill.dateout, NA);
			sprintf (mail_title, "%s ȡ�� %d����ȡ�� %d Ԫ", currentuser.userid,Bill.length,num_money );
			sprintf (mail_buffer,
				"����վ: ˼Դʵҵ���з���ϵͳ\n"
				"\n�װ����û�%s�����ڡ�%.23s��\n"
				"\n ��˼Դ������ȡ�� %d Ԫ\n"
				"\n�ʵ���Ϊ��%ld",currentuser.userid, datestring + 6,num_money ,Bill.usernum);
			bankreport(mail_title, mail_buffer, currentuser.userid, NULL);
		}
		move(12,0);
		prints("����ȡ�������Ѿ����\n");
		sprintf(sysbuf, "%s ��ȡ��� %d Ԫ���ʵ���Ϊ %ld", currentuser.userid, num_money,Bill.usernum);
		
		securityreport(sysbuf);
		pressanykey();
		outgame();
		return 1;
	}
	
	move(12,0);
	prints("����˼Դ����������Ϣ:)\n");
	pressanykey();
	outgame();
	return 1;
	
	
}


int 
bank_record(){
	struct bankbill Bill;
	long size;
	struct stat kkk;
	//	char fname[80];
	int i;
	long count;
	int fd;
	char mail_title[STRLEN]={0};
	char mail_buffer[1024] = {0};
	int all;
	char ans[5];
	char temp[65535];
	char someoneID[20];
	clear();
	all = 0;
	
	//-----------------------------------------------------------------
    modify_user_mode (QUERY);
    usercomplete ("�����������ѯ���ʺ�: ", someoneID);
    if (someoneID[0] == 0)
    {
        getdata (0, 30, "��ѯ���е�������?[Y/N]: ", ans, 7, DOECHO, YEA);
        if ((*ans != 'Y') && (*ans != 'y'))
        {
            return;
        }
        else
            all = 1;
    }
	else if (!getuser (someoneID))
    {
        prints ("����ȷ��ʹ���ߴ���\n");
        pressreturn ();
        return;
    }
	
	
	if (all==1){
		fd=open("bank/bank",O_RDWR | O_CREAT );
		stat("bank/bank",&kkk);
		size=kkk.st_size/sizeof(struct bankbill);
		i=0;
		count=0;
		sprintf (temp,"�����˵����м�¼\n");
		sprintf (mail_title,"�����˵����м�¼");		
		sprintf(mail_buffer,"%8s %12s %20s %20s %8s %s\n", "�ʵ���", "ID", "��������","ȡ������","���","����");
		strcat (temp,mail_buffer);
		
		while (i<size)
		{
			read(fd,&Bill,sizeof(struct bankbill));
			if(Bill.dateout==0){
				count=count+Bill.money;}			
			//	sprintf (mail_buffer,"\n%8d %12s %20s %20s %8d %d",Bill.usernum,Bill.userid,getdatebank(Bill.datein),getdatebank(Bill.dateout),Bill.money,Bill.length);
			getdatebank(Bill.datein);
			sprintf (mail_buffer,"\n%8d %12s %s ",Bill.usernum,Bill.userid,bankdatetime);
			strcat (temp, mail_buffer);
			getdatebank(Bill.dateout);
			sprintf (mail_buffer,"%s %8d %2d",bankdatetime,Bill.money,Bill.length);
			strcat (temp, mail_buffer);
			
			i++;
		}
		sprintf (mail_buffer,"\n�ܶ�:%ld",count);
		strcat (temp, mail_buffer);
		bankreport(mail_title, temp, currentuser.userid, NULL);
		close(fd);	
	}
	else{
		fd=open("bank/bank",O_RDWR | O_CREAT );
		stat("bank/bank",&kkk);
		size=kkk.st_size/sizeof(struct bankbill);	
		i=0;
		count=0;
		
		sprintf (mail_title,"%s�����м�¼",someoneID);	
		sprintf (temp,"%s�����м�¼\n",someoneID);				
		sprintf(mail_buffer,"%8s %20s %20s %8s %s\n", "�ʵ���", "��������","ȡ������","���","����");
		strcat (temp,mail_buffer);
		while (i<size)
		{
			read(fd,&Bill,sizeof(struct bankbill));
			//if(strcmp(Bill.userid,someoneID)==0)
			if((strcmp(Bill.userid,someoneID)==0)&&(Bill.dateout==0))
			{
				count=count+Bill.money;
				getdatebank(Bill.datein);
				sprintf (mail_buffer,"\n%8d  %s ",Bill.usernum,bankdatetime);
				strcat (temp, mail_buffer);
				getdatebank(Bill.dateout);
				sprintf (mail_buffer,"%s %8d %2d",bankdatetime,Bill.money,Bill.length);
				strcat (temp, mail_buffer);
			}
			i++;
		}
		sprintf (mail_buffer,"\ncount=%ld,size=%d,id=%s",count,size,someoneID);
		strcat (temp, mail_buffer);
		bankreport(mail_title, temp, currentuser.userid, NULL);
		close(fd);	}
}



int
getdatebank (time_t now)
{
	struct tm *tm;
	
	tm = localtime (&now);
	sprintf (bankdatetime, "%4d-%02d-%02d %02d:%02d:%02d",
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}
int lending()
{
	//	char sysbuf[STRLEN];
	int     id, canlending=0,maxnum = 0, num = 0,u_money=0;
	char	ans[STRLEN];
	char ps[5][STRLEN];
	int i = 0;
	time_t 	now=time(0);
	extern int gettheuserid();
	
	if(gotomarket("�����г�")) return 0;
	maxnum = currentuser.money - 100;
	prints("[m��ӭʹ��[1;33m"BBSNAME"[m�����г�[32m����ת��[m����");
	prints("\n\n��Ŀǰ������ǣ�\nע������([32m%d[m ��) "
		"��վ��ʱ��([32m%d[m Сʱ) "
		"[44;37m��ת���ʽ�([32m%d[37m Ԫ)[m",
		(now - currentuser.firstlogin)/86400,
		currentuser.stay/3600,currentuser.money-100);
	if(currentuser.stay<=3600||now-currentuser.firstlogin<=259200||maxnum<=0){
		prints("\n\nĿǰ�г��涨�� ����[32m����ת��[m�� ID ����߱�����"
			"ȫ��������\n"
			"    1. ���ʺ�ע���������� 3 ��;\n"
			"    2. ����վʱ������ 1 Сʱ;\n"
			"    3. ����ӵ�д��� 100 Ԫ.\n");
		prints("\n\n�����г��涨����Ŀǰ��û��[32m����ת��[m���ʸ�\n");
		pressanykey();
		outgame();
		return 0;
	}
	if (!gettheuserid(9,"����ת�ʵ�˭���ʻ��ϣ������������ʺ�: ",&id))
	{
		outgame();
		return 0;
	}
	u_money = lookupuser.money+lookupuser.medals*10000+ lookupuser.inbank;
	if(!strcmp(currentuser.userid,lookupuser.userid) ||u_money > CHANGE_MAX){
		prints("\n�Բ���%s Ŀǰ�в���Ҫ����ת�ʣ�",lookupuser.userid);
		pressanykey();
		outgame();
		return 0;
	}
	move(10,0);
	canlending = (maxnum>(CHANGE_MAX-u_money))?(CHANGE_MAX-u_money):maxnum;
	prints("����ת�ʸ� [[1;32m%s[m]��ת�������Ϊ [1;33m%d[m Ԫ��",
		lookupuser.userid, canlending);
	getdata(12, 0, "ȷʵҪת�ʣ�������ת����Ŀ��������ֱ�ӻس�ȡ��ת��: ",
		ans, 6, DOECHO, YEA);
	num = atoi(ans);
	if ( num <= 0 || num > canlending ) {
		prints("\n������Ŀ�д��� ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	set_safe_record();
	if(currentuser.money - 100  != maxnum) {
		prints("\n�Բ������Ŀ�ת���ʽ������仯��ȡ���˴ν��ף�");
		prints("\n������ִ�б����ס�");
		pressanykey();
		outgame();
		return 0;
	}
	
	for (i = 0; i < 5; i++)
		ps[i][0] = '\0';
	move (14, 0);
	prints ("������ת�ʸ���(������У��� Enter ����)");
	for (i = 0; i < 5; i++)
	{
		getdata (i + 15, 0, ": ", ps[i], STRLEN - 5, DOECHO, YEA);
		if (ps[i][0] == '\0')
			break;
	}
	if (askyn ("ȷ��ת�ʣ�", YEA, NA) == YEA)//pax@05-07 ת��ȷ��
	{
		currentuser.money -= num;
		substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);
		lookupuser.money += num;
		substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
		
		prints("\nлл�������Ѿ��� [1;32m%s[m ת�� [1;33m%d[m Ԫ��",
			lookupuser.userid,num);
		prints("\nΪ��ʾ�����ת����Ϊ�ĸ�л����վ�Ѿ����ż�֪ͨ������");
		sprintf(genbuf,"�� %s ת�� %d Ԫ",lookupuser.userid,num);
		gamelog(genbuf);
		sprintf(ans,"%s �� %s ������ %d Ԫ����ת��",currentuser.userid, lookupuser.userid, num);
		sprintf(genbuf,"\n %s :\n\n\t���ã�\n\n\t�������� %s ���������� %d Ԫ����ת���ʽ�\n",lookupuser.userid,currentuser.userid,num);
		//   bankreport(ans,genbuf,NA,lookupuser.userid);
		
		for (i = 0; i < 5; i++)
		{
			if (ps[i][0] == '\0')
				break;
			if (i == 0)
				strcat (genbuf, "\n\nת�ʸ��ԣ�\n");
			strcat (genbuf, ps[i]);
			strcat (genbuf, "\n");
		}
		
		strcpy (currboard, "MoneyAdmin");
		autoreport(ans,genbuf, lookupuser.userid, NULL);
		//add by pax@sjtubbs 05-07 ת�ʱ��ݸ��Լ�
		if (askyn ("���ݸ��Լ���", YEA, NA) == YEA)//pax@05-07 ת��ȷ��
		{
			sprintf(ans,"%s �� %s ��ȥ�� %d Ԫ����ת��",currentuser.userid, lookupuser.userid, num);
			sprintf(genbuf,"\n %s :\n\n\t���ã�\n\n\t ������������ %s ��ȥ�� %d Ԫ����ת���ʽ�\n",currentuser.userid,lookupuser.userid,num);
			for (i = 0; i < 5; i++)
			{
				if (ps[i][0] == '\0')
					break;
				if (i == 0)
					strcat (genbuf, "\n\nת�ʸ��ԣ�\n");
				strcat (genbuf, ps[i]);
				strcat (genbuf, "\n");
			}
			bankreport(ans,genbuf,currentuser.userid, NULL);
		}
		//end by pax
		
	}//pax
	pressanykey();
	outgame();
	return 1;
}

int popshop(void)
{
	int no,num,maxnum/*,templog*/;
	char ans[10];
	//char sysbuf[STRLEN];
	EXCHANGES exchanges[10] = {
		//      {"��վ����",2},
		//      {"������",5},
		//      {"������",10000},
		{"������",40000},
		{"����������",100000},
		{"�ʺ����ñ���",200000},
		{"ǿ�ƺ���",20000},
		{"�ӳ�����ʱ��",10000},
		{"666 ������",20000},
		{"����������",40000}
	};
	if(gotomarket("�г��䵱��")) return 1;
	prints("���յ䵱����:");
	for(no = 0; no < 7; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ԫ", exchanges[no].value);
	}
	//   move(16,0);
	//   prints("[0;1m��Ŀǰ�������: (1)��վ����([32m%d[m)  "
	//          "(2)������([32m%d[37m)  (3)������.([32m%d[37m)\n",
	//	  currentuser.numlogins,currentuser.numposts,currentuser.medals);
	getdata(18, 0, "����䵱��һ�", ans, 10, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > 7/*10*/ ) {
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	move(18, 30);
	prints("��ѡ��䵱��[32m%s[m����",exchanges[no-1].title);
	if(no>0/*3*/){
		set_safe_record();
		maxnum = exchanges[no-1].value;
		switch(no) {
		case 1:
			if(!HAS_PERM(PERM_CLOAK)) { num = 0; break; }
			break;
		case 2:
			if(!HAS_PERM(PERM_SEECLOAK)) { num = 0; break; }
			break;
		case 3:
			if(!HAS_PERM(PERM_XEMPT)) { num = 0; break; }
			break;
		case 4:
			if(!HAS_PERM(PERM_FORCEPAGE)) { num = 0; break; }
			break;
		case 5:
			if(!HAS_PERM(PERM_EXT_IDLE)) { num = 0; break; }
			break;
		case 6:
			if(!HAS_PERM(PERM_666LIFE)) { num = 0; break; }
			break;
		case 7:
			if(!HAS_PERM(PERM_MAILINFINITY)) { num = 0; break; }
			break;
		}
		prints("\n\n");
		if(!num) {
			prints("�Բ���, �㻹û������Ȩ��. ");
			pressanykey();
			outgame();
			return 0;
		}
		if(askyn("��ȷ��Ҫ�䵱��",NA,NA) == NA ) {
			move(23,0);clrtoeol();
			prints("����ȡ�� ...");
			pressanykey();
			outgame();
			return 0;
		}
		set_safe_record();
		switch(no) {
		case 1:
			num = HAS_PERM(PERM_CLOAK);
			currentuser.userlevel &= ~PERM_CLOAK ;
			break;
		case 2:
			num = HAS_PERM(PERM_SEECLOAK);
			currentuser.userlevel &= ~PERM_SEECLOAK ;
			break;
		case 3:
			num = HAS_PERM(PERM_XEMPT);
			currentuser.userlevel &= ~PERM_XEMPT ;
			break;
		case 4:
			num = HAS_PERM(PERM_FORCEPAGE);
			currentuser.userlevel &= ~PERM_FORCEPAGE ;
			break;
		case 5:
			num = HAS_PERM(PERM_EXT_IDLE);
			currentuser.userlevel &= ~PERM_EXT_IDLE ;
			break;
		case 6:
			num = HAS_PERM(PERM_666LIFE);
			currentuser.userlevel &= ~PERM_666LIFE ;
			break;
		case 7:
			num = HAS_PERM(PERM_MAILINFINITY);
			currentuser.userlevel &= ~PERM_MAILINFINITY ;
			break;
		}
		if(!num) {
			prints("�Բ���, ������ݷ����˱仯, ��Ŀǰû������Ȩ��. ");
			pressanykey();
			outgame();
			return 0;
		}
		} /*
		  else {
		  if( no == 1 )maxnum = currentuser.numlogins-1;
		  else if ( no == 2) maxnum = currentuser.numposts;  
		  else	maxnum = currentuser.medals;
		  templog = maxnum;
		  sprintf(genbuf,"����䵱������(���%d)��",maxnum);
		  getdata(19, 0, genbuf,ans, 10, DOECHO, YEA);
		  num = atoi(ans);
		  if ( num <= 0 || num > maxnum ) {
		  prints("�������ȡ������ ...");
		  pressanykey();
		  outgame();
		  return 0;
		  }
		  maxnum = num*exchanges[no-1].value;
		  move(19,0);
		  prints("�����Ƶ䵱%s.[32m%d.[m �ݣ� %s .[33m%d.[m Ԫ��\n",
		  exchanges[no-1].title,num,"���Ի��",maxnum);
		  if(askyn("��ȷ��Ҫ�䵱��",NA,NA) == NA ) {
		  move(21,0);clrtoeol();
		  prints("ȡ������ ...");
		  pressanykey();
		  outgame();
		  return 0;
		  }
		  set_safe_record();
		  if (no == 1) {
		  if(templog==currentuser.numlogins-1)
		  currentuser.numlogins -= num;
		  else templog = -1;
		  } else if (no == 2)  {
		  if(templog == currentuser.numposts)
		  currentuser.numposts -= num;
		  else templog = -1;
		  } else {
		  if(templog == currentuser.medals)
		  currentuser.medals -= num;
		  else templog = -1;
		  }
		  if( templog == -1) {
		  move(21,0); clrtoeol();
		  prints("�Բ���, �ڽ��׹������������ݷ����˱仯.\n");
		  prints("Ϊ��֤���׵���������, �˴ν���ȡ��.\n���������½��б�����.");
		  pressanykey();
		  outgame();
		  return 0;
		  }
	  }*/
	  
	  currentuser.money += maxnum;
	  if( currentuser.money > 900000000 ){
		  move(21,0); clrtoeol();
		  prints("�Բ��𣬽������ݹ��󣬲�������������½��ף�\n");
		  prints("��������һ���ֽ�Ǯ�������С�");
		  pressanykey();
		  outgame();
		  return 0;
	  }
	  substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	  move(21,0); clrtoeol();
	  prints("˳����ɽ��ף���ӭ�����ٴλݹˡ�;P");
	  if(no<=3)
	  {
		  sprintf(genbuf,"�䵱%s %d �ݣ����� %d Ԫ.",exchanges[no-1].title,num,maxnum);
	  }
	  else
	  {
		  sprintf(genbuf,"�䵱%s������ %d Ԫ.",exchanges[no-1].title,maxnum);
	  }
	  securityreport(genbuf);
	  gamelog(genbuf);
	  pressanykey();
	  outgame();
	  return 1;
}

/*
int shopping()//modified by smartfish, 2003.09.08.
{
	int no,hasperm=1,maxnum;
	char ans[10];
	EXCHANGES exchanges[10] = {
		{"������",100000},
		{"����������",4000000},
		{"�ʺ����ñ���",4000000},
		{"ǿ�ƺ���",4000000},
		{"�ӳ�����ʱ��",400000},
		{"666������",1000000},
		{"����������",9900000}
	};
	if(gotomarket("�г���������")) return 1;
	prints("������Ʒ����:");
	for(no = 0; no < 7; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ԫ", exchanges[no].value);
	}
	move(16,0);
	prints("��Ŀǰ���� %d Ԫ (���� %d ��)\n",
		currentuser.money,currentuser.medals);
	getdata(18, 0, "���빺����һ����Ʒ��", ans, 10, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > 7 ) {
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	set_safe_record();
	maxnum = exchanges[no-1].value;
	switch(no) {
	case 1:
		hasperm = HAS_PERM(PERM_CLOAK);
		break;
	case 2:
		hasperm = HAS_PERM(PERM_SEECLOAK);
		break;
	case 3:
		hasperm = HAS_PERM(PERM_XEMPT);
		break;
	case 4:
		hasperm = HAS_PERM(PERM_FORCEPAGE);
		break;
	case 5:
		hasperm = HAS_PERM(PERM_EXT_IDLE);
		break;
	case 6:
		hasperm = HAS_PERM(PERM_666LIFE);
		break;
	case 7:
		hasperm = HAS_PERM(PERM_MAILINFINITY);
		break;
	}
	prints("\n\n");
	if(hasperm) {
		prints("���Ѿ�������Ȩ��, ����Ҫ�ٹ���. ");
		pressanykey();
		outgame();
		return 0;
	}
	if(currentuser.money < maxnum) {
		prints("\n�Բ���, ��û���㹻��Ǯ��������Ȩ��.");
		pressanykey();
		outgame();
		return 0;
	}
	if(askyn("��ȷ��Ҫ������",NA,NA) == NA ) {
		move(23,0);clrtoeol();
		prints("ȡ�����ס�");
		pressanykey();
		outgame();
		return 0;
	}
	set_safe_record();
	switch(no) {
	case 1:
		hasperm = HAS_PERM(PERM_CLOAK);
		currentuser.userlevel |= PERM_CLOAK ;
		break;
	case 2:
		hasperm = HAS_PERM(PERM_SEECLOAK);
		currentuser.userlevel |= PERM_SEECLOAK ;
		break;
	case 3:
		hasperm = HAS_PERM(PERM_XEMPT);
		currentuser.userlevel |= PERM_XEMPT ;
		break;
	case 4:
		hasperm = HAS_PERM(PERM_FORCEPAGE);
		currentuser.userlevel |= PERM_FORCEPAGE ;
		break;
	case 5:
		hasperm = HAS_PERM(PERM_EXT_IDLE);
		currentuser.userlevel |= PERM_EXT_IDLE ;
		break;
	case 6: 
		hasperm = HAS_PERM(PERM_666LIFE); 
		currentuser.userlevel |= PERM_666LIFE ; 
		break;
	case 7: 
		hasperm = HAS_PERM(PERM_MAILINFINITY); 
		currentuser.userlevel |= PERM_MAILINFINITY ;
		break;
	}
	if(hasperm) {
		prints("�ڽ��׽���ǰ���Ѿ���������Ȩ��, ����ȡ���˴ν���. ");
		pressanykey();
		outgame();
		return 0;
	}
	if(currentuser.money < maxnum) {
		prints("\n�Բ���, ��û���㹻��Ǯ��������Ȩ��.");
		pressanykey();
		outgame();
		return 0;
	}
	currentuser.money -= maxnum;
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);        move(23,0); clrtoeol();
	prints("˳����ɽ��ף���ӭ�����ٴλݹˡ�;P");
	sprintf(genbuf,"����[%s]������ %d Ԫ.",exchanges[no-1].title,maxnum);
	securityreport(genbuf);
	gamelog(genbuf);
	pressanykey();
	outgame();
	return 1;
}	


*/




int shopping()//modified by pax, 2005.09.21.
{
	int i,no,maxnum;
	int id,counts=0;
	int fd;
	char	ans[STRLEN];
	char ps[5][STRLEN];
	char datafile[80] = BBSHOME "/etc/shop.ini";
	int type[64];
	EXCHANGES exchanges[64];
    FILE *fp;
    char t[256], *t1, *t2, *t3;
	struct gift cgift;

    fp = fopen (datafile, "r");
    if (fp == NULL)
        return;
    while (fgets (t, 255, fp) && counts <= 10)
    {
        t1 = strtok (t, " \t");
        t2 = strtok (NULL, " \t\n");
        t3 = strtok (NULL, " \t\n");
        if (t1[0] == '#' || t1 == NULL || t2 == NULL)
            continue;
		strncpy (exchanges[counts].title, t1, 16);
		exchanges[counts].value=atoi(strncpy (ans, t2, 36));
		type[counts] = t3 ? atoi (t3):0;
        counts++;
    }
    fclose (fp);

	if(gotomarket("�г���������")) return 1;
	prints("������Ʒ����:");
	for(no = 0; no < counts; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ԫ", exchanges[no].value);
	}
	move(14,0);
	prints("��Ŀǰ���� %d Ԫ (���� %d ��)\n",	currentuser.money,currentuser.medals);
	if (!gettheuserid(15,"�����˭�������������/�����ʺ�: ",&id))
	{
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	getdata(15, 0, "���빺����һ����Ʒ��", ans, 3, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > counts ) {
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	maxnum = exchanges[no-1].value;
	if(currentuser.money < maxnum) {
		prints("\n�Բ���, �����ʽ𲻹���");
		pressanykey();
		outgame();
		return 0;
	}
	if(askyn("��ȷ��Ҫ������",NA,NA) == NA ) {
		move(23,0);
		clrtoeol();
		prints("ȡ�����ס�");
		pressanykey();
		outgame();
		return 0;
	}
	currentuser.money -= maxnum;
	substitute_record(PASSFILE, &currentuser, sizeof(currentuser), usernum);
	sprintf(ans,"%s �� %s ����������",currentuser.userid, lookupuser.userid);
	sprintf(genbuf,"\n %s :\n\n\t���ã�\n\n\t�������� %s ����������� %s��\n",lookupuser.userid,currentuser.userid,exchanges[no-1].title);
	for (i = 0; i < 5; i++)
		ps[i][0] = '\0';
	move (16, 0);
	prints ("������ת�ʸ���(������У��� Enter ����)");
	for (i = 0; i < 5; i++)
	{
		getdata (i + 17, 0, ": ", ps[i], STRLEN - 5, DOECHO, YEA);
		if (ps[i][0] == '\0')
			break;
	}

		for (i = 0; i < 5; i++)
		{
			if (ps[i][0] == '\0')
				break;
			if (i == 0)
				strcat (genbuf, "\n\n���ԣ�\n");
			strcat (genbuf, ps[i]);
			strcat (genbuf, "\n");
		}		
	sprintf(ans,"%s Ϊ %s ����[%s]������ %d Ԫ.",currentuser.userid,lookupuser.userid,exchanges[no-1].title,maxnum);
	if(askyn("�Ƿ񹫿��˴ν�����Ϊ��",YEA,YEA) == YEA ) {
		strcpy (currboard, "gift");
		autoreport(ans,genbuf,lookupuser.userid, NULL);
		strcpy (currboard, "giftadmin");
		autoreport(ans,genbuf,NULL, NULL);
	}
	else
	{
	bankreport(ans,genbuf,lookupuser.userid,NA);
	strcpy (currboard, "giftadmin");
	autoreport(ans,genbuf,NULL, NULL);
	}

	//bankreport(t3,"create","pax",NULL);
	/*д�������˵ĸ����ļ�*/
	if(type[no-1]){
	sethomefile(genbuf,lookupuser.userid,"gift.dat");
	fd=open(genbuf,O_APPEND | O_WRONLY | O_CREAT ,0644 );
	strcpy(cgift.id,currentuser.userid);
	strcpy(cgift.name,exchanges[no-1].title);
	lseek(fd, 0, SEEK_END);
	write(fd,&cgift,sizeof(cgift));
	close(fd);
}
	securityreport(ans);	
	gamelog(ans);
	move(22,0);
	prints("˳����ɽ��ף���ӭ�����ٴλݹˡ�;P");
	pressanykey();
	outgame();
	return 1;
}	


bankreport (title, str, userid, sysboard)
char *title;
char *str;
char *userid;
char *sysboard;
{
	FILE *se;
	char fname[STRLEN];
	int savemode;
	savemode = uinfo.mode;
	report (title);
	sprintf (fname, "tmp/%s.%s.%05d", BBSID, currentuser.userid, uinfo.pid);
	if ((se = fopen (fname, "w")) != NULL)
	{
		fprintf (se, "%s", str);
		fclose (se);
		if (userid != NULL)
			mail_file (fname, userid, title);
		unlink (fname);
		modify_user_mode (savemode);
	}
}


/*�������д��*/

/*int shopping_admin()//modified by pax, 2005.09.21.
{
	int i,no,hasperm=1,maxnum;
	int id,id2,counts=0;
	int fd;
	char	ans[STRLEN];
	char ps[5][STRLEN];
	char datafile[80] = BBSHOME "/etc/shop.ini";
	int type[64];
	EXCHANGES exchanges[64];
    FILE *fp;
    char t[256], *t1, *t2, *t3;
	struct gift cgift;

    fp = fopen (datafile, "r");
    if (fp == NULL)
        return;
    while (fgets (t, 255, fp) && counts <= 10)
    {
        t1 = strtok (t, " \t");
        t2 = strtok (NULL, " \t\n");
        t3 = strtok (NULL, " \t\n");
        if (t1[0] == '#' || t1 == NULL || t2 == NULL)
            continue;
		strncpy (exchanges[counts].title, t1, 16);
		exchanges[counts].value=atoi(strncpy (ans, t2, 36));
		type[counts] = t3 ? atoi (t3):0;
        counts++;
    }
    fclose (fp);

	prints("������Ʒ����:");
	for(no = 0; no < counts; no ++) {
		move(5+no, 2);
		prints("(%2d) %s",no+1,exchanges[no].title);
		move(5+no, 20);
		prints("==> %6d Ԫ", exchanges[no].value);
	}
	move(14,0);
	if (!gettheuserid(15,"������: ",&id2))
	{
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	getdata(15, 0, "���빺����һ����Ʒ��", ans, 3, DOECHO, YEA);
	no = atoi(ans);
	if ( no < 1 || no > counts ) {
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}
	
	if(askyn("ȷ��Ҫ������",NA,NA) == NA ) {
		move(23,0);
		clrtoeol();
		prints("ȡ�����ס�");
		pressanykey();
		outgame();
		return 0;
	}
	/*д�������˵ĸ����ļ�
	if(type[no-1]){
	sethomefile(genbuf,lookupuser.userid,"gift.dat");
	fd=open(genbuf,O_APPEND | O_WRONLY | O_CREAT ,0644 );

	if (!gettheuserid(15,"������: ",&id))
	{
		prints("\n�������ȡ������ ...");
		pressanykey();
		outgame();
		return 0;
	}

	strcpy(cgift.id,lookupuser.userid);
	strcpy(cgift.name,exchanges[no-1].title);
	lseek(fd, 0, SEEK_END);
	write(fd,&cgift,sizeof(cgift));
	close(fd);
}

	pressanykey();
	return 1;
}	*/
/*int refreshbank(){

struct bankbill Bill;
struct userec user;	
struct callbill cBill;
long size,size1;
int money;
struct stat kkk;
struct stat kkk1;
int id;
int i;
long count;
int fd,fd1,fd2;
char temp[60000];
char temp2[60000];
char mail_title[STRLEN]={0};
char mail_buffer[1024] = {0};
char someoneID[20];
char fname[80];
clear();

  
	fd=open(PASSFILE,O_RDWR);
	stat(PASSFILE,&kkk);
	size=kkk.st_size/sizeof(struct userec);
	sprintf (mail_title,"%s","���Բ�ѯ");
	
	  while(size>0){
	  read(fd,&user,sizeof(struct userec));
	  fd1=open("bank/bank",O_RDWR | O_CREAT );
	  stat("bank/bank",&kkk1);
	  size1=kkk1.st_size/sizeof(struct bankbill);	
	  i=0;
	  count=0;
	  while (i<size1)
	  {
	  read(fd1,&Bill,sizeof(struct bankbill));
	  if((strcmp(Bill.userid,user.userid)==0)&&(Bill.dateout==0))
	  {
	  count=count+Bill.money;
	  }
	  i++;
	  }
	  sprintf (mail_buffer,"\n%-15s,���˻�:%ld,����:%ld,",user.userid,user.inbank,count);
	  strcat (temp, mail_buffer);
	  
		sprintf (fname,"bank/%s.bank", user.userid);
		if(fd2=open(fname,O_RDONLY )!=-1)
		{
		fd2=open(fname,O_RDWR);
		read(fd2,&cBill,sizeof(struct callbill));//��ȡԭ����Ϣ
		close(fd2);
		}
		else
		{
		cBill.principal=0;
		}
		money=user.inbank-count-cBill.principal;
		sprintf (mail_buffer,"����=%5ld,ԭ�ȴ��=%5ld",cBill.principal,money);
		strcat (temp, mail_buffer);
		if(money!=0)
		{
		
		  sprintf (mail_buffer,"\n%-15s��ԭ�ȴ�%-5ld",user.userid,money);
		  strcat (temp2, mail_buffer);
		  
			id = getuser(user.userid);
			user.money += money;
			user.inbank -= money;
			substitute_record(PASSFILE, &user, sizeof(struct userec),id);
			
			  
				}
				close(fd1);	
				size--;
				}
				close(fd);
				bankreport(mail_title, temp, currentuser.userid, NULL);
	bankreport(mail_title, temp2, currentuser.userid, NULL);
	
}*/


int research_current_deposit(){
	struct userec user;	
	struct callbill cBill;
	struct stat kkk;
	int all;
	int fd,fd1;
	int size;
	int cmoney;
	double cinterest;
	char temp[65535]={0};
	char mail_title[STRLEN]={0};
	char mail_buffer[1024] = {0};
	char someoneID[20];
	char fname[80];
	char ans[10];


	
	cmoney=0;
	cinterest=0.0;
	clear();
	all=0;
    modify_user_mode (QUERY);
    usercomplete ("�����������ѯ���ʺ�: ", someoneID);
    if (someoneID[0] == 0)
    {
        getdata (0, 30, "��ѯ���е�������?[Y/N]: ", ans, 7, DOECHO, YEA);
        if ((*ans != 'Y') && (*ans != 'y'))
        {
            return;
        }
        else
            all = 1;
    }
	else if (!getuser (someoneID))
    {
        prints ("����ȷ��ʹ���ߴ���\n");
        pressreturn ();
        return;
    }
	
	fd=open(PASSFILE,O_RDWR);
	stat(PASSFILE,&kkk);
	size=kkk.st_size/sizeof(struct userec);
	
	if(all==1){
		
		sprintf (mail_title,"%s(�ۼ�%d)","�����û��������ʲ�ѯ",size);
		while(size>0){


	


			sprintf (fname,"bank/%s.bank", user.userid);
			if(fd1=open(fname,O_RDONLY )!=-1)
			{	
				get_record (fname, &cBill,sizeof(struct callbill), 1);	//modified
				cmoney+=cBill.principal;
				cinterest+=cBill.interest;
			}
			else
			{
				cBill.principal=0;
				cBill.interest=0.0;
			}
			sprintf (mail_buffer,"\n%-15s,���ڣ�%16ld,��Ϣ��%8f",user.userid,cBill.principal,cBill.interest);
			strcat (temp, mail_buffer);
			size--;
		}
		sprintf (mail_buffer,"\n���� %ld,����Ϣ��%8.4f",cmoney,cinterest);
		strcat (temp, mail_buffer);
		
	}
	else
	{	
		sprintf (mail_title,"%s�������ʲ�ѯ",someoneID);
		sprintf (fname,"bank/%s.bank", someoneID);
		if(fd1=open(fname,O_RDONLY )!=-1)
		{
			get_record (fname, &cBill,sizeof(struct callbill), 1);	//modified

		}
		else
		{
			cBill.principal=0;
		}
		sprintf (temp,"\n%-15s,���ڣ�%6ld,��Ϣ��%8f",someoneID,cBill.principal,cBill.interest);	
	}
	close(fd);
	bankreport(mail_title, temp, currentuser.userid, NULL);
}