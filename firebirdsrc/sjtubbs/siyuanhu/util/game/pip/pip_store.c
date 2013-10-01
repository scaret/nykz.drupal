/*---------------------------------------------------------------------------*/
/* �̵�ѡ��:ʳ�� ��ʳ ���� ��� �鱾                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;

#ifndef MAPLE
extern char BoardName[];
#endif  // END MAPLE
//#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

/*---------------------------------------------------------------------------*/
/* �̵�ѡ��:ʳ�� ��ʳ ���� ��� �鱾                                       */
/* ���Ͽ�                                                                    */
/*---------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*  ��Ʒ�����趨                                                            */
/*--------------------------------------------------------------------------*/
struct goodsofpip
{
   int num;		/*���*/
   char *name;		/*����*/
   char *msgbuy;	/*����*/
   char *msguse;	/*˵��*/
   int money;		/*��Ǯ*/
   int change;		/*�ı���*/
   int pic1;
   int pic2;
};
typedef struct goodsofpip goodsofpip;

struct goodsofpip pipfoodlist[] = {
0,"��Ʒ��",	"˵��buy",	"˵��feed",			0,	0,	0,0,
1,"�óԵ�ʳ��",	"�����ָ�50",	"ÿ��һ��ʳ���ָ�����50�!",	50,	50,	1,1,
2,"��ζ����ʳ",	"�����ָ�100",	"���˻ָ�������С��Ҳ�������",	120,	100,	2,3,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipmedicinelist[] = {
0,"��Ʒ��",	"˵��buy",	"˵��feed",			0,	0,	0,0,
1,"���ô���",	"�����ָ�600",	"�ָ�������ʧ����������",	500,	600,	4,4,
2,"������֥",	"�����ָ�50",	"ÿ��һ����֥��ָ�����50�!",	100,	50,	7,7,
3,"ǧ���˲���",	"�����ָ�500",	"�ָ�������ʧ����������",	800,	500,	7,7,
4,"��ɽѩ��",	"�����������",	"���  �ù�......",		10000,	0,	7,7,
0,NULL,NULL,NULL,0,0,0,0
};

struct goodsofpip pipotherlist[] = {
0,"��Ʒ��",	"˵��buy",	"˵��feed",			0,	0,	0,0,
1,"�ָ������",	"���������",	"�����С����������...",	50,	0,	5,5,
2,"�ٿ�ȫ��",	"֪ʶ����Դ",	"�鱾��С������������������...",100,	0,	6,6,
0,NULL,NULL,NULL,0,0,0,0
};

/*--------------------------------------------------------------------------*/
/*  ���������趨                                                            */
/*--------------------------------------------------------------------------*/
struct weapon
{
  char *name;           /*����*/  
  int needmaxhp;	/*��Ҫhp*/
  int needmaxmp;	/*��Ҫmp*/
  int needspeed;	/*��Ҫ��speed*/
  int attack;		/*����*/
  int resist;		/*����*/
  int speed;		/*�ٶ�*/
  int cost;		/*���*/
  int sell;		/*����*/
  int special;		/*�ر�*/
  int map;		/*ͼ��*/

};
typedef struct weapon weapon;

/*����,��hp,��mp,��speed,����,����,�ٶ�,���,����,�ر�,ͼ��*/
struct weapon headlist[] = {
"����װ��",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�ܽ�ñ��",  0,  0,  0,  0,  5,  0,   500,   300,0,0,	
"ţƤСñ",  0,  0,  0,  0, 10,  0,  3500,  1000,0,0,
"  ��ȫñ", 60,  0,  0,  0, 20,  0,  5000,  3500,0,0,
"����ͷ��",150, 50,  0,  0, 30,  0, 10000,  6000,0,0,
"ħ������",100,150,  0,  0, 25,  0, 50000, 10000,0,0, 
"�ƽ�ʥ��",300,300,300,  0,100,  0,300000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,   0,   0,0,0
};

/*����,��hp,��mp,��speed,����,����,�ٶ�,���,����,�ر�,ͼ��*/
struct weapon rhandlist[] = {
"����װ��",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"��ľ��",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"��������",  0,  0,  0, 10,  0,  0,  2500,  1000,0,0,
"��ͭ��",   50,  0,  0, 20,  0,  0,  6000,  4000,0,0,
"���׽�",   80,  0,  0, 30,  0,  0, 10000,  8000,0,0,
"����",  100, 20,  0, 40,  0,  0, 15000, 10000,0,0, 
"���齣",  100, 40,  0, 35, 20,  0, 15000, 10000,0,0,
"ʨͷ����",150,  0,  0, 60,  0,  0, 35000, 20000,0,0,
"������",  200,  0,  0,100,  0,  0, 50000, 25000,0,0,
"�ƽ�ʥ��",300,300,300,100, 20,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*����,��hp,��mp,��speed,����,����,�ٶ�,���,����,�ر�,ͼ��*/
struct weapon lhandlist[] = {
"����װ��",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"��ľ��",    0,  0,  0,  5,  0,  0,  1000,   700,0,0,	
"��������",  0,  0,  0, 10,  0,  0,  1500,  1000,0,0,
"ľ��",	     0,  0,  0,  0, 10,  0,  2000,  1500,0,0,
"����ֶ�", 60,  0,  0,  0, 25,  0,  5000,  3000,0,0,
"�׽�֮��", 80,  0,  0, 10, 40,  0, 15000, 10000,0,0,
"ħ����",   80,100,  0, 20, 60,  0, 80000, 50000,0,0,
"�ƽ�ʥ��",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*����,��hp,��mp,��speed,����,����,�ٶ�,���,����,�ر�,ͼ��*/
struct weapon bodylist[] = {
"����װ��",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�ܽ��м�", 40,  0,  0,  0,  5,  0,  1000,   700,0,0,	
"�ؼ�Ƥ��", 50,  0,  0,  0, 10,  0,  2500,  1000,0,0,
"��������", 80,  0,  0,  0, 25,  0,  5000,  3500,0,0,
"ħ������", 80, 40,  0,  0, 20, 20, 15500, 10000,0,0,
"�׽����",100, 30,  0,  0, 40, 20, 30000, 20000,0,0, 
"�ƽ�ʥ��",300,300,300, 30,100,  0,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,     0,   0,0,0
};

/*����,��hp,��mp,��speed,����,����,�ٶ�,���,����,�ر�,ͼ��*/
struct weapon footlist[] = {
"����װ��",  0,  0,  0,  0,  0,  0,     0,     0,0,0,
"�ܽ���Ь",  0,  0,  0,  0,  0, 10,   800,   500,0,0,
"����ľ��",  0,  0,  0, 15,  0, 10,  1000,   700,0,0, 	
"�ؼ���Ь",  0,  0,  0,  0, 10, 10,  1500,  1000,0,0,
"NIKE�˶�Ь",70, 0,  0,  0, 10, 40,  8000,  5000,0,0,
"����Ƥѥ", 80, 20,  0, 10, 25, 20, 12000,  8000,0,0,
"����ħѥ",100,100,  0, 30, 50, 60, 25000, 10000,0,0,
"�ƽ�ʥѥ",300,300,300, 50,100,100,150000,100000,0,0,
NULL,        0,  0,  0,  0,  0,  0,    0,   0,0,0
};

/*---------------------------------------------------------------------------*/
/* �̵�ѡ��:ʳ�� ��ʳ ���� ��� �鱾                                       */
/* ��ʽ��                                                                    */
/*---------------------------------------------------------------------------*/

int pip_store_food()
{
    int num[3];
    num[0]=2;
    num[1]=d.food;
    num[2]=d.cookie;
    pip_buy_goods_new(1,pipfoodlist,num);
    d.food=num[1];
    d.cookie=num[2];
    return 0;
}

int pip_store_medicine()
{
    int num[5];
    num[0]=4;
    num[1]=d.bighp;
    num[2]=d.medicine;
    num[3]=d.ginseng;
    num[4]=d.snowgrass;
    pip_buy_goods_new(2,pipmedicinelist,num);
    d.bighp=num[1];
    d.medicine=num[2];
    d.ginseng=num[3];
    d.snowgrass=num[4];
    return 0;
}

int pip_store_other()
{
    int num[3];
    num[0]=2;
    num[1]=d.playtool;
    num[2]=d.book;
    pip_buy_goods_new(3,pipotherlist,num);
    d.playtool=num[1];
    d.book=num[2];
    return 0;
}

int pip_store_weapon_head()	/*ͷ������*/
{
     d.weaponhead=pip_weapon_doing_menu(d.weaponhead,0,headlist);
     return 0; 
}
int pip_store_weapon_rhand()	/*��������*/
{
     d.weaponrhand=pip_weapon_doing_menu(d.weaponrhand,1,rhandlist);
     return 0;
}
int pip_store_weapon_lhand()    /*��������*/
{
     d.weaponlhand=pip_weapon_doing_menu(d.weaponlhand,2,lhandlist);
     return 0;
}
int pip_store_weapon_body()	/*��������*/
{
     d.weaponbody=pip_weapon_doing_menu(d.weaponbody,3,bodylist);
     return 0;
}
int pip_store_weapon_foot()     /*�㲿����*/
{
     d.weaponfoot=pip_weapon_doing_menu(d.weaponfoot,4,footlist);
     return 0;
}


int 
pip_buy_goods_new(mode,p,oldnum)
int mode;
int oldnum[];
struct goodsofpip *p;
{
    char *shopname[4]={"����","�����̵�","�ǿ�ҩ��","ҹ�����"};
    char inbuf[256];
    char genbuf[20];
    long smoney;
    int oldmoney;
    int i,pipkey,choice;
    oldmoney=d.money;
    do
    {
	    clrchyiuan(6,18);
	    move(6,0);
	    sprintf(inbuf,"[1;31m  ��[41;37m ��� [0;1;31m��[41;37m ��      Ʒ [0;1;31m����[41;37m Ч            �� [0;1;31m����[41;37m ��     �� [0;1;31m��[37;41m ӵ������ [0;1;31m��[0m  ");
	    prints(inbuf);
	    for(i=1;i<=oldnum[0];i++)
	    {
		    move(7+i,0);
		    sprintf(inbuf,"     [1;35m[[37m%2d[35m]     [36m%-10s      [37m%-14s        [1;33m%-10d   [1;32m%-9d    [0m",
		    p[i].num,p[i].name,p[i].msgbuy,p[i].money,oldnum[i]);
		    prints(inbuf);
	    }
	    clrchyiuan(19,24);
	    move(b_lines,0); 
	    sprintf(inbuf,"[1;44;37m  %8sѡ��  [46m  [B]������Ʒ  [S]������Ʒ  [Q]������                         [m",shopname[mode]);
	    prints(inbuf);
	    pipkey=egetch(); 
	    switch(pipkey)  
	    {
		case 'B':
		case 'b':      
			move(b_lines-1,1);
			sprintf(inbuf,"��Ҫ����ɶ��? [0]�������� [1��%d]��Ʒ�̺�",oldnum[0]);
#ifdef MAPLE
			getdata(b_lines-1,1,inbuf,genbuf, 3, LCECHO,"0");
#else
                        getdata(b_lines-1,1,inbuf,genbuf, 3, DOECHO,YEA);
                        if ((genbuf[0] >= 'A') && (genbuf[0] <= 'Z'))
                                genbuf[0] = genbuf[0] | 32;
#endif  // END MAPLE

     			choice=atoi(genbuf);
			if(choice>=1 && choice<=oldnum[0])
			{
				clrchyiuan(6,18);
				if(rand()%2>0)
					show_buy_pic(p[choice].pic1);
				else
					show_buy_pic(p[choice].pic2);
				move(b_lines-1,0);
				clrtoeol();  
				move(b_lines-1,1);       
				smoney=0;
				if(mode==3)
					smoney=1;
				else
				{
					sprintf(inbuf,"��Ҫ������Ʒ [%s] ���ٸ���?(���� %d)",p[choice].name,d.money/p[choice].money);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0);
#else
                                        getdata(b_lines-1,1,inbuf,genbuf,6, DOECHO, YEA);
#endif  // END MAPLE
					smoney=atoi(genbuf);
				}
				if(smoney<0)
				{
					pressanykey("��������...");
				}
				else if(d.money<smoney*p[choice].money)
				{
					pressanykey("���Ǯû��������..");
				}
				else
				{
					sprintf(inbuf,"ȷ��������Ʒ [%s] ���� %d ����?(������� %d) [y/N]:",p[choice].name,smoney,smoney*p[choice].money);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0); 
#else
                                        getdata(b_lines-1,1,inbuf,genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
					if(genbuf[0]=='y' || genbuf[0]=='Y')
					{
						oldnum[choice]+=smoney;
						d.money-=smoney*p[choice].money;
						sprintf(inbuf,"�ϰ������%d��%s",smoney,p[choice].name);
						pressanykey(inbuf);
						pressanykey(p[choice].msguse);
						if(mode==3 && choice==1)
						{
							d.happy+=rand()%10+20*smoney;
							d.satisfy+=rand()%10+20*smoney;
						}
						if(mode==3 && choice==2)
						{
							d.happy+=(rand()%2+2)*smoney;
							d.wisdom+=(2+10/(d.wisdom/100+1))*smoney;
							d.character+=(rand()%4+2)*smoney;
							d.art+=(rand()%2+1)*smoney;
						}
					}
					else
					{
						pressanykey("��������...");
					}
				}
			}
			else
			{
				sprintf(inbuf,"��������.....");
				pressanykey(inbuf);            
			}
			break;     
     
 		case 'S':
 		case 's':
 			if(mode==3)
 			{
 				pressanykey("��Щ�����������....");
 				break;
 			}
			move(b_lines-1,1);
			sprintf(inbuf,"��Ҫ����ɶ��? [0]�������� [1��%d]��Ʒ�̺�",oldnum[0]);
#ifdef MAPLE
			getdata(b_lines-1,1,inbuf,genbuf, 3, LCECHO,"0");
#else
                        getdata(b_lines-1,1,inbuf,genbuf, 3, DOECHO,YEA);
                        if ((genbuf[0] >= 'A') && (genbuf[0] <= 'Z'))
                                genbuf[0] = genbuf[0] | 32;
#endif  // END MAPLE
     			choice=atoi(genbuf);
			if(choice>=1 && choice<=oldnum[0])
			{
				clrchyiuan(6,18);
				if(rand()%2>0)
					show_buy_pic(p[choice].pic1);
				else
					show_buy_pic(p[choice].pic2);
				move(b_lines-1,0);
				clrtoeol();  
				move(b_lines-1,1);       
				smoney=0;
				sprintf(inbuf,"��Ҫ������Ʒ [%s] ���ٸ���?(���� %d)",p[choice].name,oldnum[choice]);
#ifdef MAPLE
				getdata(b_lines-1,1,inbuf,genbuf,6, 1, 0); 
#else
                                getdata(b_lines-1,1,inbuf,genbuf,6, DOECHO, YEA);
#endif  // END MAPLE
				smoney=atoi(genbuf);
				if(smoney<0)
				{
					pressanykey("��������...");
				}
				else if(smoney>oldnum[choice])
				{
					sprintf(inbuf,"��� [%s] û���������",p[choice].name);
					pressanykey(inbuf);
				}
				else
				{
					sprintf(inbuf,"ȷ��������Ʒ [%s] ���� %d ����?(������ %d) [y/N]:",p[choice].name,smoney,smoney*p[choice].money*8/10);
#ifdef MAPLE
					getdata(b_lines-1,1,inbuf,genbuf, 2, 1, 0);
#else
                                        getdata(b_lines-1,1,inbuf,genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
					if(genbuf[0]=='y' || genbuf[0]=='Y')
					{
						oldnum[choice]-=smoney;
						d.money+=smoney*p[choice].money*8/10;
						sprintf(inbuf,"�ϰ����������%d��%s",smoney,p[choice].name);
						pressanykey(inbuf);
					}
					else
					{
						pressanykey("��������...");
					}
				}
			}
			else
			{
				sprintf(inbuf,"��������.....");
				pressanykey(inbuf);            
			}
			break;
		case 'Q':
		case 'q':
			sprintf(inbuf,"��Ǯ���׹� %d Ԫ,�뿪 %s ",d.money-oldmoney,shopname[mode]);
			pressanykey(inbuf);
			break;
#ifdef MAPLE
		case Ctrl('R'):
			if (currutmp->msgs[0].last_pid)
			{
				show_last_call_in();
				my_write(currutmp->msgs[0].last_pid, "ˮ�򶪻�ȥ��");
			}
			break;
#endif  // END MAPLE
	    }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));    
  return 0;
}

int
pip_weapon_doing_menu(variance,type,p)               /* ���������� */
int variance;
int type;
struct weapon *p;
{
  time_t now;
  register int n=0;
  register char *s;
  char buf[256];
  char ans[5];
  char shortbuf[100];
  char menutitle[5][11]={"ͷ��װ����","����װ����","����װ����","����װ����","�㲿װ����"};  
  int pipkey;
  char choicekey[5];
  int choice;
  
  do{
   clear();
   showtitle(menutitle[type], BoardName);
   show_weapon_pic(0);
/*   move(10,2); 
   sprintf(buf,"[1;37m�ֽ�����:����Max:[36m%-5d[37m  ����Max:[36m%-5d[37m  ����:[36m%-5d[37m  ����:[36m%-5d[37m  �ٶ�:[36m%-5d [m",
           d.maxhp,d.maxmp,d.attack,d.resist,d.speed);
   prints(buf);*/
   move(11,2);
   sprintf(buf,"[1;37;41m [NO]  [������]  [����]  [����]  [�ٶ�]  [����]  [����]  [�ٶ�]  [��  ��] [m");
   prints(buf);
   move(12,2);
   sprintf(buf," [1;31m����[37m��ɫ ���Թ���[31m����[32m��ɫ ӵ��װ��[31m����[33m��ɫ ǮǮ����[31m����[35m��ɫ ��������[31m����[m");
   prints(buf); 

   n=0;
   while (s = p[n].name)
   {   
     move(13+n,2);
     if(variance!=0 && variance==(n))/*�����е�*/
     {
      sprintf(buf, 
      "[1;32m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);        
     }
     else if(d.maxhp < p[n].needmaxhp || d.maxmp < p[n].needmaxmp || d.speed < p[n].needspeed )/*��������*/
     {
      sprintf(buf, 
      "[1;35m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);
     }

     else if(d.money < p[n].cost)  /*Ǯ������*/
     {
      sprintf(buf, 
      "[1;33m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);    
     }    
     else
     {
      sprintf(buf, 
      "[1;37m (%2d)  %-10s %4d    %4d    %4d    %4d    %4d    %4d    %6d[m",     
      n,p[n].name,p[n].needmaxhp,p[n].needmaxmp,p[n].needspeed,
      p[n].attack,p[n].resist,p[n].speed,p[n].cost);        
     } 
     prints(buf);
     n++;
   }
   move(b_lines,0); 
   sprintf(buf,"[1;44;37m  ��������ѡ��  [46m  [B]��������  [S]����װ��  [W]��������  [Q]������            [m");
   prints(buf);   
   now=time(0);
   pip_time_change(now);
   pipkey=egetch();
   pip_time_change(now);

   switch(pipkey)  
   {
    case 'B':      
    case 'b':
     move(b_lines-1,1);
     sprintf(shortbuf,"��Ҫ����ɶ��? ���ǮǮ[%d]Ԫ:[����]",d.money);
     prints(shortbuf);
#ifdef MAPLE
     getdata(b_lines-1,1,shortbuf,choicekey, 4, LCECHO,"0");
#else
     getdata(b_lines-1,1,shortbuf,choicekey, 4, DOECHO,YEA);
     if ((choicekey[0] >= 'A') && (choicekey[0] <= 'Z'))
             choicekey[0] = choicekey[0] | 32;
#endif  // END MAPLE
     choice=atoi(choicekey);
     if(choice>=0 && choice<=n)
     {
       move(b_lines-1,0);
       clrtoeol();  
       move(b_lines-1,1);       
       if(choice==0)     /*���*/
       { 
          sprintf(shortbuf,"��������...");
          pressanykey(shortbuf);
       }
      
       else if(variance==choice)  /*���Ѿ�����*/
       {
          sprintf(shortbuf,"�����Ѿ��� %s ��",p[variance].name);
          pressanykey(shortbuf);      
       }
      
       else if(p[choice].cost >= (d.money+p[variance].sell))  /*Ǯ����*/
       {
          sprintf(shortbuf,"���Ҫ %d Ԫ�����Ǯ������!",p[choice].cost);
          pressanykey(shortbuf);      
       }      
     
       else if(d.maxhp < p[choice].needmaxhp || d.maxmp < p[choice].needmaxmp 
               || d.speed < p[choice].needspeed ) /*��������*/
       {
          sprintf(shortbuf,"��ҪHP %d MP %d SPEED %d �",
                p[choice].needmaxhp,p[choice].needmaxmp,p[choice].needspeed);
          pressanykey(shortbuf);            
       }
       else  /*˳������*/
       {
          sprintf(shortbuf,"��ȷ��Ҫ���� %s ��?($%d) [y/N]",p[choice].name,p[choice].cost);
#ifdef MAPLE
          getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
#else
          getdata(b_lines-1,1,shortbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
          if(ans[0]=='y' || ans[0]=='Y')
          {
              sprintf(shortbuf,"С���Ѿ�װ���� %s ��",p[choice].name);
              pressanykey(shortbuf);
              d.attack+=(p[choice].attack-p[variance].attack);
              d.resist+=(p[choice].resist-p[variance].resist);
              d.speed+=(p[choice].speed-p[variance].speed);
              d.money-=(p[choice].cost-p[variance].sell);
              variance=choice;
          }
          else
          {
              sprintf(shortbuf,"��������.....");
              pressanykey(shortbuf);            
          }
       }
     }       
     break;     
     
   case 'S':
   case 's':
     if(variance!=0)
     { 
        sprintf(shortbuf,"��ȷ��Ҫ����%s��? ����:%d [y/N]",p[variance].name,p[variance].sell);
#ifdef MAPLE
        getdata(b_lines-1,1,shortbuf, ans, 2, 1, 0); 
#else
        getdata(b_lines-1,1,shortbuf, ans, 2, DOECHO, YEA);
#endif  // END MAPLE
        if(ans[0]=='y' || ans[0]=='Y')
        {
           sprintf(shortbuf,"װ�� %s ���� %d",p[variance].name,p[variance].sell);
           d.attack-=p[variance].attack;
           d.resist-=p[variance].resist;
           d.speed-=p[variance].speed;
           d.money+=p[variance].sell;
           pressanykey(shortbuf);
           variance=0;
        }
        else
        {
           sprintf(shortbuf,"ccc..�һ���ת����...");
           pressanykey(shortbuf);         
        }
     }
     else if(variance==0)
     {
        sprintf(shortbuf,"�㱾����û��װ����...");
        pressanykey(shortbuf);
        variance=0;
     }
     break;
                      
   case 'W':
   case 'w':
     pip_data_list();   
     break;    
   
#ifdef MAPLE
   case Ctrl('R'):
     if (currutmp->msgs[0].last_pid)
     {
       show_last_call_in();
       my_write(currutmp->msgs[0].last_pid, "ˮ�򶪻�ȥ��");
     }
     break;
#endif  // END MAPLE
   }
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));
    
  return variance;
}
