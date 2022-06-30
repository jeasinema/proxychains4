#include "common.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

static int check_path(char *path) {
	if(!path)
		return 0;
	return access(path, R_OK) != -1;
}

char *get_config_path(char* default_path, char* pbuf, size_t bufsize) {
	char buf[512];
	// top priority: user defined path
	char *path = default_path;
	if(check_path(path))
		return path;
	if (!pbuf)
		goto err;

	// priority 1: env var PROXYCHAINS_CONF_FILE
	path = getenv(PROXYCHAINS_CONF_FILE_ENV_VAR);
	if(check_path(path))
		return path;

	// priority 2; proxychains conf in actual dir
	path = getcwd(buf, sizeof(buf));
	snprintf(pbuf, bufsize, "%s/%s", path, PROXYCHAINS_CONF_FILE);
	path = pbuf;
	if(check_path(path))
		return path;

	// priority 3; $HOME/.proxychains/proxychains.conf
	path = getenv("HOME");
	snprintf(pbuf, bufsize, "%s/.proxychains/%s", path, PROXYCHAINS_CONF_FILE);
	path = pbuf;
	if(check_path(path))
		return path;

	// priority 4: $SYSCONFDIR/proxychains.conf
	path = SYSCONFDIR "/" PROXYCHAINS_CONF_FILE;
	if(check_path(path))
		return path;

	// priority 5: /etc/proxychains.conf
	path = "/etc/" PROXYCHAINS_CONF_FILE;
	if(check_path(path))
		return path;

err:
	perror("couldnt find configuration file");
	exit(1);
}
