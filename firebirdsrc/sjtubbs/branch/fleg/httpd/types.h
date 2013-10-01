char *eff_size(char *file);
char *crypt1(char *pw, char *salt);
char *brc_getrecord(char *ptr, char *name, int *pnum, int *list);
char *brc_putrecord(char *ptr, char *name, int num, int *list);
void brc_update(char *userid, char *board);
int brc_init(char *userid, char *boardname);
int brc_add_read(char *filename);
int brc_un_read(char *file);
int brc_clear();
int brc_has_read(char *file);
int file_has_word(char *file, char *word);
int f_append(char *file, char *buf);
struct stat *f_stat(char *file);
int file_time(char *file);
int get_record(void *buf, int size, int num, char *file);
int put_record(void *buf, int size, int num, char *file);
int del_record(char *file, int size, int num);
char *Ctime(time_t t);
char *noansi(char *s);
char *nohtml(char *s);
char *strright(char *s, int len);
char *strcasestr(char *s1, char *s2);
int strsncpy(char *s1, char *s2, int n);
char *ltrim(char *s);
char *rtrim(char *s);
char *trim(char *s);
char *get_new_shm(int key, int size);
char *get_old_shm(int key, int size);
char *get_shm(int key, int size);
int cgi_head();
char *getsenv(char *s);
int http_quit();
int http_fatal(char *fmt, ...);
int strnncpy(char *s, int *l, char *s2);
int hsprintf(char *s, char *fmt, ...);
int hprintf(char *fmt, ...);
int change1(char *buf);
int hhprintf(char *fmt, ...);
int parm_add(char *name, char *val);
int http_init();
char *getparm(char *var);
int get_shmkey(char *s);
int shm_init();
int user_init(struct userec *x, struct user_info **y);
int post_mail(char *userid, char *title, char *file, char *id,
              char *nickname, char *ip, int sig);
int post_imail(char *userid, char *title, char *file, char *id,
               char *nickname, char *ip, int sig);
int sig_append(FILE * fp, char *id, int sig);
char *anno_path_of(char *board);
int has_BM_perm(struct userec *user, char *board);
int has_badmin_perm(struct userec *user, struct shortfile *x);
int has_read_perm(struct userec *user, char *board);
int has_post_perm(struct userec *user, char *board);
//created by rwzmm @ sjtubbs. 新注册ID 24小时内不能POST
int	too_young(struct userec *user, char *board);

struct shortfile *getbcache(char *board);
int getbnum(char *board);
int count_on_board(char *board);
int count_mails(char *id, int *total, int *unread);
int findnextutmp(char *id, int from);
int sethomefile(char *buf, char *id, char *file);
int send_msg(char *myuserid, int mypid, char *touserid, int topid,
             char msg[256],int isWeb);
char *horoscope(int month, int day);
char *ModeType(int mode);
char *cexp(int exp, int type);
char *cexp2(int exp);
char *cexp3(int exp);
char *cperf(int perf);
int count_life_value(struct userec *urec);
int countexp(struct userec *x);
int countperf(struct userec *x);
int modify_mode(struct user_info *x, int newmode);
int save_user_data(struct userec *x);
int is_bansite(char *ip);
int user_perm(struct userec *x, int level);
int getusernum(char *id);
int getuser_like(char *userid);
int str_is_like(char *userid, char *userid2);
struct userec *getuser(char *id);
int checkpasswd(char *pw_crypted, char *pw_try);
char *crypt_passwd(char *pw);
int checkuser(char *id, char *pw);
int count_id_num(char *id);
int count_online();
int count_online2();
int loadfriend(char *id);
int isfriend(char *id);
int loadbad(char *id);
int isbad(char *id1, char *id2);
int init_all();
int init_no_http();
char *void1(unsigned char *s);
char *sec(char c);
char *flag_str(int access);
char *flag_str2(int access, int has_read);
char *userid_str(char *s);
int fprintf2(FILE * fp, char *s);
struct fileheader *get_file_ent(char *board, char *file);
char *getbfroma(char *path);
int load_my_www();
char *bg(char *board);
char *nohtml2(char *s);
char *void3(char *s);
float get_load(int i);
void *file_map(char *file, int len);
int in_pw_lock(char *userid, char *ip);
int add_pw_lock(char *userid, char *ip);
int del_pw_lock(char *userid, char *ip);
int check_pw_lock();
int check_maxmail2(int *maxmail, int *size);
int tcp_open(char *host, int port);
char *nohtml3(char *s);
char *nohtml5(char *s);
char *nohtml4(char *s);
char *Gtime(time_t t);
int count_wwwguest();
int check_post_limit();
struct tm *Ltime(time_t t);
int Day(time_t t);
int today();
int check_today();
int tcp_bind(int port, void func(), int pid);
int file_size(char *file);
int file_rtime(char *file);
int file_exist(char *file);
int file_isdir(char *file);
int file_isfile(char *file);
int setcookie(char *s, char *b);
int redirect(char *x);
int refreshto(char *x, int t);
int base64(char c);
char *unbase64(char *buf);
