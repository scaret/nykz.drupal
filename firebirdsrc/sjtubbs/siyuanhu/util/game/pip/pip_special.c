/*---------------------------------------------------------------------------*/
/* ����ѡ��:���� ���� ս�� �ݷ� ����                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <time.h>
#include "bbs.h"
#include "pip.h"
extern struct chicken d;
extern time_t start_time;
extern time_t lasttime;
//#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)

struct royalset royallist[] = {
{"T",	"�ݷö���",	  0,	0,	 0,	  0,"","" /*NULL,NULL*/},
{"A",	"�ǿ������",	  1,	10,	15,    	100,"����ã�����������..","�����ǿյİ�ȫ�Ǻ������.."},
{"B",	"�ǿգ�����",	  1,   100,	25,	200,"������ò��С��..��ϲ��...","����������ܱ���վ����ȫ����.."},
{"C",	"����󽫾�",	  1,   200,	30,	250,"������ࡣ������Ǹ�ս�ۺܾ����..","�����Ǹ߹����ŵ�С��..."},
{"D",	"��ı����",	  1,   300,	35,	300,"�Ұ�վ��������������..","��������ܺ���Ү..�Һ�ϲ���...:)"},
{"E",	"С��ʹվ��",	  1,   400,	35,	300,"����н���ࡣ��ܸ�����ʶ��...","���ŵ��㣬�����Ұ�����...."},
{"F",	"������վ��",	  1,   500,	40,	350,"��ÿɰ��..��ϲ�����....","����..����Ҫ������������..."},
{"G",	"��С��վ��",	  1,   550,	40,	350, "���㽲���ܿ����..����վ��һ������..","��������ϥ���ϣ����ҽ�����.."},
{"H",	"С�׿�վ��",	  1,   600,	50,     400,"һվ֮�������ش�ѽ..:)..","лл�����ҽ���..����Ҫ�����..."},
{"I",	"�ǿչ�ˮȺ",	  2,    60,	 0,	  0,"�����..��������..�ܿɰ�....","��  ����һ������ˮ��...."},
{"J",	"����˧���",	  0,	 0,	 0,	  0,"��ã�������٣��մ����ӱ߾�������Ϣ..","ϣ���´λ��ܼ�����...:)"},
//NULL,		NULL,NULL,    NULL,    NULL,NULL,NULL
//{NULL,			0,	0,	0,	0, NULL, NULL}
};

int pip_see_doctor()	/*��ҽ��*/
{
    char buf[256];
    long savemoney;
    savemoney=d.sick*25;
    if(d.sick<=0)
    {
    pressanykey("����..û����ҽԺ����..������..��~~");
    d.character-=(rand()%3+1);
    if(d.character<0)
      d.character=0;
    d.happy-=(rand()%3+3);
    d.satisfy-=rand()%3+2;
    }    
    else if(d.money < savemoney)
    {
     sprintf(buf,"��Ĳ�Ҫ�� %d Ԫ�....�㲻��Ǯ��...",savemoney);    
     pressanykey(buf);
    }
    else if(d.sick>0 && d.money >=savemoney)
    {
    d.tired-=rand()%10+20;
    if(d.tired<0)
       d.tired=0;
    d.sick=0;
    d.money=d.money-savemoney;
    move(4,0);
    show_special_pic(1);
    pressanykey("ҩ������..û�и�����!!");
    }
    return 0;
}

/*����*/
int pip_change_weight()
{
    char genbuf[5];
    char inbuf[256];
    int weightmp;
    
    move(b_lines-1, 0);
    clrtoeol();
    show_special_pic(2);
#ifdef MAPLE
    getdata(b_lines-1,1, "���ѡ����? [Q]�뿪:", genbuf, 2, 1, 0);    
#else
    getdata(b_lines-1,1, "���ѡ����? [Q]�뿪:", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
    if (genbuf[0]=='1'|| genbuf[0]=='2'|| genbuf[0]=='3'|| genbuf[0]=='4')
    { 
      switch(genbuf[0])
      {
        case '1':
          if(d.money<80)
          {
            pressanykey("��ͳ����Ҫ80Ԫ�....�㲻��Ǯ��...");
          }
          else
          {
#ifdef MAPLE
            getdata(b_lines-1,1, "�軨��80Ԫ(3��5����)����ȷ����? [y/N]", genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1, "�軨��80Ԫ(3��5����)����ȷ����? [y/N]", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
              weightmp=3+rand()%3;
              d.weight+=weightmp;
              d.money-=80;
              d.maxhp-=rand()%2;
              d.hp-=rand()%2+3;
              show_special_pic(3);
              sprintf(inbuf, "�ܹ�������%d����",weightmp);
              pressanykey(inbuf);
            }
            else
            {
              pressanykey("����ת����.....");
            }
          }
          break;
          
        case '2':
#ifdef MAPLE
          getdata(b_lines-1,1, "��һ����Ҫ30Ԫ����Ҫ�����ٹ�����? [��������]:", genbuf, 4, 1, 0);
#else
          getdata(b_lines-1,1, "��һ����Ҫ30Ԫ����Ҫ�����ٹ�����? [��������]:", genbuf, 4, DOECHO, YEA);
#endif  // END MAPLE
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("��������..������...");          
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "����%d����ܹ��軨����%dԪ��ȷ����? [y/N]",weightmp,weightmp*30);
#ifdef MAPLE
            getdata(b_lines-1,1,inbuf, genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1,inbuf, genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
                d.money-=weightmp*30;
                d.weight+=weightmp;
                d.maxhp-=(rand()%2+2);
                count_tired(5,8,"N",100,1);
                d.hp-=(rand()%2+3);
                d.sick+=rand()%10+5;
                show_special_pic(3);
                sprintf(inbuf, "�ܹ�������%d����",weightmp);
                pressanykey(inbuf);
            }
            else
            {
              pressanykey("����ת����.....");
            }
          }
          else
          {
            pressanykey("��Ǯû�������.......");            
          }
          break;        
          
        case '3':
          if(d.money<80)
          {
            pressanykey("��ͳ����Ҫ80Ԫ�....�㲻��Ǯ��...");
          }
          else
          {
#ifdef MAPLE
            getdata(b_lines-1,1, "�軨��80Ԫ(3��5����)����ȷ����? [y/N]", genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1, "�軨��80Ԫ(3��5����)����ȷ����? [y/N]", genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
              weightmp=3+rand()%3;
              d.weight-=weightmp;
              if(d.weight<0)
                   d.weight=0;
              d.money-=100;
              d.maxhp+=rand()%2;
              d.hp-=rand()%2+3;
              show_special_pic(4);
              sprintf(inbuf, "�ܹ�������%d����",weightmp);
              pressanykey(inbuf);
            }
            else
            {
              pressanykey("����ת����.....");
            }
          }        
          break;
        case '4':
#ifdef MAPLE
          getdata(b_lines-1,1, "��һ����Ҫ30Ԫ����Ҫ�����ٹ�����? [��������]:", genbuf, 4, 1, 0);
#else
          getdata(b_lines-1,1, "��һ����Ҫ30Ԫ����Ҫ�����ٹ�����? [��������]:", genbuf, 4, DOECHO, YEA);
#endif  // END MAPLE
          weightmp=atoi(genbuf);
          if(weightmp<=0)
          {
            pressanykey("��������..������...");
          }          
          else if(d.weight<=weightmp)
          {
            pressanykey("��û�������.....");
          }
          else if(d.money>(weightmp*30))
          {
            sprintf(inbuf, "����%d����ܹ��軨����%dԪ��ȷ����? [y/N]",weightmp,weightmp*30);
#ifdef MAPLE
            getdata(b_lines-1,1,inbuf, genbuf, 2, 1, 0);
#else
            getdata(b_lines-1,1,inbuf, genbuf, 2, DOECHO, YEA);
#endif  // END MAPLE
            if(genbuf[0]=='Y' || genbuf[0]=='y')
            {
                d.money-=weightmp*30;
                d.weight-=weightmp;
                d.maxhp-=(rand()%2+2);
                count_tired(5,8,"N",100,1);
                d.hp-=(rand()%2+3);
                d.sick+=rand()%10+5;
                show_special_pic(4);
                sprintf(inbuf, "�ܹ�������%d����",weightmp);
                pressanykey(inbuf);
            }
            else
            {
              pressanykey("����ת����.....");
            }
          }
          else
          {
            pressanykey("��Ǯû�������.......");            
          }
          break;
      }
    }
    return 0;
}


/*�μ�*/

int
pip_go_palace()
{
  pip_go_palace_screen(royallist);
  return 0;
}

int
pip_go_palace_screen(p) 
struct royalset *p;
{
  int n;
  int a;
  int b;
  int choice;
  int prince;  /*���ӻ᲻�����*/
  int pipkey;
  int change;
  char buf[256];
  char inbuf1[20];
  char inbuf2[20];
  char ans[5];
  char *needmode[3]={"      ","���Ǳ��֣�","̸�¼��ɣ�"};
  int save[11]={0,0,0,0,0,0,0,0,0,0,0};

  d.nodone=0; 
  do
  {
  clear();
  show_palace_pic(0);
  move(13,4);
  sprintf(buf,"[1;31m����������������[37;41m ������˾���  ��ѡ�������ݷõĶ��� [0;1;31m����������������[0m");
  prints(buf);
  move(14,4);
  sprintf(buf,"[1;31m��                                                                  ��[0m");
  prints(buf);

  for(n=0;n<5;n++)  
  {
    a=2*n+1;
    b=2*n+2;
    move(15+n,4);
    sprintf(inbuf1,"%-10s%3d",needmode[p[a].needmode],p[a].needvalue);
    if(n==4)
    { 
      sprintf(inbuf2,"%-10s",needmode[p[b].needmode]);
    }
    else
    {
      sprintf(inbuf2,"%-10s%3d",needmode[p[b].needmode],p[b].needvalue);
    }
    if((d.seeroyalJ==1 && n==4)||(n!=4))
      sprintf(buf,"[1;31m�� [36m([37m%s[36m) [33m%-10s  [37m%-14s     [36m([37m%s[36m) [33m%-10s  [37m%-14s[31m��[0m",    
             p[a].num,p[a].name,inbuf1,p[b].num,p[b].name,inbuf2);
    else
      sprintf(buf,"[1;31m�� [36m([37m%s[36m) [33m%-10s  [37m%-14s                                   [31m��[0m",    
             p[a].num,p[a].name,inbuf1);             
    prints(buf);
  }
  move(20,4);
  sprintf(buf,"[1;31m��                                                                  ��[0m");
  prints(buf);
  move(21,4);
  sprintf(buf,"[1;31m����������������������������������������������������������������������[0m");
  prints(buf);
  

   if(d.death==1 || d.death==2 || d.death==3)
     return 0;    
  /*���������Ѿ��������ֵ�л���*/
   save[1]=d.royalA;            /*from����*/
   save[2]=d.royalB;            /*from����*/
   save[3]=d.royalC;		/*from����*/
   save[4]=d.royalD;            /*from��*/
   save[5]=d.royalE;            /*from��˾*/
   save[6]=d.royalF;            /*from����*/
   save[7]=d.royalG;            /*from����*/
   save[8]=d.royalH;            /*from����*/
   save[9]=d.royalI;            /*fromС��*/
   save[10]=d.royalJ;           /*from����*/

   move(b_lines-1, 0);
   clrtoeol();
   move(b_lines-1,0);
   prints("[1;33m [������] %d/%d  [ƣ�Ͷ�] %d [0m",d.hp,d.maxhp,d.tired);
             
   move(b_lines, 0);
   clrtoeol();
   move(b_lines,0);
   prints(
   "[1;37;46m  �μ�ѡ��  [44m [��ĸ]ѡ�����ݷõ�����  [Q]�뿪�ǿ���˾���                    [0m");
   pipkey=egetch();
   choice=pipkey-64;
   if(choice<1 || choice>10)
      choice=pipkey-96;

   if((choice>=1 && choice<=10 && d.seeroyalJ==1)||(choice>=1 && choice<=9 && d.seeroyalJ==0))
   {
    d.social+=rand()%3+3;
    d.hp-=rand()%5+6;
    d.tired+=rand()%5+8;
    if(d.tired>=100)
    {
       d.death=1;
       pipdie("[1;31m������...[m  ",1);
    }
    if(d.hp<0)
    {
       d.death=1;
       pipdie("[1;31m������...[m  ",1);
    }
    if(d.death==1)
    {
      sprintf(buf,"������...���Ǳ���..");
    }
    else
    {
    if((p[choice].needmode==0)||
       (p[choice].needmode==1 && d.manners>=p[choice].needvalue)||
       (p[choice].needmode==2 && d.speech>=p[choice].needvalue))    
    {
      if(choice>=1 && choice<=9 && save[choice] >= p[choice].maxtoman)
      {
        if(rand()%2>0)
            sprintf(buf,"�ܺ�����ΰ����㽲���������Ҩ�...");
        else
            sprintf(buf,"�ܸ��������ݷ��ң����Ҳ��ܸ���ʲ����..");
      }
      else
      {
        change=0;
        if(choice>=1 && choice <=8 )
        {
          switch(choice)
          {
            case 1:
              change=d.character/5;
              break;
            case 2:
              change=d.character/8;
              break;
            case 3:
              change=d.charm/5;
              break;              
            case 4:
              change=d.wisdom/10;
              break;
            case 5:
              change=d.belief/10;
              break;
            case 6:
              change=d.speech/10;
              break;
            case 7:
              change=d.social/10;
              break;
            case 8:
              change=d.hexp/10;
              break;            
          }
          /*������ÿ�ε����������*/
          if(change > p[choice].addtoman)
             change=p[choice].addtoman;
          /*�������ԭ�ȵ�֮�������ܸ�������ֵʱ*/
          if((change+save[choice])>= p[choice].maxtoman)
             change=p[choice].maxtoman-save[choice];
          save[choice]+=change;
          d.toman+=change;
        }
        else if(choice==9)
        {
          save[9]=0;
          d.social-=13+rand()%4;
          d.affect+=13+rand()%4;
        }
        else if(choice==10 && d.seeroyalJ==1)
        {
            save[10]+=15+rand()%4;
            d.seeroyalJ=0;
        }
        if(rand()%2>0)
            sprintf(buf,"%s",p[choice].words1);
        else    
            sprintf(buf,"%s",p[choice].words2);
      }
    }
    else
    {
      if(rand()%2>0)
            sprintf(buf,"�Ҳ����������ļ�̸��....");
      else
            sprintf(buf,"����ֻû�����ļ�����ȥѧѧ���ǰ�....");    
    
    }
    }    
    pressanykey(buf);
   }
   d.royalA=save[1];
   d.royalB=save[2];
   d.royalC=save[3];
   d.royalD=save[4];
   d.royalE=save[5];
   d.royalF=save[6];
   d.royalG=save[7];
   d.royalH=save[8];
   d.royalI=save[9];
   d.royalJ=save[10];
  }while((pipkey!='Q')&&(pipkey!='q')&&(pipkey!=KEY_LEFT));

  pressanykey("�뿪�ǿ���˾�.....");  
  return 0;
}

int pip_query()  /*�ݷ�С��*/
{

#ifdef MAPLE
  userec muser;
#endif  // END MAPLE
  int id;
  char genbuf[STRLEN];

#ifndef MAPLE
  char *msg_uid = MSG_UID;
  char *err_uid = ERR_UID;
#endif  // END MAPLE

  stand_title("�ݷ�ͬ��");
  usercomplete(msg_uid, genbuf);
  if (genbuf[0])
  {
    move(2, 0);
    if (id = getuser(genbuf))
    {
        pip_read(genbuf);
        pressanykey("��Ħһ�±��˵�С��...:p");
    }
    else
    {
      outs(err_uid);
      clrtoeol();
    }
  }
  return 0;
}

int
pip_read(genbuf)
char *genbuf;
{
  FILE *fs;
  struct chicken d1;
  char buf[200];
  /*char yo[14][5]={"����","Ӥ��","�׶�","��ͯ","����","����","����",
                  "׳��","׳��","׳��","����","����","����","��ϡ"};*/
  char yo[12][5]={"����","Ӥ��","�׶�","��ͯ","����","����",
                  "����","׳��","����","����","��ϡ","����"};                  
  int pc1,age1,age=0;
  
  int year1,month1,day1,sex1,death1,nodone1,relation1,liveagain1,dataB1,dataC1,dataD1,dataE1;
  int hp1,maxhp1,weight1,tired1,sick1,shit1,wrist1,bodyA1,bodyB1,bodyC1,bodyD1,bodyE1;
  int social1,family1,hexp1,mexp1,tmpA1,tmpB1,tmpC1,tmpD1,tmpE1;
  int mp1,maxmp1,attack1,resist1,speed1,hskill1,mskill1,mresist1,magicmode1,fightB1,fightC1,fightD1,fightE1;
  int weaponhead1,weaponrhand1,weaponlhand1,weaponbody1,weaponfoot1,weaponA1,weaponB1,weaponC1,weaponD1,weaponE1;
  int toman1,character1,love1,wisdom1,art1,etchics1,brave1,homework1,charm1,manners1,speech1,cookskill1,learnA1,learnB1,learnC1,learnD1,learnE1;
  int happy1,satisfy1,fallinlove1,belief1,offense1,affect1,stateA1,stateB1,stateC1,stateD1,stateE1;
  int food1,medicine1,bighp1,cookie1,ginseng1,snowgrass1,eatC1,eatD1,eatE1;
  int book1,playtool1,money1,thingA1,thingB1,thingC1,thingD1,thingE1;
  int winn1,losee1;
  int royalA1,royalB1,royalC1,royalD1,royalE1,royalF1,royalG1,royalH1,royalI1,royalJ1,seeroyalJ1,seeA1,seeB1,seeC1,seeD1,seeE1;
  int wantend1,lover1;
  char name1[200];
  int classA1,classB1,classC1,classD1,classE1;
  int classF1,classG1,classH1,classI1,classJ1;
  int classK1,classL1,classM1,classN1,classO1;
  int workA1,workB1,workC1,workD1,workE1;
  int workF1,workG1,workH1,workI1,workJ1;
  int workK1,workL1,workM1,workN1,workO1;
  int workP1,workQ1,workR1,workS1,workT1;
  int workU1,workV1,workW1,workX1,workY1,workZ1;

#ifdef MAPLE
  sprintf(buf,"home/%s/new_chicken",genbuf);
  currutmp->destuid = genbuf;
#else
  sprintf(buf,"home/%c/%s/new_chicken",toupper(genbuf[0]),genbuf);
#endif  // END MAPLE

  if(fs=fopen(buf,"r"))
  {
    fread(&d1,sizeof(d1),1,fs);
    //fgets(buf, 80, fs);
    //age = ((time_t) atol(buf))/60/30;
	age = d1.bbtime/1800;
  
    if(age==0) /*����*/
       age1=0;
    else if( age==1) /*Ӥ��*/
       age1=1;
    else if( age>=2 && age<=5 ) /*�׶�*/
       age1=2;
    else if( age>=6 && age<=12 ) /*��ͯ*/
       age1=3;
    else if( age>=13 && age<=15 ) /*����*/
       age1=4;     
    else if( age>=16 && age<=18 ) /*����*/
       age1=5;     
    else if( age>=19 && age<=35 ) /*����*/
       age1=6;
    else if( age>=36 && age<=45 ) /*׳��*/
       age1=7;
    else if( age>=45 && age<=60 ) /*����*/
       age1=8;
    else if( age>=60 && age<=70 ) /*����*/
       age1=9;
    else if( age>=70 && age<=100 ) /*��ϡ*/
       age1=10;
    else if( age>100 ) /*����*/
       age1=11;
/*
    fscanf(fs,
    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
    &(year1),&(month1),&(day1),&(sex1),&(death1),&(nodone1),&(relation1),&(liveagain1),&(dataB1),&(dataC1),&(dataD1),&(dataE1),
    &(hp1),&(maxhp1),&(weight1),&(tired1),&(sick1),&(shit1),&(wrist1),&(bodyA1),&(bodyB1),&(bodyC1),&(bodyD1),&(bodyE1),
    &(social1),&(family1),&(hexp1),&(mexp1),&(tmpA1),&(tmpB1),&(tmpC1),&(tmpD1),&(tmpE1),
    &(mp1),&(maxmp1),&(attack1),&(resist1),&(speed1),&(hskill1),&(mskill1),&(mresist1),&(magicmode1),&(fightB1),&(fightC1),&(fightD1),&(fightE1),
    &(weaponhead1),&(weaponrhand1),&(weaponlhand1),&(weaponbody1),&(weaponfoot1),&(weaponA1),&(weaponB1),&(weaponC1),&(weaponD1),&(weaponE1),
    &(toman1),&(character1),&(love1),&(wisdom1),&(art1),&(etchics1),&(brave1),&(homework1),&(charm1),&(manners1),&(speech1),&(cookskill1),&(learnA1),&(learnB1),&(learnC1),&(learnD1),&(learnE1),
    &(happy1),&(satisfy1),&(fallinlove1),&(belief1),&(offense1),&(affect1),&(stateA1),&(stateB1),&(stateC1),&(stateD1),&(stateE1),
    &(food1),&(medicine1),&(bighp1),&(cookie1),&(ginseng1),&(snowgrass1),&(eatC1),&(eatD1),&(eatE1),
    &(book1),&(playtool1),&(money1),&(thingA1),&(thingB1),&(thingC1),&(thingD1),&(thingE1),
    &(winn1),&(losee1),
    &(royalA1),&(royalB1),&(royalC1),&(royalD1),&(royalE1),&(royalF1),&(royalG1),&(royalH1),&(royalI1),&(royalJ1),&(seeroyalJ1),&(seeA1),&(seeB1),&(seeC1),&(seeD1),&(seeE1),
    &(wantend1),&(lover1),
    name1,
    &(classA1),&(classB1),&(classC1),&(classD1),&(classE1),
    &(classF1),&(classG1),&(classH1),&(classI1),&(classJ1),
    &(classK1),&(classL1),&(classM1),&(classN1),&(classO1),
    &(workA1),&(workB1),&(workC1),&(workD1),&(workE1),
    &(workF1),&(workG1),&(workH1),&(workI1),&(workJ1),
    &(workK1),&(workL1),&(workM1),&(workN1),&(workO1),
    &(workP1),&(workQ1),&(workR1),&(workS1),&(workT1),
    &(workU1),&(workV1),&(workW1),&(workX1),&(workY1),&(workZ1)
  );
*/
  fclose(fs);

  move(1,0);
  clrtobot();
#ifdef MAPLE
  prints("����%s����С����\n",xuser.userid);
#else
  prints("����%s����С����\n",genbuf);
#endif  // END MAPLE

  if (d1.death==0)
  {
   prints("[1;32mName��%-10s[m  ���գ�%02d��%02d��%2d��   ���䣺%2d��  ״̬��%s  ǮǮ��%d\n"
          "������%3d/%-3d  ���֣�%-4d  ���⣺%-4d  ���ʣ�%-4d  �ǻۣ�%-4d  ���أ�%-4d\n"
          "���裺%-4d   ʳ�%-4d  ��ʳ��%-4d  ƣ�ͣ�%-4d  ���ࣺ%-4d  ������%-4d\n",
        d1.name,d1.year,d1.month,d1.day,age,yo[age1],d1.money,
        d1.hp,d1.maxhp,d1.happy,d1.satisfy,d1.character,d1.wisdom,d1.weight,
        d1.bighp,d1.food,d1.cookie,d1.tired,d1.shit,d1.sick);

    move(5,0);
    switch(age1)
    {
     case 0:       
     case 1:
     case 2:
       if(d1.weight<=(60+10*age-30))
          show_basic_pic(1);
       else if(d1.weight>(60+10*age-30) && d1.weight<(60+10*age+30))
          show_basic_pic(2);
       else if(d1.weight>=(60+10*age+30))
          show_basic_pic(3);
       break;
     case 3:
     case 4:
       if(d1.weight<=(60+10*age-30))
          show_basic_pic(4);
       else if(d1.weight>(60+10*age-30) && d1.weight<(60+10*age+30))
          show_basic_pic(5);
       else if(d1.weight>=(60+10*age+30))
          show_basic_pic(6);
       break;
     case 5:
     case 6:
       if(d1.weight<=(60+10*age-30))
          show_basic_pic(7);
       else if(d1.weight>(60+10*age-30) && d1.weight<(60+10*age+30))
          show_basic_pic(8);
       else if(d1.weight>=(60+10*age+30))
          show_basic_pic(9);
       break;     
     case 7:
     case 8:
       if(d1.weight<=(60+10*age-30))
          show_basic_pic(10);
       else if(d1.weight>(60+10*age-30) && d1.weight<(60+10*age+30))
          show_basic_pic(11);
       else if(d1.weight>=(60+10*age+30))
          show_basic_pic(12);
       break;     
     case 9:
       show_basic_pic(13);
       break;
     case 10:
     case 11:
       show_basic_pic(13);
       break;
    }
   move(18,0);
   if(d1.shit==0) prints("��Ǭ��..");
   if(d1.shit>40&&d1.shit<60) prints("������..");
   if(d1.shit>=60&&d1.shit<80) prints("�ó��..");
   if(d1.shit>=80&&d1.shit<100) prints("[1;34m�������..[m");
   if(d1.shit>=100) {prints("[1;31m������..[m"); return -1;}

   pc1=hp1*100/d1.maxhp;
   if(pc1==0) {prints("������.."); return -1;}
   if(pc1<20) prints("[1;35mȫ��������.�������.[m");
   if(pc1<40&&pc1>=20) prints("������̫��..��Ե㶫��..");
   if(pc1<100&&pc1>=80) prints("�š����ӱ���������..");
   if(pc1>=100) prints("[1;34m�������..[m");

   pc1=d1.tired;
   if(pc1<20) prints("���񶶶���..");
   if(pc1<80&&pc1>=60) prints("[1;34m�е�С��..[m");
   if(pc1<100&&pc1>=80) {prints("[1;31m����ร��첻����..[m"); }
   if(pc1>=100) {prints("������..."); return -1;}

   pc1=60+10*age;
   if(d1.weight<(pc1+30) && d1.weight>=(pc1+10)) prints("�е�С��..");
   if(d1.weight<(pc1+50) && d1.weight>=(pc1+30)) prints("̫����..");
   if(d1.weight>(pc1+50)) {prints("������..."); return -1;}

   if(d1.weight<(pc1-50)) {prints("������.."); return -1;}
   if(d1.weight>(pc1-30) && d1.weight<=(pc1-10)) prints("�е�С��..");
   if(d1.weight>(pc1-50) && d1.weight<=(pc1-30)) prints("̫����..");

   if(d1.sick<75&&d1.sick>=50) prints("[1;34m������..[m");
   if(d1.sick<100&&d1.sick>=75) {prints("[1;31m����!!..[m"); }
   if(d1.sick>=100) {prints("������.!."); return -1;}

   pc1=d1.happy;
   if(pc1<20) prints("[1;31m�ܲ�����..[m");
   if(pc1<40&&pc1>=20) prints("������..");
   if(pc1<95&&pc1>=80) prints("����..");
   if(pc1<=100&&pc1>=95) prints("�ܿ���..");

   pc1=d1.satisfy;
   if(pc1<40) prints("[31;1m������..[m");
   if(pc1<95&&pc1>=80) prints("����..");
   if(pc1<=100&&pc1>=95) prints("������..");
  }
  else if(d1.death==1)
  {
     show_die_pic(2);
     move(14,20);
     prints("������С���غ�������");
  } 
  else if(d1.death==2)
  {
     show_die_pic(3);
  }
  else if(d1.death==3)
  {
    move(5,0);
    outs("��Ϸ�Ѿ��浽�����....");
  }
  else
  {
    pressanykey("���������....");
  }
 }   /* ����С�� */
 else
 {
   move(1,0);
   clrtobot();
   pressanykey("��һ�ҵ���û����С��......");
 }
}
