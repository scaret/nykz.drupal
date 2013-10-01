#include "www.h"

#define MAXREGIST (2500)

extern int loadregistdetail();

struct registdetail
{
	char id[IDLEN + 1];
	char email[REAL_NAME_EMAIL_LEN];
	int num;
	int gender;
	int bus;	// 0: ��; 1: ��, 2: ��; 3: both
};

extern struct registdetail alldetail[MAXREGIST];

int
writedetail(char* id, int num, int bus, char* email, int gender)
{
	char filename[] = "vote/regist";
	int fd;
	struct registdetail detail;

	strncpy(detail.id, id, IDLEN + 1);
	g_message("id is %s, first char is %d", id, id[0]);
	if(id[0] == '\0')
	{
		detail.num = 0;
	}
	else
	{
		detail.num = num;
	}
	strncpy(detail.email, email, REAL_NAME_EMAIL_LEN);
	detail.bus = bus;
	detail.gender = gender;
	if((fd = open(filename, O_WRONLY)) == -1)
	{
		g_message("bbsdoregist, cannot write");
		return -1;
	}
	lseek(fd, (num - 1) * sizeof(struct registdetail), SEEK_SET);
	flock(fd, LOCK_EX);
	write(fd, &detail, sizeof(struct registdetail));
	flock(fd, LOCK_UN);
	close(fd);
	return 0;
}

int
bbsdoregist_main()
{
	int num, bus, originalnum = 0;
	char id[IDLEN + 1], gender, email[REAL_NAME_EMAIL_LEN];
	int i = 0;
	int type;	// 1: ����; 2: �޸�; 3: ɾ��
	// ��������
	// 3: �����ѱ�ѡ��;
	// 2: ��id��ѡ��;
	// 1: �ð�email��ѡ��;
	int errortype = 0;
	char errormsg[STRLEN];
	char* status_file = "vote/regist_status";
	char* log_file = "vote/regist_log";
	int regIsOpen = 0;
	FILE* fp;
	time_t now;

	init_all();
	
	if ((fp = fopen(status_file, "r")) != NULL){
		fscanf(FCGI_ToFILE(fp), "%d", &regIsOpen);
		fclose(fp);
	}

	if (!regIsOpen){
		printf("<body>");
		printf("��ǰ������δ�������Ѿ��رա�<br/>");
		printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
		printf("</body>");
	}
	else if (!loginok)	// guest ���ܱ���
	{
		printf("<body>");
		printf("�Ҵҹ��Ͳ��ܱ��������ȵ�¼��<br/>");
		printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
		printf("</body>");
	}
	else if (!(currentuser.userlevel & PERM_LOGINOK))
	{
		printf("<body>");
		printf("����δͨ��ע�ᣬ���ܱ�����<br/>");
		printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
		printf("</body>");
	}
	else
	{
		type = atoi(getparm("type"));
		loadregistdetail();
		
		if(type == 1)
		{
			strncpy(id, currentuser.userid, IDLEN + 1);
			gender = getparm("gender")[0] == 'M' ? 1 : 2;
			bus = atoi(getparm("bus"));
			num = atoi(getparm("number"));
			strncpy(email, getparm("email"), REAL_NAME_EMAIL_LEN);
			for(i = 0; i < MAXREGIST; i++)
			{
				if(alldetail[i].num == num)
				{
					errortype = 3;
					break;
				}
				if(strcmp(alldetail[i].id, id) == 0)
				{
					errortype = 2;
					continue;
				}
				if(strcmp(alldetail[i].email, email) == 0)
				{
					if(strcmp(email, "NO_REAL_EMAIL") != 0)
					{
						if(errortype < 1)
						{
							errortype = 1;
						}
					}
				}
			}
			if(errortype == 0)	// ����ѡ��
			{
				if(writedetail(id,num,bus,email,gender) == 0)
				{
					printf("����ѡ��ɹ���<br/>");
					printf("��ѡ��ĺ�����: %d</br>", num);
					printf("��鿴ѡ�������ȷ����ѡ����<br/>");
					printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");

					now = time(0);
					getdatestring (now, NA);
					fp = fopen(log_file, "at");
					fprintf(fp, "%-3c%-36s%-15s%-5d%3c\n", '+', datestring, id, num, (gender==1)?'M':'F');
					fclose(fp);
				}
			}
			else
			{
				if(errortype == 3)
				{
					strcpy(errormsg, "�ú����ѱ�ѡ��");
				}
				else if(errortype == 2)
				{
					strcpy(errormsg, "��id�Ѿ�ѡ�������");
				}
				else if(errortype == 1)
				{
					strcpy(errormsg, "��id�󶨵������Ѿ�ѡ�������");
				}
				printf("ѡ��ʧ�ܣ�ԭ����: <br/>");
				printf("%s<br/>", errormsg);
				printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
			}
		}
		else if(type == 2)
		{
			strncpy(id, currentuser.userid, IDLEN + 1);
			gender = getparm("gender")[0] == 'M' ? 1 : 2;
			bus = atoi(getparm("bus"));
			num = atoi(getparm("number"));
			strncpy(email, getparm("email"), REAL_NAME_EMAIL_LEN);
			if(strcmp(currentuser.userid, alldetail[num - 1].id) == 0)
			{
				if(alldetail[num-1].bus == bus)
				{
					printf("����ѡ��û�б仯��<br/>");
					printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
				}
				else
				{
					writedetail(currentuser.userid, num, bus, email, gender);
					
					now = time(0);
					getdatestring (now, NA);
					fp = fopen(log_file, "at");
					fprintf(fp, "%-3c%-36s%-15s%-5d%3c\n", '@', datestring, id, num, (gender==1)?'M':'F');
					fclose(fp);

					printf("����ѡ��ɹ���<br/>");
					printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
				}
			}
			else
			{
				for(i = 0; i < MAXREGIST; i++)
				{
					if(strcmp(alldetail[i].id, currentuser.userid) == 0)
					{
						originalnum = alldetail[i].num;
						break;
					}
				}
				if(alldetail[num-1].id[0] == '\0')	// �ú���û��ѡ��
				{
					if(originalnum > 0)
					{
						writedetail(currentuser.userid,num,bus,email,gender);
						bzero(id, sizeof(id));
						bzero(email, sizeof(email));
						writedetail(id,originalnum,0, email,0);
						printf("�����޸ĳɹ���<br/>");
						printf("��鿴ѡ�������ȷ����ѡ����<br/>");
						printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
					}
				}
				else
				{
					printf("�Բ��𣬸ú���������ѡ��<br/>");
					printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");
				}
			}
		}
		else if(type == 3)
		{
			for(i = 0; i < MAXREGIST; i++)
			{
				if(strcmp(alldetail[i].id, currentuser.userid) == 0)
				{
					num = alldetail[i].num;
					break;
				}
			}
			if(strcmp(alldetail[i].id, currentuser.userid) == 0)
			{
				bzero(id, sizeof(id));
				bzero(email, sizeof(email));
				writedetail(id,num, 0, email, 0);
				printf("ɾ��ѡ�ųɹ���<br/>");
				printf("��鿴ѡ�������ȷ����ѡ����<br/>");
				printf("[<a href=\"bbsregistview\">�鿴ѡ�����</a>]");		

				now = time(0);
				getdatestring (now, NA);
				fp = fopen(log_file, "at");
				fprintf(fp, "%-3c%-36s%-15s\n", '-', datestring, currentuser.userid);
				fclose(fp);
			}
		}
	}
	return 0;
}


