// by zhch
// Last Modified by soff bitbbs.org on 12/10/2000 ��������
#include "../../include/bbs.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#define stty(fd, data) tcsetattr( fd, TCSANOW, data )
#define gtty(fd, data) tcgetattr( fd, data )
struct termios tty_state, tty_new ;
static int oy, ox, ok, on=-1;
int a[21][12]={
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,0,0,0,0,0,0,0,0,0,0,8,
8,8,8,8,8,8,8,8,8,8,8,8};
unsigned char buf[4000], userid[30]="unknown.", userip[60]="unknown.";
int bufip=0;
int dx[7][4][4], dy[7][4][4];
int d[7][4][4]={
0,1,4,5, 0,1,4,5, 0,1,4,5, 0,1,4,5,
4,5,6,7, 1,5,9,13, 4,5,6,7, 1,5,9,13,
0,1,5,6, 1,4,5,8, 0,1,5,6, 1,4,5,8,
1,2,4,5, 0,4,5,9, 1,2,4,5, 0,4,5,9, 
0,1,2,4, 0,1,5,9, 2,4,5,6, 0,4,8,9,
0,1,2,6, 1,5,8,9, 0,4,5,6, 0,1,4,8,
0,1,2,5, 1,4,5,9, 1,4,5,6, 0,4,5,8};
int k,n,y,x,e;
int newk=0;
int lines=0;
int score=0;
char tmp[200];
int delay, level;
char topID[20][20],topFROM[20][32];
int topT[20],topS[20];
int starttime;
 
get_tty()
{
    if(gtty(1,&tty_state) < 0) 
        return 0;
    return 1 ;
}

init_tty()
{
    long vdisable;

    memcpy( &tty_new, &tty_state, sizeof(tty_new)) ;
    tty_new.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ISIG);
    tty_new.c_cflag &= ~CSIZE;
    tty_new.c_cflag |= CS8;
    tty_new.c_cc[ VMIN ] = 1;
    tty_new.c_cc[ VTIME ] = 0;
    if ((vdisable = fpathconf(STDIN_FILENO, _PC_VDISABLE)) >= 0)
      {
      tty_new.c_cc[VSTART] = vdisable;
      tty_new.c_cc[VSTOP]  = vdisable;
      tty_new.c_cc[VLNEXT] = vdisable;
      }
    tcsetattr(1, TCSANOW, &tty_new);
}

reset_tty()
{
    stty(1,&tty_state) ;
}

int getch()
{
    int c,d,e;
    c=getch0();
    if(c==3||c==4||c==-1) quit(); 
    if(c!=27) return c;
    d=getch0();
    e=getch0();
    if(d==0) return c;
    if(e=='A') return 257;
    if(e=='B') return 258;
    if(e=='C') return 259;
    if(e=='D') return 260;
    return 0;
}

int getch0()
{
    char ch; 
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 50000;
    if(select(1, &rfds, NULL, NULL, &tv)) {
    	if(read(0, &ch, 1)<=0) exit(-1);
        return ch;
    }
    else
        return 0; 
}

int prints(char* s)
{
  sprintf(buf+bufip, s);
  bufip+=strlen(s);
}

int oflush()
{
  write(1,buf,bufip);
  bufip=0;
}

int move(int y1, int x1)
{
   sprintf(tmp,"[%d;%dH",y1+3,x1+1);
   prints(tmp);
}

int color(int c)
{
  static lastc=-1;
  if(c==lastc)return;
  lastc=c;
  if(c==4) c=12;
  sprintf(tmp,"[%d;%dm",c/8,c%8+30);
  prints(tmp);
}

int clear()
{
   sprintf(tmp,"[H[J");
   prints(tmp);
}

int clear2()
{
   sprintf(tmp,"[H[J");
   prints(tmp);
   color (8);
   move(3,0);
   prints("                            [33m������     \r\n");
   prints("                            ���ԩ�     \r\n");
   prints("                            ������     \r\n");
   prints("                            [34m������     \r\n");
   prints("                            ���ũ�     \r\n");
   prints("                            ������     \r\n");
   prints("                            [33m������     \r\n");
   prints("                            ���ԩ�     \r\n");
   prints("                            ������     \r\n");
   prints("                            [35m������     \r\n");
   prints("                            ���ҩ�     \r\n");
   prints("                            ������     \r\n");
   prints("                            [31m������     \r\n");
   prints("                            ���ɩ�     \r\n");
   prints("                            ������     \r\n");
   prints("                            [32m������     \r\n");
   prints("                            ���ө�     \r\n");
   prints("                            ������[m     \r\n");

   prints("��'CTRL+C'�˳�����Ϸ��");
   move(-2,0);
}

int sh2()
{ 
   if(oy==y && ox==x && ok==k && on==n) return;
   sh(oy, ox, ok, on, 0);
   oy=y; ox=x; ok=k; on=n;
   sh(oy,ox,ok,on,ok+1);
   oflush(); 
}

int sh(int y, int x, int k, int n, int c)
{
   if(n==-1) return;
   for(e=0;e<=3;e++)
   {
     move(y+dy[k][n][e],2*(x+dx[k][n][e]));
     color(c);
     if(c)prints("��");else prints("  ");
   }
   move(-2,0);
}

int show0()
{
  int y,x;
  for(y=0;y<=20;y++)
  {
    move(y,0);
    for(x=0;x<=11;x++)
    {
      color(a[y][x]);
      if(a[y][x])
        prints("��");
      else
        prints("  ");
    }
  }
  oflush();
}

main(int argc, char* argv[])
{
  if(argc>=2) strcpy(userid, argv[1]);
  if(argc>=3) strcpy(userip, argv[2]);
  starttime=time(0);
  get_tty();
  init_tty();
  intr();
  start();
  quit();
}

quit()
{
  reset_tty();
  color(7);
  sprintf(tmp, "Quit. Stay time: %d", time(0)-starttime);
  syslog(tmp);
  printf("[H[J��ӭ����, �ټ�!\r\n");
  fflush(stdout);
  usleep(500000);
  oflush();
  exit(0);
}

init_data()
{
  for(k=0;k<=6;k++)
  for(n=0;n<=3;n++)
  for(e=0;e<=3;e++)
  {
     dx[k][n][e]=d[k][n][e]%4;
     dy[k][n][e]=d[k][n][e]/4;
  }
  for(y=0;y<=19;y++)
  for(x=1;x<=10;x++)
  {
     a[y][x]=0;
  }
  srand(time(0));
  newk=rand()%7;
  level=0;
  delay=20;
  lines=0;
  score=0;
}

int crash2(int x, int y, int k, int n)
{
  for(e=0;e<=3;e++)
  if(a[y+dy[k][n][e]][x+dx[k][n][e]])return 1;
  return 0;
}

int bell()
{
  static char c=7;
  write(0,&c,1);
}

start()
{
  int c,t,first;
  win_showrec();
  while(1)
  {
    init_data();
    clear2();
    show0();
    first=1;
    while(1)
    {
      k=newk;
      newk=rand()%7;
      n=0;
      color(0);
      move(0,25);prints("                ");
      move(1,25);prints("                ");
      sh(0,14,newk,0,newk+1);
      n=0;
      x=3;y=0;
      sh2();
      if(first) {presskey();first=0;}
      if(crash2(x,y,k,n)) 
      {
         sprintf(tmp,"%d",lines); 
         syslog(tmp); 
         win_checkrec(score,lines);
         break;
      }
      t=times(0);
      while(1)
      {
        c=getch();
        if (c==27 ||c=='p'||c=='P') presskey();
        if (c==260||c=='a'||c=='A') if(!crash2(x-1,y,k,n)){x--;sh2();}
        if (c==259||c=='s'||c=='S') if(!crash2(x+1,y,k,n)){x++;sh2();}
        if (c=='b'||c=='B'||c==10) if(!crash2(x,y,k,(n+1)%4)){n=(n+1)%4;sh2();}
        if (c=='h'||c=='H'||c==257) if(!crash2(x,y,k,(n+3)%4)){n=(n+3)%4;sh2();}  
        if (c=='J'||c=='j') if(!crash2(x,y,k,(n+2)%4)){n=(n+2)%4;sh2();}
        if (c==' ') {while(!crash2(x,y+1,k,n))y++;sh2();down();break;}
        if (times(0)-t>delay||c==258||c=='z'||c=='Z')
        {
          t=times(0);
          if(crash2(x,y+1,k,n)){down();break;} else {y++;sh2();}  
        }
      }
    }
  }
}

down()
{
   for(e=0;e<=3;e++)
   a[y+dy[k][n][e]][x+dx[k][n][e]]=k+1;
   checklines();
   on=-1;
}

checklines()
{
   int y1,x1;
   int y2;
   int s=0, t=0;
   int getscore;
   for(y1=0;y1<=19;y1++)
   {
      for(x1=1;x1<=10;x1++)
      if(a[y1][x1]==0)break;
      if(x1<=10) continue;
      s=1;
      t++;
      printf("[37m[24;1H������[32m%3d", lines+1);
      if(lines==0) printf("                         ");
      for(y2=y1;y2>=1;y2--)
      for(x1=1;x1<=10;x1++)
        a[y2][x1]=a[y2-1][x1];  
      for(x1=1;x1<=10;x1++) 
        a[0][x1]=0;
      if((++lines)%30==0) 
        {delay*=.9; level++; bell();printf("[37m[24;23H�ȼ���[32m%3d", level);} 
   }
   if(t==0) getscore = 0;
   if(t==1) getscore = 1;
   if(t==2) getscore = 3;
   if(t==3) getscore = 7;
   if(t==4) getscore = 13;
   score += getscore;
   printf("[37m[24;12H������[32m%3d", score);
   if(s) {show0();fflush(stdout);}
}

syslog(char* cc )
{
   FILE *fp;
   time_t t;
   t=time(0);
   fp=fopen("tetris.log","a");
   fprintf(fp,"%s did %s on %s", userid, cc, ctime(&t));
   fclose(fp);
}

intr()
{
  clear();
  oflush();
  sprintf(tmp,"��ӭ���� %s. ������2000��9��13�ձ���Ϸ�ĵ� [1;33m%d[m λ������.\r\n\r\n", userid, count());
  prints(tmp);
  prints("��������: \r\n");
  prints("��: '[1;32ma[m', '[1;32m��[m';\r\n");
  prints("��: '[1;32ms[m', '[1;32m��[m';\r\n");
  prints("��: '[1;32mz[m', '[1;32m��[m';\r\n");
  prints("˳ʱ��ת��: '[1;32mb[m', <[1;32mCR[m>; ��ʱ��ת��: '[1;32mh[m', '[1;32m��[m'; 180��ת��: '[1;32mj[m';\r\n");
  prints("�콵: ' ', ��ͣ: <[1;32mESC[m>��<[1;32mp[m>.\r\n");
  prints("�˳�: '[1;32mCTRL+C[m', '[1;32mCTRL+D[m'.\r\n\r\n");
  oflush();
  getchar();
  clear();
  oflush();
}

int count()
{
  int c;
  FILE *fp;
  fp=fopen("tetris.count","r");
  if(fp==NULL) {system("echo 1 > tetris.count"); return 1;}
  fscanf(fp,"%d",&c);
  fclose(fp);
  fp=fopen("tetris.count","w");
  fprintf(fp,"%d",c+1);
  fclose(fp);
  return c+1;
}

int win_loadrec()
{
  FILE *fp;
  int n;
  for(n=0;n<=19;n++)
  {
    strcpy(topID[n],"unknown.");
    topT[n]=0;
    strcpy(topFROM[n],"unknown.");
  }
  fp=fopen("tetris.rec","r");
  if(fp==NULL){win_saverec();return;}
  for(n=0;n<=19;n++)
    fscanf(fp,"%s %d %d %s\n",topID[n],&topT[n],&topS[n],&topFROM[n]);
  fclose(fp);
}

int win_saverec()
{
  FILE *fp;
  int n;
  fp=fopen("tetris.rec","w");
  for(n=0;n<=19;n++)
    {
      fprintf(fp,"%s %d %d %s\n",topID[n],topT[n],topS[n],topFROM[n]);
    }
  fclose(fp);
}

int win_showrec()
{
  int n;
  win_loadrec();
  clear();
  prints("[44;37m                         -˼Դ��  TETRIS  ���а�-                                 \r\n[m");
  prints("[41m No.          ID        ����          ����                 ����                      [m\n\r");
  for(n=0;n<=19;n++)
  {
    sprintf(tmp, "[1;37m%3d[32m%13s[0;37m%12d%12d[m%29s\n\r",n+1,topID[n],topT[n],topS[n]
         ,topFROM[n]);
    prints(tmp);
  }
  prints("[41m                                                                               [m\n\r");
  oflush();
  presskey();
}

int win_checkrec(int sc,int dt)
{
  char id[30];
  int n;
  win_loadrec();
  strcpy(id,userid);
  for(n=0;n<=19;n++)
    if(!strcmp(topID[n],id))
    {
      if( sc>topS[n] || (sc==topS[n] && dt==topT[n]) )
      {
        topT[n]=dt;
        topS[n]=sc;
        strcpy(topFROM[n],userip);
        win_sort();
        win_saverec();
        win_showrec();
      }
      return;
    }
    if( sc>topS[19] || (sc==topS[19] && dt==topT[19]) )
  	{
    	strcpy(topID[19],id);
    	topS[19]=sc;
    	topT[19]=dt;
    	strcpy(topFROM[19],userip);
    	win_sort();
    	win_saverec();
    	win_showrec();
    	return;
  	}

}

int win_sort()
{
  int n,n2,tmp;
  char tmpID[30];
  clear();
  prints("ף��! ��ˢ�����Լ��ļ�¼!\r\n");
  oflush();
  presskey();
  for(n=0;n<=18;n++)
  for(n2=n+1;n2<=19;n2++)
    if(topS[n]<topS[n2])
    {
      tmp=topS[n];topS[n]=topS[n2];topS[n2]=tmp;
      tmp=topT[n];topT[n]=topT[n2];topT[n2]=tmp;
      strcpy(tmpID,topID[n]);strcpy(topID[n],topID[n2]);
      strcpy(topID[n2],tmpID);
      strcpy(tmpID,topFROM[n]);strcpy(topFROM[n],topFROM[n2]);
      strcpy(topFROM[n2],tmpID);
    }
}

int presskey()
{
  char c;
  if(read (0,&c,1)<=0) exit(-1);
}
