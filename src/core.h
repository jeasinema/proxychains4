/***************************************************************************
                          core.h  -  description
                             -------------------
    begin                : Tue May 14 2002
    copyright          :  netcreature (C) 2002
    email                 : netcreature@users.sourceforge.net
 ***************************************************************************
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdint.h>

#ifndef __CORE_HEADER
#define __CORE_HEADER
#define BUFF_SIZE 8*1024  // used to read responses from proxies.
#define     MAX_LOCALNET 64
#define     MAX_DNAT 64

typedef union {
	unsigned char octet[4];
	uint32_t as_int;
} ip_type;

typedef struct {
	uint32_t hash;
	char* string;
} string_hash_tuple;

typedef struct {
	uint32_t counter;
	uint32_t capa;
	string_hash_tuple** list;
} internal_ip_lookup_table;

extern internal_ip_lookup_table internal_ips;

#ifdef THREAD_SAFE
#include <pthread.h>

extern pthread_mutex_t internal_ips_lock;
extern pthread_mutex_t internal_getsrvbyname_lock;

# define MUTEX_LOCK(x) pthread_mutex_lock(x)
# define MUTEX_UNLOCK(x) pthread_mutex_unlock(x)
# define MUTEX_INIT(x,y) pthread_mutex_init(x, y)
#else
# define MUTEX_LOCK(x)
# define MUTEX_UNLOCK(x)
# define MUTEX_INIT(x,y)
#endif

/*error codes*/
typedef enum {
	SUCCESS=0,
	MEMORY_FAIL,        // malloc failed
	SOCKET_ERROR,  // look errno for more
	CHAIN_DOWN,    // no proxy in chain responds to tcp
	CHAIN_EMPTY,   //  if proxy_count = 0
	BLOCKED  //  target's port blocked on last proxy in the chain
} ERR_CODE;

typedef enum {
	HTTP_TYPE,
	RAW_TYPE,
	SOCKS4_TYPE,
	SOCKS5_TYPE
} proxy_type;

typedef enum {
	DYNAMIC_TYPE,
	STRICT_TYPE,
	RANDOM_TYPE}
chain_type;

typedef enum {
	PLAY_STATE,
	DOWN_STATE,
	BLOCKED_STATE,
	BUSY_STATE
} proxy_state;

typedef enum {
	RANDOMLY,
	FIFOLY
} select_type;

typedef struct {
	struct in_addr in_addr, netmask;
	unsigned short port;
} localaddr_arg;

typedef struct {
	struct in_addr orig_dst, new_dst;
	unsigned short orig_port, new_port;
} dnat_arg;

typedef struct {
	ip_type ip;
	unsigned short port;
	proxy_type pt;
	proxy_state ps;
	char user[256];
	char pass[256];
} proxy_data;

int connect_proxy_chain (int, ip_type, unsigned short, proxy_data *, unsigned int,
    chain_type, unsigned int);

void proxychains_write_log(char *, ...);

typedef int (*connect_t)(int, const struct sockaddr *, socklen_t);
typedef struct hostent* (*gethostbyname_t)(const char *);
typedef int (*freeaddrinfo_t)(struct addrinfo *);

#if (defined __linux__) || (defined __APPLE__)
typedef struct hostent *(*gethostbyaddr_t) (const void *, socklen_t, int);
#else
typedef struct hostent *(*gethostbyaddr_t) (const char *, socklen_t, int);
#endif

typedef int (*getaddrinfo_t)(const char *, const char *, const struct addrinfo *,
			     struct addrinfo **);
typedef int (*getnameinfo_t) (const struct sockaddr *, socklen_t, char *,
			      socklen_t, char *, socklen_t, int);


extern connect_t true_connect;
extern gethostbyname_t true_gethostbyname;
extern getaddrinfo_t true_getaddrinfo;
extern freeaddrinfo_t true_freeaddrinfo;
extern getnameinfo_t true_getnameinfo;
extern gethostbyaddr_t true_gethostbyaddr;

struct gethostbyname_data {
	struct hostent hostent_space;
	in_addr_t resolved_addr;
	char *resolved_addr_p[2];
	char addr_name[1024 * 8];
};

struct hostent* proxy_gethostbyname(const char *, struct gethostbyname_data *);
void proxy_getservbyname(const char *, struct servent *, char *, size_t, struct servent **);
int proxy_getaddrinfo(const char *, const char *, const struct addrinfo *, struct addrinfo **);
void proxy_freeaddrinfo(struct addrinfo *);

#ifdef DEBUG
# define PDEBUG(fmt, args...) do { fprintf(stderr,"DEBUG:"fmt, ## args); fflush(stderr); } while(0)
#else
# define PDEBUG(fmt, args...) do {} while (0)
#endif

#endif
