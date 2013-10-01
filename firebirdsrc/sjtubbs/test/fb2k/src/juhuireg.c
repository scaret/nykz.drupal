#include "bbs.h"

struct registdetail alldetail[MAX_REGIST_NUM];
int isopen = 0, max_allow_num = 0;

void get_real_name_email(char *buf)
{
    char path[128];
    int fd;
    ssize_t readnum;

    sethomefile(path, currentuser.userid, REAL_NAME_FILE);
    if ((fd = open(path, O_RDONLY)) == -1)
    {
        buf[0] = '\0';
        return;
    }
    readnum = read(fd, buf, REAL_NAME_EMAIL_LEN);
    close(fd);
    buf[readnum] = '\0';
    return;
}

void open_juhuireg(void)
{
    int fd, i;
    struct registdetail detail;
    FILE *fp;

    clear();
    if (askyn("�����ۻᱨ��?", YEA, YEA) == YEA)
    {
        if (!dashd(REGIST_DIR))
            mkdir(REGIST_DIR, 0755);

        fp = fopen(REGIST_OPENCHECK_FILE, "w+");
        fprintf(fp, "%d", 1);
        fclose(fp);

        if (!dashf(REGIST_DATA_FILE))
        {
            fd = open(REGIST_DATA_FILE, O_WRONLY | O_CREAT, S_IRWXU);
            detail.bus = 0;
            bzero(detail.email, sizeof(detail.email));
            detail.gender = 0;
            bzero(detail.userid, sizeof(detail.userid));
            for (i = 0; i < MAX_REGIST_NUM; i++)
            {
                write(fd, &detail, sizeof(struct registdetail));
            }
            close(fd);
        }

        prints("�ۻᱨ��������!");
        pressanykey();
    }
}

void close_juhuireg(void)
{
    FILE *fp;

    clear();
    if (askyn("�رվۻᱨ��?", YEA, YEA) == YEA)
    {
        fp = fopen(REGIST_OPENCHECK_FILE, "w+");
        fprintf(fp, "%d", 0);
        fclose(fp);
        prints("�ۻᱨ��������!");
        pressanykey();
    }
}

void clear_juhuireg_data(void)
{
    clear();
    if (askyn("��յ�ǰ�ۻᱨ������?", YEA, YEA) == YEA)
    {
        remove(REGIST_DATA_FILE);
        remove(REGIST_DATAVIEW_FILE);
        remove(REGIST_LOG);
        prints("�ۻᱨ�����������...");
        pressanykey();
    }
}

void view_juhuireg_log(void)
{
    ansimore_withmode(REGIST_LOG, 0, MORE_MODE_READING);
}

int loadregistdetail(int *isopen, int *max_allow_num)
{
    int fd;
    int i, bufread;
    struct registdetail detail;
    FILE *fp;

    if ((fp = fopen(REGIST_OPENCHECK_FILE, "r")) != NULL)
    {
        fscanf(fp, "%d", isopen);
        fclose(fp);
    }
    if (fp == NULL || *isopen == 0)
    {
        return -1;
    }

    if ((fp = fopen(REGIST_MAXALLOWNUM_FILE, "r")) != NULL)
    {
        fscanf(fp, "%d", max_allow_num);
        fclose(fp);
    }
    if (fp == NULL || *max_allow_num == 0)
        return -1;

    if ((fd = open(REGIST_DATA_FILE, O_RDONLY, 0)) == -1)
        return -1;
    for (i = 0; i <= *max_allow_num; i++)
    {
        bufread = read(fd, &detail, sizeof(struct registdetail));
        if (bufread != sizeof(struct registdetail))
            break;
        alldetail[i].bus = detail.bus;
        alldetail[i].gender = detail.gender;
        strcpy(alldetail[i].email, detail.email);
        strcpy(alldetail[i].userid, detail.userid);
    }
    close(fd);
    return 0;
}

int writedetail(char *userid, int num, int bus, char *email, int gender)
{
    int fd;
    struct registdetail detail;

    strncpy(detail.userid, userid, IDLEN + 1);
    strncpy(detail.email, email, REAL_NAME_EMAIL_LEN);
    detail.bus = bus;
    detail.gender = gender;
    if ((fd = open(REGIST_DATA_FILE, O_WRONLY)) == -1)
        return 0;
    lseek(fd, num * sizeof(struct registdetail), SEEK_SET);
    flock(fd, LOCK_EX);
    write(fd, &detail, sizeof(struct registdetail));
    flock(fd, LOCK_UN);
    close(fd);
    return 1;
}

int get_originalnum(char *userid)
{
    int i;
    for (i = 1; i <= max_allow_num; i++)
        if (strcmp(alldetail[i].userid, userid) == 0)
            return i;
    return -1;
}

void show_juhuireg_note(int num)
{
    clear();
    ansimore_partial(REGIST_NOTE, NA, 0, 16);

    if (HAS_PERM(PERM_SYSOP))
    {
        move(t_lines - 2, 0);
        prints("[1;44;31m[վ�����] [33m �������� + | �رձ��� - | ��ձ��� d | �鿴��־ l [m");
    }

    move(t_lines - 1, 0);
    prints("[1;44;31m[վ�챨��] [33m�鿴������� v ");
    if (num == -1)
        prints("| ��Ҫ���� Enter                       ");
    else
        prints("| ��ѡ����: %-4d �޸� Enter ȡ������ c ", num);
    prints("������ q,��  [m");
}

void view_juhuireg_data()
{
    loadregistdetail(&isopen, &max_allow_num);
    if (!isopen || max_allow_num <= 0)
        return;

    ansimore_withmode(REGIST_DATAVIEW_FILE, 0, MORE_MODE_READING);
}

void rebuild_juhuireg_dataview()
{
    FILE *fp;
    char filename[STRLEN];
    struct registdetail *detail;
    int i, j;
    int blank, leftblank, k;
    int totalnum, male, female, buscome, busgo;

    loadregistdetail(&isopen, &max_allow_num);
    if (!isopen || max_allow_num <= 0)
        return;

    totalnum = male = female = buscome = busgo = 0;

    sprintf(filename, "%s.%d", REGIST_DATAVIEW_FILE, getuid());
    fp = fopen(filename, "w");

    for (i = 1; i <= max_allow_num; i++)
    {
        if (alldetail[i].userid[0] != '\0')
        {
            totalnum++;
            if (alldetail[i].bus == 1)
                buscome++;
            else if (alldetail[i].bus == 2)
                busgo++;
            else if (alldetail[i].bus == 3)
            {
                buscome++;
                busgo++;
            }
            if (alldetail[i].gender == 'M')
                male++;
            else
                female++;
        }
    }

    fprintf(fp, "                           [1;32m��ˮ˼ԴBBSվ վ�챨������[m\n");
    fprintf(fp, "  [1;32m��������:%4d��  [36m����:%4d��  [35mŮ��:%4d��  [33m��У����:%4d��  ��У����:%4d��\n\n[m",
            totalnum, male, female, buscome, busgo);
    fprintf(fp, "[1;42;37m���������Щ����������������Щ������Щ����������������Щ������Щ�����������������[m\n");
    fprintf(fp, "[1;42;37m��[40;33m ���� [42;37m��[40m    ����ID      [1;42;37m��[40;33m ���� [42;37m��[40m    ����ID      [1;42;37m��[40;33m ���� [42;37m��[40m    ����ID      [1;42;37m��\n");

    fprintf(fp, "[1;42;37m���������੤���������������੤�����੤���������������੤�����੤����������������[m\n");
    j = 0;
    for (i = 1; i<= max_allow_num; i++)
    {
        detail = &alldetail[i];
        if (detail->userid[0] != '\0')
        {
            fprintf(fp, "[1;42;37m��[40;33m %04d [42;37m��[40m", i);
            blank = 16 - strlen(detail->userid);
            leftblank = blank / 2;
            for (k = 0; k < leftblank; k ++)
                fprintf(fp, " ");
            fprintf(fp, "[%dm%s[m", detail->gender == 'F' ? 35 : 36, detail->userid);
            for (k = 0; k < blank - leftblank; k ++)
                fprintf(fp, " ");
            j ++;
            if (j == 3)
            {
                j = 0;
                fprintf(fp, "[1;42;37m��[m\n");
            }
        }
    }
    if (j > 0)
    {
        for (k = 0; k < 3 - j; k ++)
            fprintf(fp, "[1;42;37m��[m      [1;42;37m��[m                ");
        fprintf(fp, "[1;42;37m��[m\n");
    }
    fprintf(fp, "[1;42;37m���������ة����������������ة������ة����������������ة������ة�����������������[m\n");
    fclose(fp);
    rename(filename, REGIST_DATAVIEW_FILE);
}

void log_regist(char *userid, int num, int bus, char gender, int type)
{
    FILE *fp;
    time_t now;

    now = time(0);
    getdatestring(now, NA);
    fp = fopen(REGIST_LOG, "at");
    fprintf(fp, "%-3c%-36s%-15s%-5d%-3c%3d\n", (type == 1)? '+' : ((type == 2) ? '@' : '-'),
            datestring, userid, num, gender, bus);
    fclose(fp);
}

int doapply(int originalnum, int num, int bus)
{
    int i;
    char email[REAL_NAME_EMAIL_LEN];

    if (bus < 0 || bus > 3)
    {
        prints("ѡ��ʧ�ܣ�����У��ѡ������\n");
        return 0;
    }

    if (num < 1 || num > max_allow_num)
    {
        prints("ѡ��ʧ�ܣ��ú��벻��������뷶Χ��\n");
        return 0;
    }

    if (originalnum == num && bus == alldetail[originalnum].bus)
    {
        prints("����ѡ��û�б仯��\n");
        return 0;
    }

    loadregistdetail(&isopen, &max_allow_num);
    for (i = 1; i <= max_allow_num; i++)
    {
        if (originalnum != i && !strcmp(currentuser.userid, alldetail[i].userid))
        {
            prints("ѡ��ʧ�ܣ�����ѡ�������\n");
            return 0;
        }
    }

    if (originalnum != num && alldetail[num].userid[0] != '\0')
    {
        prints("ѡ��ʧ�ܣ��ú����ѱ�ѡ��\n");
        return 0;
    }
    
    get_real_name_email(email);
    if (originalnum == -1 && email[0] != '\0')
    {
        for (i = 1; i <= max_allow_num; i++)
        {
            if (!strcmp(email, alldetail[i].email))
            {                
                prints("ѡ��ʧ�ܣ���id�󶨵������Ѿ�ѡ�������\n");
                return 0;
            }
        }
    }

    if (originalnum != -1)
        writedetail("", originalnum, 0, "", 0);
    writedetail(currentuser.userid, num, bus, email, currentuser.gender);
    log_regist(currentuser.userid, num, bus, currentuser.gender, originalnum == -1 ? 1 : 2);
    rebuild_juhuireg_dataview();
    if (originalnum == -1)
        prints("����ѡ��ɹ���\n");
    else
        prints("�����޸ĳɹ���\n");
    prints("��ѡ��ĺ�����: %d\n", num);
    return 1;
}

int juhuireg_apply(int originalnum)
{
    int i, bus, preset_bus, num, preset_num;
    char askbuf[STRLEN], buf[10];

    clear();

    if (originalnum == -1)
    {
        for (i = 1; i <= max_allow_num; i++)
            if (alldetail[i].userid[0] == '\0')
                break;

        if (i == max_allow_num + 1)
        {
            move(0, 0);
            prints("�ܱ�Ǹ��Ŀǰ������ȫ�����ꡣ");
            pressanykey();
            return -1;
        }
        preset_num = i;
        preset_bus = 0;
    }
    else
    {
        preset_num = originalnum;
        preset_bus = alldetail[originalnum].bus;
    }

    while (1)
    {
        clear();
        prints("%s", originalnum == -1 ? "��Ҫ����" : "�޸ı���");
        buf[0] = '\0';
        sprintf(askbuf, "�Ƿ����У��(0:��;1:��У����;2:��У����;3:������У������ѡ[1;31m%d[m): ", preset_bus);
        getdata(1, 0, askbuf, buf, 5, DOECHO, NA);
        if (!isdigit(buf[0]))
            bus = preset_bus;
        else
            bus = atoi(buf);

        buf[0] = '\0';
        sprintf(askbuf, "��������ѡ��ĺ���(ֱ�ӻس�ѡ[1;31m%d[m, ����0�˳�): ", preset_num);
        getdata(2, 0, askbuf, buf, 5, DOECHO, NA);
        if (!isdigit(buf[0]))
        {
            num = preset_num;
            if (doapply(originalnum, num, bus))
            {
                pressanykey();
                return num;
            }
            else
            {
                pressanykey();
                return originalnum;
            }
        }
        else
        {
            num = atoi(buf);
            if (num == 0)
                return originalnum;

            if (doapply(originalnum, num, bus))
            {
                pressanykey();
                return num;
            }
            else
            {
                pressanykey();
                return originalnum;
            }
        }
        pressanykey();
    }

    return originalnum;
}

int juhuireg_cancel(int num, int bus)
{
    clear();
    move(0, 0);
    if (askyn("ȷ��Ҫȡ��������?", NA, NA) == YEA)
    {
        writedetail("", num, 0, "", 0);
        log_regist(currentuser.userid, num, bus, currentuser.gender, 3);
        rebuild_juhuireg_dataview();
        prints("\n��ȡ������...");
        pressanykey();
        return -1;
    }
    return num;
}

void m_RegJuhui()
{
    int num;
    int ch;

    clear();
    move(0, 0);
    loadregistdetail(&isopen, &max_allow_num);
    if ((!isopen || max_allow_num <= 0) && !HAS_PERM(PERM_SYSOP))
    {
        prints("վ�챨����δ�������Ѿ��ر�");
        pressanykey();
        return;
    }

    modify_user_mode(READNEW);

    num = get_originalnum(currentuser.userid);

    show_juhuireg_note(num);
    ch = igetkey();
    while (1)
    {
        switch (ch)
        {
            case 'q':
            case KEY_LEFT:
                return;
            case 'v':
                view_juhuireg_data();               
                break;
            case '\r':
            case '\n':
                juhuireg_apply(num);
                num = get_originalnum(currentuser.userid);
                break;
            case 'c':
                if (num != -1)
                {
                    juhuireg_cancel(num, alldetail[num].bus);
                    num = get_originalnum(currentuser.userid);
                }
                break;
            case 'l':
                if (HAS_PERM(PERM_SYSOP))
                    view_juhuireg_log();
                break;
            case '+':
                if (HAS_PERM(PERM_SYSOP))
                    open_juhuireg();
                break;
            case '-':
                if (HAS_PERM(PERM_SYSOP))
                    close_juhuireg();
                break;
            case 'd':
                if (HAS_PERM(PERM_SYSOP))
                    clear_juhuireg_data();
                break;
        }
        show_juhuireg_note(num);
        ch = igetkey();
    }
}

