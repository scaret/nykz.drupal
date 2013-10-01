/*--------------------------------------------------------------------------*/
/*  С�������趨                      					    */
/*--------------------------------------------------------------------------*/
struct chicken {
 /*---����������---*/ //14
 char name[20];		/*��    ��*/
 char birth[21];	/*��    ��*/
 int year;		/*����  ��*/
 int month;		/*����  ��*/
 int day; 		/*����  ��*/
 int sex;		/*��    �� 1:��   2:��  */
 int death;             /*1:  ���� 2:���� 3:���*/
 int nodone;		/*1:  δ��*/
 int relation;		/*���˹�ϵ*/
 int liveagain;		/*�������*/
 int dataB;
 int dataC;
 int dataD;
 int dataE;
  
 /*---����Ĳ���---*/ //12
 int hp;		/*��    ��*/
 int maxhp;             /*�������*/
 int weight;            /*��    ��*/
 int tired;		/*ƣ �� ��*/
 int sick;		/*��    ��*/
 int shit;		/*�� �� ��*/ 
 int wrist;		/*��    ��*/
 int bodyA;
 int bodyB;
 int bodyC;
 int bodyD;
 int bodyE;
 
 /*---���۵Ĳ���---*/ //9
 int social;		/*�罻����*/
 int family;		/*��������*/
 int hexp;		/*ս������*/
 int mexp;		/*ħ������*/
 int tmpA;
 int tmpB;
 int tmpC;
 int tmpD;
 int tmpE;
 
 /*---ս���ò���---*/ //13
 int mp;		/*��    ��*/
 int maxmp;             /*�����*/
 int attack;		/*�� �� ��*/
 int resist;		/*�� �� ��*/
 int speed;		/*��    ��*/
 int hskill;		/*ս������*/
 int mskill;		/*ħ������*/
 int mresist;		/*��ħ����*/
 int magicmode;		/*ħ����̬*/
 int fightB;
 int fightC;
 int fightD;
 int fightE;
 

 /*---�����Ĳ���---*/ //10
 int weaponhead;	/*ͷ������*/
 int weaponrhand;	/*��������*/
 int weaponlhand;	/*��������*/
 int weaponbody;	/*��������*/
 int weaponfoot;	/*�ŵ�����*/ 
 int weaponA;
 int weaponB;
 int weaponC;
 int weaponD;
 int weaponE;
 
 /*---����������---*/ //17
 int toman;		/*���˽���*/ 
 int character;		/*�� �� ��*/ 
 int love;		/*��    ��*/ 
 int wisdom;		/*��    ��*/
 int art;		/*��������*/
 int etchics;		/*��    ��*/
 int brave;		/*��    ��*/
 int homework;		/*ɨ��ϴ��*/
 int charm;		/*��	��*/
 int manners;		/*��    ��*/
 int speech;		/*̸	��*/
 int cookskill;		/*��    �*/
 int learnA;
 int learnB;
 int learnC;
 int learnD;
 int learnE;
 
 
 /*---��״̬��ֵ---*/ //11
 int happy;		/*�� �� ��*/
 int satisfy;		/*�� �� ��*/
 int fallinlove;	/*����ָ��*/
 int belief;		/*��    ��*/
 int offense;		/*��    ��*/
 int affect;		/*��    ��*/
 int stateA;
 int stateB;
 int stateC;
 int stateD;
 int stateE;
 
 /*---�ԵĶ�����---*/ //9
 int food;		/*ʳ    ��*/
 int medicine;          /*��    ֥*/
 int bighp;             /*�� �� ��*/
 int cookie;		/*��    ʳ*/
 int ginseng;		/*ǧ����  */
 int snowgrass;		/*��ɽѩ��*/
 int eatC;
 int eatD;
 int eatE;
 
 /*---ӵ�еĶ���---*/ //8
 int book;		/*��    ��*/
 int playtool; 		/*��    ��*/
 int money;		/*��    Ǯ*/
 int thingA;		
 int thingB;		
 int thingC;
 int thingD;
 int thingE; 
 
 /*---��ȭ�Ĳ���---*/ //2
 int winn;		
 int losee;
 
 /*---�μ�����--*/ //16
 int royalA;		/*from����*/
 int royalB;		/*from����*/
 int royalC;		/*from����*/
 int royalD;		/*from��*/
 int royalE;		/*from��˾*/ 		
 int royalF;		/*from����*/
 int royalG;		/*from����*/
 int royalH;  		/*from����*/
 int royalI;		/*fromС��*/
 int royalJ;		/*from����*/
 int seeroyalJ;		/*�Ƿ��Ѿ�����������*/
 int seeA;
 int seeB;
 int seeC;
 int seeD;
 int seeE;

 /*---���----*///2
 int wantend;		/*20����*/
 /*1:��Ҫ��δ�� 2:��Ҫ���ѻ�  3:��Ҫ�ҵ������� 4:Ҫ��δ��  5:Ҫ���ѻ� 6:Ҫ�ҵ�������*/
 int lover;		/*����*/
 /*0:û�� 1:ħ�� 2:���� 3:A 4:B 5:C 6:D 7:E */ 
 
 /*-------��������--------*/
 int workA; /*����*/
 int workB; /*��ķ*/
 int workC; /*�õ�*/
 int workD; /*ũ��*/
 int workE; /*����*/
 int workF; /*����*/
 int workG; /*��̯*/
 int workH; /*��ľ*/
 int workI; /*����*/
 int workJ; /*����*/
 int workK; /*����*/
 int workL; /*��Ĺ*/
 int workM; /*�ҽ�*/
 int workN; /*�Ƽ�*/
 int workO; /*�Ƶ�*/
 int workP; /*ҹ*/
 int workQ;
 int workR;
 int workS;
 int workT;
 int workU;
 int workV;
 int workW;
 int workX;
 int workY;
 int workZ;
 
 /*-------�Ͽδ���--------*/
 int classA;
 int classB;
 int classC;
 int classD;
 int classE;
 int classF;
 int classG;
 int classH; 
 int classI;
 int classJ;
 int classK;
 int classL;
 int classM;
 int classN;
 int classO;
 
 /*---С����ʱ��---*/ //1
 time_t bbtime;
};
typedef struct chicken chicken;

/*--------------------------------------------------------------------------*/
/*  ѡ�����趨                                                              */
/*--------------------------------------------------------------------------*/
struct pipcommands
{
	int (*fptr) ();
	int key1 ;
	int key2 ;
};
typedef struct pipcommands pipcommands;

/*--------------------------------------------------------------------------*/
/*  ��������趨                                                            */
/*--------------------------------------------------------------------------*/
struct playrule
{
  //char *name;           /*����*/  
  char name[20];
  int hp;		/*����*/
  int maxhp; 		/*MAXHP*/
  int mp;		/*����*/
  int maxmp;		/*MAXMP*/
  int attack;		/*����*/
  int resist;		/*����*/
  int speed;		/*�ٶ�*/
  int money;		/*�Ʊ�*/
  char special[6];	/*�ر�*/  /*1:�»� 2:�±� */
  int map;		/*ͼ��*/
  int death;		/*����*/
};
typedef struct playrule playrule;

/* d.special�Ķ���            */
/* 00000 ��һϵ��ħ���˺���С */
/* 10000 ��һϵ��ħ���˺��ϴ� */
/* 11000 һ��ϵ��ħ���˺��ϴ� */
/* ��������                    */
/* ��ϵ ��ϵ ��ϵ ��ϵ ��ϵ */
/*struct playrule goodmanlist[] = {
}*/ 

#define PIP_BADMAN       (3)            /* �����������  */   

/*--------------------------------------------------------------------------*/
/*  ħ�������趨                                                            */
/*--------------------------------------------------------------------------*/
struct magicset
{
  //char *name;           /*ħ��������*/ 
  char name[20];
  int needmp;		/*��Ҫ�ķ���*/
  int hpmode;		/*0:���� 1:���� 2:���ֵ*/
  int hp;		/*ɱ��������*/
  int tiredmode;        /*0:���� 1:���� 2:��Сֵ*/
  int tired;		/*ƣ�Ͷȸı�*/
  int map;		/*ͼ��*/
};
typedef struct magicset magicset;


/*--------------------------------------------------------------------------*/
/*  �μ����������趨                                                        */
/*--------------------------------------------------------------------------*/
struct royalset
{
  //char *num;		/*����*/
  //char *name;           /*����������*/ 
  char num[2];
  char name[20];
  int needmode;		/*��Ҫ��mode*/ /*0:����Ҫ 1:���� 2:̸��*/
  int needvalue;	/*��Ҫ��value*/
  int addtoman;		/*����������*/
  int maxtoman;		/*�����*/
  //char *words1;
  //char *words2;
  char words1[40];
  char words2[40];
};
typedef struct royalset royalset;

#ifndef  MAPLE
#define  MSG_UID	"������ʹ���ߴ��ţ�"
#define  ERR_UID	"����û�����������"
#define  cuser		currentuser
#define  b_lines	t_lines-1
#endif  // End MAPLE
#define   cuser     currentuser 
//#define getdata(a, b, c , d, e, f, g) getdata(a,b,c,d,e,f,NULL,g)
#define pressanykey(a) temppress(a)
