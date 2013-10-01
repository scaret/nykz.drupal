#include "www.h"

int
bbslogout_main()
{
	int pid;

	init_all();
	if (!loginok)
		http_quit_print("��û�е�¼");
	pid = u_info->pid;
	if (pid > 0)
	{
		if (is_web_user(u_info))
			kick_web_user(u_info);
		else
			kill(pid, SIGHUP);
	}
	setcookie("utmpkey", "");
	setcookie("utmpnum", "");
	setcookie("utmpuserid", "");
	redirect("/");
	return 0;
}
