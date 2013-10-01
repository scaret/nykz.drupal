/*
         [ ���BBSD��ʽʹ�õ�uptime    by Eric ]
���޸� cron.bbs �еģ�
0,5,10,15,20,25,30,35,40,45,50,55 * * * * /usr/bin/uptime >> /home/HOPE/bbs/reclog/uptime.log
Ϊ��
0,5,10,15,20,25,30,35,40,45,50,55 * * * * /home/bbs/bin/bbsuptime >> /home/HOPE/bbs/reclog/uptime.log
�������� bbs �����ִ�У� crontab cron.bbs
*/

#include "bbs.h"

int search_shmkey(char *keyname)
{
    int i = 0, found = 0;

    while (shmkeys[i].key != NULL)
    {
        if (strcmp(shmkeys[i].key, keyname) == 0)
        {
            found = shmkeys[i].value;
            break;
        }
        i++;
    }
    return found;
}

int main()
{
    void *shmptr;
    struct UTMPFILE *utmpshm;
    struct tm *now;
    int shmid, shmkey;
    int i, max, count;
    time_t nowtime;

    shmkey = search_shmkey("UTMP_SHMKEY");
    if (shmkey < 1024)
        shmkey = 30020;
    if ((shmid = shmget(shmkey, 0, 0)) == -1)
        exit(1);
    shmptr = (void *) shmat(shmid, NULL, 0);
    utmpshm = shmptr;

    max = USHM_SIZE - 1;
    while (max > 0 && utmpshm->uinfo[max].active == 0)
        max--;
    count = 0;
    for (i = 0; i <= max; i++)
    {
        if (utmpshm->uinfo[i].active && utmpshm->uinfo[i].pid)
            count++;
    }
    shmdt(shmptr);
    nowtime = time(0);
    now = localtime(&nowtime);
    printf("  %d:%d%s ,  %d user, \n",
           (now->tm_hour % 12 == 0) ? 12 : (now->tm_hour % 12),
           now->tm_min, (now->tm_hour > 11) ? "pm" : "am", count);
}
