/* �����ˮվ �����г����� 1999.12.19 */
#include <string.h>
#include <stdlib.h>

#include "bbs.h"
#include "money.h"

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
	char msg[3];
	char money[7];
	int opt;
	int num_money;
	int id;

	id = getuser(currentuser.userid);

	if(gotomarket("��å������")) 
		return 0;
	prints("��ӭ���� ��å������\n\n");

	prints("��ĿǰЯ���ֽ� %d Ԫ�������ʻ����� %d Ԫ��\n\n", currentuser.money, currentuser.inbank);

	getdata( 8, 0,"��������Ҫ���еĲ��� [1] ȡ�� [2]��� :", msg, 2, DOECHO, /*NULL,*/ YEA);
	opt = atoi(msg);
	if (!isdigit(msg[0]) || opt <= 0 || opt > 2 || msg[0] == '\n' || msg[0] == '\0')
	{
		move(10,0);
		prints("�Բ�����å�����в��ṩ�������\n");
		pressanykey();
		outgame();
		return 0;
	}

	if(opt == 1)
	{
		getdata(10, 0, "��������Ҫȡ��Ľ�� [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
		if (!isdigit(money[0]) || num_money <= 0 || num_money > 50000 || msg[0] == '\n' || msg[0] == '\0')
		{
			move( 12, 0);
			prints("�Բ���ȡ��������� 1 - 50000 ֮�䣡\n");
			pressanykey();
			outgame();
			return 0;
		}

		if(currentuser.inbank < num_money)
		{
			move(12, 0);
			prints("�Բ���������å�������ʻ�����͸֧��\n");
			pressanykey();
			outgame();
			return 0;
		}

		currentuser.inbank -= num_money;
		currentuser.money += num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);	
		move(12,0);
		prints("����ȡ�������Ѿ����\n");
		pressanykey();
		outgame();
		return 1;

	}
		
	if(opt == 2)
	{
		getdata(10, 0, "����������Ľ�� [1 - 50000] :", money, 6, DOECHO, YEA);
		num_money = atoi(money);
		
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

		currentuser.inbank += num_money;
		currentuser.money -= num_money;
		substitute_record(PASSFILE, &currentuser, sizeof(struct userec),id);	
		move(12,0);
		prints("���Ĵ�������Ѿ����\n");
		pressanykey();
		outgame();
		return 1;

	}

//	prints();
	move(12,0);
	prints("������å��������Ϣ:)\n");
	pressanykey();
	outgame();
	return 1;


}


int lending()
{
   int     id, canlending=0,maxnum = 0, num = 0,u_money=0;
   char	ans[STRLEN];
   time_t 	now=time(0);
   extern int gettheuserid();
   
   if(gotomarket("�����г�")) return 0;
   maxnum = currentuser.money - 1000;
   prints("[m��ӭʹ��[1;33m"BBSNAME"[m�����г�[32m����ת��[m����");
   prints("\n\n��Ŀǰ������ǣ�\nע������([32m%d[m ��) "
          "��վ��ʱ��([32m%d[m Сʱ) "
	  "[44;37m��ת���ʽ�([32m%d[37m Ԫ)[m",
	  (now - currentuser.firstlogin)/86400,
	  currentuser.stay/3600,currentuser.money-1000);
   if(currentuser.stay<=36000||now-currentuser.firstlogin<=2592000||maxnum<=0){
      prints("\n\nĿǰ�г��涨�� ����[32m����ת��[m�� ID ����߱�����"
             "ȫ��������\n"
	     "    1. ���ʺ�ע���������� 30 ��;\n"
	     "    2. ����վʱ������ 10 Сʱ;\n"
	     "    3. ����ӵ�д��� 1000 Ԫ.\n");
      prints("\n\n�����г��涨����Ŀǰ��û��[32m����ת��[m���ʸ�\n");
      pressanykey();
      outgame();
      return 0;
   }
   if (!gettheuserid(9,"����ת�ʵ�˭���ʻ��ϣ������������ʺ�: ",&id))return 0;
   u_money = lookupuser.money+lookupuser.medals*10000+ lookupuser.inbank;
   if(!strcmp(currentuser.userid,lookupuser.userid) ||u_money > 20000){
         prints("\n�Բ���%s Ŀǰ�в���Ҫ����ת�ʣ�",lookupuser.userid);
	 pressanykey();
         outgame();
	 return 0;
   }
   move(10,0);
   canlending = (maxnum>(20000-u_money))?(20000-u_money):maxnum;
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
   if(currentuser.money - 1000  != maxnum) {
      prints("\n�Բ������Ŀ�ת���ʽ������仯��ȡ���˴ν��ף�");
      prints("\n������ִ�б����ס�");
      pressanykey();
      outgame();
      return 0;
   }
   currentuser.money -= num;
   substitute_record(PASSFILE, &currentuser, sizeof(currentuser),usernum);
   lookupuser.money += num;
   substitute_record(PASSFILE, &lookupuser, sizeof(lookupuser), id);
   prints("\nлл�������Ѿ��� [1;32m%s[m ת�� [1;33m%d[m Ԫ��",
      lookupuser.userid,num);
   prints("\nΪ��ʾ�����ת����Ϊ�ĸ�л����վ�Ѿ����ż�֪ͨ������");
   sprintf(genbuf,"�� %s ת�� %d Ԫ",lookupuser.userid,num);
   gamelog(genbuf);
   sprintf(ans,"%s ���������� %d Ԫ����ת��",currentuser.userid,num);
   sprintf(genbuf,"\n %s :\n\t���ã�\n\t�������� %s ���������� %d Ԫ����ת���ʽ�\n\t������ Market ���������˽��Ϊ���ı�ʾ��л��\n\t���������Ϳ��Ի���������ת���ʽ�\n\n\t��Ȼ����Ҳ�����˳�һ�κ��ٽ��뱾վ��\n\tһ�����Ի���������ת���ʽ�\n  ",lookupuser.userid,currentuser.userid,num);
   autoreport(ans,genbuf,NA,lookupuser.userid);
   pressanykey();
   outgame();
   return 1;
}

int popshop(void)
{
   int no,num,maxnum,templog;
   char ans[10];
   
   EXCHANGES exchanges[10] = {
      {"��վ����",2},
      {"������",5},
      {"������",10000},
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
   for(no = 0; no < 10; no ++) {
      move(5+no, 2);
      prints("(%2d) %s",no+1,exchanges[no].title);
      move(5+no, 20);
      prints("==> %6d Ԫ", exchanges[no].value);
   }
   move(16,0);
   prints("[0;1m��Ŀǰ�������: (1)��վ����([32m%d[m)  "
          "(2)������([32m%d[37m)  (3)������.([32m%d[37m)\n",
	  currentuser.numlogins,currentuser.numposts,currentuser.medals);
   getdata(18, 0, "����䵱��һ�", ans, 10, DOECHO, YEA);
   no = atoi(ans);
   if ( no < 1 || no > 10 ) {
      prints("\n�������ȡ������ ...");
      pressanykey();
      outgame();
      return 0;
   }
   move(18, 30);
   prints("��ѡ��䵱��[32m%s[m����",exchanges[no-1].title);
   if(no>3){
      set_safe_record();
      maxnum = exchanges[no-1].value;
      switch(no) {
         case 4:
	    if(!HAS_PERM(PERM_CLOAK)) { num = 0; break; }
	    break;
	 case 5:
	    if(!HAS_PERM(PERM_SEECLOAK)) { num = 0; break; }
	    break;
	 case 6:
	    if(!HAS_PERM(PERM_XEMPT)) { num = 0; break; }
	    break;
	 case 7:
	    if(!HAS_PERM(PERM_FORCEPAGE)) { num = 0; break; }
	    break;
	 case 8:
	    if(!HAS_PERM(PERM_EXT_IDLE)) { num = 0; break; }
	    break;
	 case 9:
	    if(!HAS_PERM(PERM_666LIFE)) { num = 0; break; }
	    break;
	 case 10:
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
         case 4:
	    num = HAS_PERM(PERM_CLOAK);
	    currentuser.userlevel &= ~PERM_CLOAK ;
	    break;
         case 5:
	    num = HAS_PERM(PERM_SEECLOAK);
	    currentuser.userlevel &= ~PERM_SEECLOAK ;
	    break;
         case 6:
	    num = HAS_PERM(PERM_XEMPT);
	    currentuser.userlevel &= ~PERM_XEMPT ;
	    break;
         case 7:
	    num = HAS_PERM(PERM_FORCEPAGE);
	    currentuser.userlevel &= ~PERM_FORCEPAGE ;
	    break;
	 case 8:
	    num = HAS_PERM(PERM_EXT_IDLE);
	    currentuser.userlevel &= ~PERM_EXT_IDLE ;
	    break;
	 case 9:
	    num = HAS_PERM(PERM_666LIFE);
	    currentuser.userlevel &= ~PERM_666LIFE ;
	    break;
	 case 10:
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
  } else {
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
      prints("�����Ƶ䵱%s[32m%d[m �ݣ� %s [33m%d[m Ԫ��\n",
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
   }
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
   sprintf(genbuf,"�䵱%s %d �ݣ����� %d Ԫ.",exchanges[no-1].title,num,maxnum);
   gamelog(genbuf);
   pressanykey();
   outgame();
   return 1;
}

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
   gamelog(genbuf);
   pressanykey();
   outgame();
   return 1;
}	


/**************************************************************************/
/*   ��Ϸ���ݽ����г�						          */
/**************************************************************************/

int incmoney(char *userid, int money)
{
   FILE *fp;
   struct Money mdata;
   char buf[STRLEN]; 
   int result =0;
   
   sethomefile(buf,userid,"game_money.dat");
   if(dashf(buf)) update_game_money();
   fp = fopen(buf,"r");
   if(fp==NULL) return 0;
   result = fread(&mdata,sizeof(mdata),1,fp);
   fclose(fp);
   if(result <=0) return 0;
   if(money == 0) return mdata.money;
   fp = fopen(buf,"w");
   if(fp==NULL) return 0;
   mdata.money += money;
   result = fwrite(&mdata, sizeof(mdata), 1, fp);
   fclose(fp);
   if(result == 0)return 0;
   return money;
}

int inmoney(int money) 
{ 
   return incmoney(currentuser.userid,money); 
}   

int demoney(int money)
{
   return inmoney((-1)*money);
}

int check_money(int money)
{
   int now_money = inmoney(0);
   if(inmoney(0) < money) {
      move(22, 0);
      clrtobot();
      prints("��Ǹ����ֻ�� %d Ԫ�Ľ�", money,now_money);
      pressanykey();
      return 1;
   }
   return 0;
}

void show_money(int money, char *welcome,int Clear)
{
   if(Clear) clear();
   if(welcome) ansimore(welcome, NA);
   move(22, 0);
   clrtobot();
   prints("[0;1;37;44m                  �������ֽ�: [36m%-18d[37mѺע���: [36m%-20d[m  ", inmoney(0), money);
}

int get_money(int m, char *welcome)
{
   int  money;
   char	buf[5];
   do {
      show_money(m,welcome,YEA);
      getdata(23,16,"��ҪѺע����Ǯ(1 - 2000)? ", buf, 5, DOECHO, YEA);
      if(buf[0] == '\0') return 0;
      money = abs(atoi(buf));
      if ( money <= 0) return 0;
      if(check_money(money))return 0;
   } while ((money < 1) || (money > 2000));
   demoney(money);
   show_money(money,NULL,YEA);
   return money;
}
