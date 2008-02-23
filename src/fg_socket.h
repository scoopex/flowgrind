#ifndef _FG_SOCKET_H
#define _FG_SOCKET_H

#include <sys/socket.h>

ssize_t read_exactly(int, void *, size_t);
size_t read_until_plus(int d, char *buf, size_t nbytes);
ssize_t write_exactly(int, const void *, size_t);

int set_congestion_control(int fd, const char *cc_alg);
int set_so_debug(int fd);
int set_keepalive(int fd, int how);
int set_nodelay(int fd);
int set_non_blocking (int fd);
int set_route_record(int fd);
int set_so_dscp(int fd, uint8_t);
int set_so_elcn(int fd, int val);
int set_so_icmp(int fd);
int set_dscp(int sock, uint8_t dscp);
int set_so_cork(int fd);
int set_window_size(int, int);
int set_window_size_directed(int, int, int);

int get_mtu(int fd);
int get_mss(int fd);

const char *fg_nameinfo(const struct sockaddr *sa);
char sockaddr_compare(const struct sockaddr *a, const struct sockaddr *b);

#endif