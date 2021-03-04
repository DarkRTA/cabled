#include <jack/jack.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <unistd.h>
#include <inih/ini.h>

struct Entry;

struct Entry {
	char *out;
	char *in;
	struct Entry *next;
};

struct Entry *conns = NULL;
struct Entry *disconns = NULL;
jack_client_t *client = NULL;

// XXX: concurrency note: this should proabably be atomic and possibly even
// be behind a mutex
// rn the worst that can happen is an update being missed
int update = 1;

void port_registered(jack_port_id_t id, int registered, void *arg)
{
	if (registered) {
		update = 1;
	}
}

void port_connected(jack_port_id_t a, jack_port_id_t b, int connected, void *arg)
{
	if (connected) {
		update = 1;
	}
}

int cfg_handler(
	void *data, const char *section, const char *name, const char *value)
{
	printf("%s: %s -> %s\n", section, name, value);
	struct Entry **ptr;
	if (!strcmp("connect", section)) {
		ptr = &conns;
	} else if (!strcmp("disconnect", section)) {
		ptr = &disconns;
	} else {
		return 0;
	}

	struct Entry *entry = malloc(sizeof(struct Entry));
	entry->next = *ptr;
	entry->out = strdup(name);
	entry->in = strdup(value);
	*ptr = entry;

	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		exit(1);
	}
	if (ini_parse(argv[1], cfg_handler, NULL) < 0) {
		printf("Can't load config file\n");
		return 1;
	}

	client = jack_client_open("cabled", JackNoStartServer, NULL);
	jack_set_port_registration_callback(client, port_registered, NULL);
	jack_set_port_connect_callback(client, port_connected, NULL);
	jack_activate(client);

	while (1) {
		sleep(1);
		if (update) {
			update = 0;
			// we should have a queue of connected ports so we aren't
			// blindly trying to reconnect everything
			// perhaps we could also support regex here as well
			for (struct Entry *e = conns; e != NULL; e = e->next) {
				jack_connect(client, e->out, e->in);
			}
			for (struct Entry *e = disconns; e != NULL; e = e->next) {
				jack_disconnect(client, e->out, e->in);
			}
		}
	}
}
