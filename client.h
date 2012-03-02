#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "stat.h"

struct fio_net_cmd;

struct fio_client {
	struct flist_head list;
	struct flist_head hash_list;
	struct flist_head arg_list;
	union {
		struct sockaddr_in addr;
		struct sockaddr_in6 addr6;
		struct sockaddr_un addr_un;
	};
	char *hostname;
	int port;
	int fd;

	char *name;

	int state;

	int skip_newline;
	int is_sock;
	int disk_stats_shown;
	unsigned int jobs;
	int error;
	int ipv6;
	int sent_job;

	struct flist_head eta_list;
	struct client_eta *eta_in_flight;

	struct flist_head cmd_list;

	uint16_t argc;
	char **argv;

	void *client_data;
};

typedef void (*client_text_op_func)(struct fio_client *client,
		FILE *f, __u16 pdu_len, const char *buf);
typedef void (*client_disk_util_op_func)(struct fio_client *client, struct fio_net_cmd *cmd);
typedef void (*client_thread_status_op)(struct fio_net_cmd *cmd);
typedef void (*client_group_stats_op)(struct fio_net_cmd *cmd);
typedef void (*client_eta_op)(struct fio_client *client, struct fio_net_cmd *cmd);
typedef void (*client_probe_op)(struct fio_client *client, struct fio_net_cmd *cmd);
typedef void (*client_thread_status_display_op)(char *status_message, double perc);
typedef void (*client_quit_op)(struct fio_client *);
typedef void (*client_add_job_op)(struct fio_client *, struct fio_net_cmd *);
typedef void (*client_timed_out)(struct fio_client *);

struct client_ops {
	client_text_op_func text_op;
	client_disk_util_op_func disk_util;
	client_thread_status_op thread_status;
	client_group_stats_op group_stats;
	client_eta_op eta;
	client_probe_op probe;
	client_quit_op quit;
	client_add_job_op add_job;
	client_timed_out timed_out;
	int stay_connected;
};

extern struct client_ops fio_client_ops;

struct client_eta {
	struct jobs_eta eta;
	unsigned int pending;
};

extern int fio_handle_client(struct fio_client *, struct client_ops *ops);
extern void fio_client_dec_jobs_eta(struct client_eta *eta, void (*fn)(struct jobs_eta *));
extern void fio_client_sum_jobs_eta(struct jobs_eta *dst, struct jobs_eta *je);
extern void fio_client_convert_jobs_eta(struct jobs_eta *je);

enum {
	Fio_client_ipv4 = 1,
	Fio_client_ipv6,
	Fio_client_socket,
};

extern int fio_clients_connect(void);
extern int fio_clients_send_ini(const char *);
extern int fio_handle_clients(struct client_ops *ops);
extern int fio_client_add(const char *, void **);
extern struct fio_client *fio_client_add_explicit(const char *, int, int);
extern void fio_client_add_cmd_option(void *, const char *);
extern void fio_clients_terminate(void);

#endif
