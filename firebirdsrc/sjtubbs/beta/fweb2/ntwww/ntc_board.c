#include <stdlib.h>
#include <glib.h>

#include "ntcore.h"
#include "ntcinternal.h"


static char seccode[SectNum][5] = {
	"a", "bc", "d", "efg", "hij", "kl", "o", "p", "qrs", "tu", "vw", "x"
};

static char secname[SectNum][2][20] = {
	{"BBS ϵͳ", "[��վ][վ��][BBS]"},
	{"�Ϻ�����", "[��У]"},
	{"ѧ��ԺУ", "[��У][����]"},
	{"���Լ���", "[����]"},
	{"ѧ����ѧ", "[��ѧ]"},
	{"�����Ļ�", "[����][����]"},
	{"�����˶�", "[����][�˶�]"},
	{"��������", "[����][����][����]"},
	{"֪�Ը���", "[֪��] [����]"},
	{"�����Ϣ", "[���][��Ϣ][����]"},
	{"����Ⱥ��", "[����][Ⱥ��]"},
	{"��Ϸר��", "[��Ϸ]"}
};


#define _INFO_BRD_NUM (256)
#define _INFO_BRD_FILE "etc/infobrds"
static struct {
	char district[_INFO_BRD_NUM];
	char board[_INFO_BRD_NUM][32];
	time_t mtime;
	unsigned int num;
}info_board;

static gboolean board_check_info_brd_file(void)
{
	time_t mtime;
	unsigned int i;
	FILE *fp;
	mtime = nt_stat_mtime(_INFO_BRD_FILE);
	if(mtime==0)
		return FALSE;
	if(mtime == info_board.mtime)
		return TRUE;
	fp = fopen(_INFO_BRD_FILE, "r");
	if(fp==NULL)
		return FALSE;
	for(i=0;i<_INFO_BRD_NUM;i++)
	{
		if(2!=fscanf(fp,"%c %31s\n",&(info_board.district[i]),info_board.board[i]))
			break;
	}
	fclose(fp);
	info_board.mtime = mtime;
	info_board.num = i;
	return TRUE;
}

gboolean nt_board_get_tip(char *board, unsigned int boardlen, char *tip, unsigned int tiplen)
{
	char tips[3][512], path[1024];
	int rbrd, rtip;
	unsigned int i;
	FILE *fp;

	if(!board_check_info_brd_file()) return FALSE;
	if(info_board.num==0) return FALSE;
	rbrd = rand()%info_board.num;
	snprintf(path,sizeof(path),"0Announce/groups/GROUP_%c/%s/info",info_board.district[rbrd],info_board.board[rbrd]);
	fp = fopen(path,"r");
	if(fp==NULL)
		return FALSE;
	for(i=0;i<3;i++)
	{
		if(NULL==fgets(tips[i],512,fp))
			break;
	}
	fclose(fp);
	if(i==0) return FALSE;
	rtip = rand()%i;
	g_strlcpy(board,info_board.board[rbrd],boardlen);
	g_strlcpy(tip,tips[rtip],tiplen);
	return TRUE;
}

gboolean ntcboard_init(void)
{
	memset(&info_board,0,sizeof(info_board));
	return TRUE;
}

void ntcboard_final(void)
{
	return;
}

void ntcboard_reset(void)
{
	return;
}
