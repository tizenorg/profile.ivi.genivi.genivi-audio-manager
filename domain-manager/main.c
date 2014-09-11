/*
 * Copyright (c) 2014, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

#include <murphy/common.h>
#include <murphy/common/dbus-libdbus.h>

#include "ini-parser.h"

/* from audiomanagertypes.h */

/* domain status */
#define DS_UNKNOWN        0
#define DS_CONTROLLED     1
#define DS_RUNDOWN        2
#define DS_DOWN           255

/* interrupt state */
#define IS_OFF            1
#define IS_INTERRUPTED    2

/* availability status */
#define AS_AVAILABLE      1
#define AS_UNAVAILABLE    2

/* availability reason */
#define AR_NEWMEDIA       1
#define AR_SAMEMEDIA      2
#define AR_NOMEDIA        3
#define AR_TEMPERATURE    4
#define AR_VOLTAGE        5
#define AR_ERRORMEDIA     6

/* mute state */
#define MS_MUTED          1
#define MS_UNMUTED        2

/* connection format */
#define CF_MONO           1
#define CF_STEREO         2
#define CF_AUTO           4

/* error codes */
#define E_OK              0
#define E_UNKNOWN         1
#define E_OUT_OF_RANGE    2
#define E_NOT_USED        3
#define E_DATABSE_ERROR   4
#define E_ALREADY_EXISTS  5
#define E_NO_CHANGE       6
#define E_NOT_POSSIBLE    7
#define E_NON_EXISTENT    8
#define E_ABORTED         9
#define E_WRONG_FORMAT    10

/* D-Bus names, paths and interfaces */
#define AUDIOMGR_DBUS_NAME            "org.genivi.audiomanager"
#define AUDIOMGR_DBUS_INTERFACE       "org.genivi.audiomanager"
#define AUDIOMGR_DBUS_PATH            "/org/genivi/audiomanager"

#define AUDIOMGR_DBUS_ROUTE_NAME      "RoutingInterface"
#define AUDIOMGR_DBUS_ROUTE_PATH      AUDIOMGR_DBUS_PATH "/routinginterface"

#define OWN_DBUS_NAME                 "test.audiomanager.domain"
#define OWN_DBUS_INTERFACE            "test.audiomanager.route"
#define OWN_DBUS_PATH                 "/"

/* audiomanager router methods */

#define AUDIOMGR_REGISTER_DOMAIN      "registerDomain"
#define AUDIOMGR_DOMAIN_COMPLETE      "hookDomainRegistrationComplete"
#define AUDIOMGR_DEREGISTER_DOMAIN    "deregisterDomain"

#define AUDIOMGR_REGISTER_SOURCE      "registerSource"
#define AUDIOMGR_DEREGISTER_SOURCE    "deregisterSource"

#define AUDIOMGR_REGISTER_SINK        "registerSink"
#define AUDIOMGR_DEREGISTER_SINK      "deregisterSink"

#define AUDIOMGR_REGISTER_GATEWAY     "registerGateway"
#define AUDIOMGR_DEREGISTER_GATEWAY   "deregisterGateway"

#define AUDIOMGR_CONNECT              "asyncConnect"
#define AUDIOMGR_CONNECT_ACK          "ackConnect"

#define AUDIOMGR_DISCONNECT           "asyncDisconnect"
#define AUDIOMGR_DISCONNECT_ACK       "ackDisconnect"

#define AUDIOMGR_PEEK_DOMAIN          "peekDomain"
#define AUDIOMGR_PEEK_SINK            "peekSink"
#define AUDIOMGR_PEEK_SOURCE          "peekSource"

#define AUDIOMGR_SETSINKVOL_ACK       "ackSetSinkVolume"
#define AUDIOMGR_SETSRCVOL_ACK        "ackSetSourceVolume"
#define AUDIOMGR_SINKVOLTICK_ACK      "ackSinkVolumeTick"
#define AUDIOMGR_SRCVOLTICK_ACK       "ackSourceVolumeTick"
#define AUDIOMGR_SETSINKPROP_ACK      "ackSetSinkSoundProperty"

/* config file sections */

#define CONFIG_SINK                   "sink"
#define CONFIG_SOURCE                 "source"
#define CONFIG_DOMAIN                 "domain"
#define CONFIG_GATEWAY                "gateway"
#define CONFIG_CONFIG                 "config"

/* config file keys */

#define CONFIG_DBUS_BUS               "dbus_bus"
#define CONFIG_DBUS_ADDRESS           "dbus_address"
#define CONFIG_DBUS_PATH              "dbus_path"
#define CONFIG_DBUS_INTERFACE         "dbus_interface"
#define CONFIG_NAME                   "name"
#define CONFIG_BUS_NAME               "bus_name"
#define CONFIG_NODE_NAME              "node_name"
#define CONFIG_EARLY                  "early"
#define CONFIG_NAME                   "name"
#define CONFIG_DOMAIN_NAME            "domain_name"
#define CONFIG_CONNECTION_SCRIPT      "connection_script"
#define CONFIG_DISCONNECTION_SCRIPT   "disconnection_script"
#define CONFIG_CLASS                  "class"
#define CONFIG_VOLUME                 "volume"
#define CONFIG_MAIN_VOLUME            "main_volume"
#define CONFIG_VISIBLE                "visible"
#define CONFIG_AVAILABILITY_STATUS    "availability_status"
#define CONFIG_AVAILABILITY_REASON    "availability_reason"
#define CONFIG_MUTE                   "mute"
#define CONFIG_INTERRUPT              "interrupt"

#define CONFIG_SINK_DOMAIN            "sink_domain"
#define CONFIG_SOURCE_DOMAIN          "source_domain"

typedef struct {
    mrp_mainloop_t *ml;
    mrp_dbus_t *dbus;
    mrp_sighandler_t *sighandler;

    char *dbus_bus;
    char *dbus_address;

    int pending_dbus_calls;

    mrp_list_hook_t domains;
} ctx_t;

typedef struct {
    /* configuration file */
    char *name;
    char *bus_name;
    char *node_name;
    bool early;

    char *dbus_path;
    char *dbus_interface;
    char *connection_script;
    char *disconnection_script;

    /* audio manager */
    uint16_t domain_id;
    uint16_t state;

    mrp_list_hook_t sources;
    mrp_list_hook_t sinks;
    mrp_list_hook_t gateways;
    mrp_list_hook_t connections;

    mrp_list_hook_t hook;
    ctx_t *ctx;
} domain_t;

typedef struct {
    uint16_t id;
    char *sink;
    char *source;

    mrp_list_hook_t hook;
} connection_t;

typedef struct {
    uint16_t id;
    uint16_t state;

    char *name;

    char *sink_name;
    char *source_name;
    char *sink_domain_name;
    char *source_domain_name;

    /* these are found only after registration */
    uint16_t sink_id;
    uint16_t source_id;
    uint16_t domain_sink_id;
    uint16_t domain_source_id;

    mrp_list_hook_t hook;
    domain_t *d;
} gateway_t;

typedef struct {
    uint16_t id;
    uint16_t state;

    char *name;
    int16_t volume;
    int16_t main_volume;
    int16_t mute;

    bool visible;
    uint16_t klass;

    int16_t availability_reason;
    int16_t availability_status;

    mrp_list_hook_t hook;
    domain_t *d;
} sink_t;

typedef struct {
    uint16_t id;
    uint16_t state;

    char *name;
    int16_t volume;
    int16_t main_volume;
    uint16_t interrupt;

    bool visible;
    uint16_t klass;

    int16_t availability_reason;
    int16_t availability_status;

    mrp_list_hook_t hook;
    domain_t *d;
} source_t;


void destroy_sink(sink_t *s)
{
    if (!s)
        return;

    mrp_list_delete(&s->hook);

    mrp_free(s->name);
    mrp_free(s);
}

void destroy_source(source_t *s)
{
    if (!s)
        return;

    mrp_list_delete(&s->hook);

    mrp_free(s->name);
    mrp_free(s);
}

void destroy_connection(connection_t *c)
{
    if (!c)
        return;

    mrp_list_delete(&c->hook);

    mrp_free(c->sink);
    mrp_free(c->source);
    mrp_free(c);
}

void destroy_gateway(gateway_t *gw)
{
    if (!gw)
        return;

    mrp_free(gw->name);
    mrp_free(gw->sink_name);
    mrp_free(gw->source_name);
    mrp_free(gw->sink_domain_name);
    mrp_free(gw->source_domain_name);

    mrp_free(gw);
}

void destroy_domain(domain_t *d)
{
    mrp_list_hook_t *sp, *sn;

    if (!d)
        return;

    mrp_list_delete(&d->hook);

    mrp_list_foreach(&d->sinks, sp, sn) {
        sink_t *s = mrp_list_entry(sp, typeof(*s), hook);
        destroy_sink(s);
    }

    mrp_list_foreach(&d->sources, sp, sn) {
        source_t *s = mrp_list_entry(sp, typeof(*s), hook);
        destroy_source(s);
    }

    mrp_list_foreach(&d->gateways, sp, sn) {
        gateway_t *gw = mrp_list_entry(sp, typeof(*gw), hook);
        destroy_gateway(gw);
    }

    mrp_list_foreach(&d->connections, sp, sn) {
        connection_t *c = mrp_list_entry(sp, typeof(*c), hook);
        destroy_connection(c);
    }

    mrp_free(d->connection_script);
    mrp_free(d->disconnection_script);
    mrp_free(d->name);
    mrp_free(d->node_name);
    mrp_free(d->bus_name);
    mrp_free(d->dbus_path);
    mrp_free(d->dbus_interface);
    mrp_free(d);
}

static int method_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *msg, void *data)
{
    const char *member = mrp_dbus_msg_member(msg);
    const char *iface = mrp_dbus_msg_interface(msg);
    const char *path = mrp_dbus_msg_path(msg);

    uint16_t error_code = E_NOT_POSSIBLE;
    mrp_dbus_msg_t *reply;

    ctx_t *ctx = data;

    printf("Method callback called -- member: '%s', path: '%s',"
            " interface: '%s'\n", member, path, iface);

    if (strcmp(member, AUDIOMGR_CONNECT) == 0) {
        mrp_dbus_msg_t *ack;

        char *sink_name = NULL;
        char *source_name = NULL;

        uint16_t handle;
        uint16_t connection;
        uint16_t source;
        uint16_t sink;
        int32_t format;

        domain_t *connection_domain = NULL;
        sink_t *domain_sink = NULL;
        source_t *domain_source = NULL;

        mrp_list_hook_t *sp, *sn, *kp, *kn;

        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_UINT16, &handle);
        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_UINT16, &connection);
        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_UINT16, &source);
        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_UINT16, &sink);
        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_INT32, &format);

        error_code = E_OK;

        mrp_dbus_reply(dbus, msg, MRP_DBUS_TYPE_INT16, &error_code,
                MRP_DBUS_TYPE_INVALID);

        /* find the gateway */

        printf("connect! h: %u, conn: %u, source: %u, sink: %u, format: %i\n",
                handle, connection, source, sink, format);

        mrp_list_foreach(&ctx->domains, kp, kn) {
            domain_t *d;
            d = mrp_list_entry(kp, typeof(*d), hook);

            if (strcmp(iface, d->dbus_interface) == 0 &&
                    strcmp(path, d->dbus_path) == 0) {
                connection_domain = d;
                break;
            }
        }

        mrp_list_foreach(&ctx->domains, kp, kn) {
            domain_t *d;
            d = mrp_list_entry(kp, typeof(*d), hook);
            mrp_list_foreach(&d->sinks, sp, sn) {
                sink_t *s;
                s = mrp_list_entry(sp, typeof(*s), hook);
                if (s->id == sink) {
                    sink_name = s->name;
                    domain_sink = s;
                }
            }
            mrp_list_foreach(&d->sources, sp, sn) {
                source_t *s;
                s = mrp_list_entry(sp, typeof(*s), hook);
                if (s->id == source) {
                    source_name = s->name;
                    domain_source = s;
                }
            }
        }

        printf("sink: %s, source: %s\n",
                sink_name ? sink_name : "(NULL)",
                source_name ? source_name : "(NULL)");

        if (!sink_name || !source_name) {
            error_code = E_NOT_POSSIBLE;
        }
        else {
            /* create connection struct for book keeping */

            connection_t *c = mrp_allocz(sizeof(connection_t));

            c->id = connection;
            c->sink = mrp_strdup(sink_name);
            c->source = mrp_strdup(source_name);
            mrp_list_init(&c->hook);

            mrp_list_append(&connection_domain->connections, &c->hook);

            if (connection_domain->connection_script) {
                char buf[256];
                int ret;

                /* FIXME: sanity check of the string */

                ret = snprintf(buf, 256, "%s %s %s %i %i %i %i %u",
                        connection_domain->connection_script,
                        source_name, sink_name,
                        domain_source->volume, domain_source->main_volume,
                        domain_sink->volume, domain_sink->main_volume,
                        c->id);

                if (ret > 0 && ret != 256) {
                    printf("calling: '%s'\n", buf);
                    system(buf);
                }
            }
        }

        /* send back the ack after processing */

        ack = mrp_dbus_msg_method_call(dbus, AUDIOMGR_DBUS_NAME,
                AUDIOMGR_DBUS_ROUTE_PATH, AUDIOMGR_DBUS_INTERFACE,
                AUDIOMGR_CONNECT_ACK);

        mrp_dbus_msg_append_basic(ack, MRP_DBUS_TYPE_UINT16, &handle);
        mrp_dbus_msg_append_basic(ack, MRP_DBUS_TYPE_UINT16, &connection);
        mrp_dbus_msg_append_basic(ack, MRP_DBUS_TYPE_UINT16, &error_code);

        mrp_dbus_send_msg(dbus, ack);
    }
    else if (strcmp(member, AUDIOMGR_DISCONNECT) == 0) {
        mrp_dbus_msg_t *ack;

        uint16_t handle;
        uint16_t connection;

        mrp_list_hook_t *sp, *sn, *kp, *kn;

        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_UINT16, &handle);
        mrp_dbus_msg_read_basic(msg, MRP_DBUS_TYPE_UINT16, &connection);

        error_code = E_OK;

        mrp_dbus_reply(dbus, msg, MRP_DBUS_TYPE_INT16, &error_code,
                MRP_DBUS_TYPE_INVALID);

        /* TODO: process command here */

        mrp_list_foreach(&ctx->domains, sp, sn) {
            domain_t *d;
            d = mrp_list_entry(sp, typeof(*d), hook);
            mrp_list_foreach(&d->connections, kp, kn) {
                connection_t *c = mrp_list_entry(kp, typeof(*c), hook);
                if (c->id == connection) {
                    if (d->disconnection_script) {
                        char buf[256];
                        int ret;

                        /* FIXME: sanity check of the string */

                        ret = snprintf(buf, 256, "%s %s %s %u",
                                d->disconnection_script,
                                c->source, c->sink, c->id);

                        if (ret > 0 && ret != 256) {
                            printf("calling: '%s'\n", buf);
                            system(buf);
                        }
                    }

                    destroy_connection(c);
                }
            }
        }

        /* send back the ack after processing */

        ack = mrp_dbus_msg_method_call(dbus, AUDIOMGR_DBUS_NAME,
                AUDIOMGR_DBUS_ROUTE_PATH, AUDIOMGR_DBUS_INTERFACE,
                AUDIOMGR_DISCONNECT_ACK);

        mrp_dbus_msg_append_basic(ack, MRP_DBUS_TYPE_UINT16, &handle);
        mrp_dbus_msg_append_basic(ack, MRP_DBUS_TYPE_UINT16, &connection);
        mrp_dbus_msg_append_basic(ack, MRP_DBUS_TYPE_UINT16, &error_code);

        mrp_dbus_send_msg(dbus, ack);
    }

    return TRUE;
}

void register_gateway_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    gateway_t *gw = user_data;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR register gateway cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &gw->id);
    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &gw->state);

    printf("register gateway cb: id %u, state %u\n",
            gw->id, gw->state);
}

static bool register_gateway(gateway_t *gw, uint16_t id, char *name,
        uint16_t sink_id, uint16_t source_id, uint16_t domain_sink_id,
        uint16_t domain_source_id, uint16_t control_domain_id)
{
    int16_t i;
    uint16_t error = E_OK;

    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(gw->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_GATEWAY);

    /* Source domain has to be the domain from which the stream is coming:
     * the routing algorithm builds a tree coming from source domain to all
     * other domains. */

    printf("register gateway: %u, %s, %u, %u, %u, %u, %u\n",
        id, name, sink_id, source_id, domain_sink_id, domain_source_id,
        gw->d->domain_id);

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    /* qsqqqqq */
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, name);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &sink_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &source_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_sink_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_source_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &gw->d->domain_id);

    /* source connection formats ai */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "i");

    {
        int16_t one = 1;
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &one);
    }

    mrp_dbus_msg_close_container(msg);

    /* sink connection formats ai */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "i");

    {
        int16_t one = 1;
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &one);
    }

    mrp_dbus_msg_close_container(msg);

    /* conversion matrix ab*/

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "b");

    for (i = 1; i < 2; i++) {
        uint32_t b = TRUE;
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_BOOLEAN, &b);
    }

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &error);

    mrp_dbus_send(gw->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_GATEWAY, -1,
            register_gateway_cb, gw, msg);

    mrp_dbus_msg_unref(msg);

    gw->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool unregister_gateway(ctx_t *ctx, uint16_t id)
{
    uint16_t error = E_OK;

    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_DEREGISTER_GATEWAY);

    printf("unregistering gateway %u\n", id);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &error);

    mrp_dbus_send_msg(ctx->dbus, msg);

    mrp_dbus_msg_unref(msg);

    return TRUE;
}

static bool unregister_sink(ctx_t *ctx, int16_t id)
{
    uint16_t error = E_OK;

    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_DEREGISTER_SINK);

    printf("unregistering sink %i\n", id);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &error);

    mrp_dbus_send_msg(ctx->dbus, msg);

    mrp_dbus_msg_unref(msg);

    return TRUE;
}

static bool unregister_source(ctx_t *ctx, int16_t id)
{
    uint16_t error = E_OK;

    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_DEREGISTER_SOURCE);

    printf("unregistering source %i\n", id);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &error);

    mrp_dbus_send_msg(ctx->dbus, msg);

    mrp_dbus_msg_unref(msg);

    return TRUE;
}

static bool register_gateways(ctx_t *ctx)
{
    mrp_list_hook_t *sp, *sn, *kp, *kn;

    mrp_list_foreach(&ctx->domains, kp, kn) {
        domain_t *d;
        d = mrp_list_entry(kp, typeof(*d), hook);

        mrp_list_foreach(&d->gateways, sp, sn) {
            gateway_t *gw;
            gw = mrp_list_entry(sp, typeof(*gw), hook);

            register_gateway(gw, 0, gw->name, gw->sink_id, gw->source_id,
                    gw->domain_sink_id, gw->domain_source_id, gw->d->domain_id);
        }
    }

    return TRUE;
}

static void peek_sink_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    gateway_t *gw = user_data;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR peek_sink cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &gw->sink_id);

    printf("peek_sink cb got id %d\n", gw->sink_id);

    gw->d->ctx->pending_dbus_calls--;

    if (gw->d->ctx->pending_dbus_calls > 0)
        return;

    /* start creating gateways now when the sinks and sources have been
     * registered */

    register_gateways(gw->d->ctx);
}

static void peek_source_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    gateway_t *gw = user_data;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR peek_sink cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &gw->source_id);

    gw->d->ctx->pending_dbus_calls--;

    if (gw->d->ctx->pending_dbus_calls > 0)
        return;

    /* start creating gateways now when the sinks and sources have been
     * registered */

    register_gateways(gw->d->ctx);
}

static void peek_sink_domain_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    gateway_t *gw = user_data;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR peek_sink cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &gw->domain_sink_id);

    gw->d->ctx->pending_dbus_calls--;

    if (gw->d->ctx->pending_dbus_calls > 0)
        return;

    /* start creating gateways now when the sinks and sources have been
     * registered */

    register_gateways(gw->d->ctx);
}

static void peek_source_domain_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    gateway_t *gw = user_data;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR peek_sink cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &gw->domain_source_id);

    gw->d->ctx->pending_dbus_calls--;

    if (gw->d->ctx->pending_dbus_calls > 0)
        return;

    /* start creating gateways now when the sinks and sources have been
     * registered */

    register_gateways(gw->d->ctx);
}

static bool peek_gw_sink(gateway_t *gw)
{
    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(gw->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_SINK);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, gw->sink_name);

    mrp_dbus_send(gw->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_SINK, -1,
            peek_sink_cb, gw, msg);

    mrp_dbus_msg_unref(msg);

    gw->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool peek_gw_source(gateway_t *gw)
{
    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(gw->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_SOURCE);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, gw->source_name);

    mrp_dbus_send(gw->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_SOURCE, -1,
            peek_source_cb, gw, msg);

    mrp_dbus_msg_unref(msg);

    gw->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool peek_gw_source_domain(gateway_t *gw)
{
    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(gw->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_DOMAIN);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING,
            gw->source_domain_name);

    mrp_dbus_send(gw->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_DOMAIN, -1,
            peek_source_domain_cb, gw, msg);

    mrp_dbus_msg_unref(msg);

    gw->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool peek_gw_sink_domain(gateway_t *gw)
{
    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(gw->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_DOMAIN);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, gw->sink_domain_name);

    mrp_dbus_send(gw->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_PEEK_DOMAIN, -1,
            peek_sink_domain_cb, gw, msg);

    mrp_dbus_msg_unref(msg);

    gw->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool find_id_mapping(gateway_t *gw)
{
    mrp_list_hook_t *kp, *kn, *sp, *sn;
    domain_t *domain = gw->d;
    bool found = FALSE;
    domain_t *sink_domain = NULL;
    domain_t *source_domain = NULL;

    /* need to go through both domains: sink and source */

    mrp_list_foreach(&gw->d->ctx->domains, sp, sn) {
        domain_t *d = mrp_list_entry(sp, typeof(*d), hook);
        if (strcmp(gw->sink_domain_name, d->name) == 0) {
            sink_domain = d;
        }
        if (strcmp(gw->source_domain_name, d->name) == 0) {
            source_domain = d;
        }
    }

    if (!sink_domain) {
        printf("peeking sink\n");
        peek_gw_sink_domain(gw);
        peek_gw_sink(gw);
    }
    else {
        mrp_list_foreach(&sink_domain->sinks, sp, sn) {
            sink_t *s = mrp_list_entry(sp, typeof(*s), hook);
            if (strcmp(s->name, gw->sink_name) == 0) {
                gw->domain_sink_id = s->d->domain_id;
                gw->sink_id = s->id;
                printf("found GW sink (%s / %u), domain (%s / %u)\n",
                    s->name, s->id, s->d->name, s->d->domain_id);
                found = TRUE;
                break;
            }
        }
    }

    if (!found && sink_domain) {
        printf("ERROR finding sink %s for gateway\n", gw->sink_name);
        goto error;
    }

    found = FALSE;

    if (!source_domain) {
        printf("peeking source\n");
        peek_gw_source_domain(gw);
        peek_gw_source(gw);
    }
    else {
        mrp_list_foreach(&source_domain->sources, sp, sn) {
            source_t *s = mrp_list_entry(sp, typeof(*s), hook);

            if (strcmp(s->name, gw->source_name) == 0) {
                gw->domain_source_id = s->d->domain_id;
                gw->source_id = s->id;
                printf("found GW source (%s / %u), domain (%s / %u)\n",
                    s->name, s->id, s->d->name, s->d->domain_id);
                found = TRUE;
                break;
            }
        }
    }

    if (!found) {
        printf("ERROR finding source for gateway %u\n", gw->id);
        goto error;
    }

    return TRUE;

error:
    return FALSE;
}


static void find_gateway_ids(ctx_t *ctx)
{
    mrp_list_hook_t *sp, *sn, *kp, *kn;

    mrp_list_foreach(&ctx->domains, kp, kn) {
        domain_t *d;
        d = mrp_list_entry(kp, typeof(*d), hook);

        mrp_list_foreach(&d->gateways, sp, sn) {
            gateway_t *gw;
            gw = mrp_list_entry(sp, typeof(*gw), hook);

            find_id_mapping(gw);
        }
    }
}

static void register_source_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    source_t *s = user_data;
    mrp_list_hook_t *sp, *sn;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR register source cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &s->id);
    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &s->state);

    printf("register source cb: id %u, state %u\n",
            s->id, s->state);

    s->d->ctx->pending_dbus_calls--;

    if (s->d->ctx->pending_dbus_calls > 0)
        return;

    /* start creating gateways now when the sinks and sources have been
     * registered */

    find_gateway_ids(s->d->ctx);

    if (s->d->ctx->pending_dbus_calls > 0)
        return;

    /* no extra queries needed to me made */

    register_gateways(s->d->ctx);
}

void register_sink_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    sink_t *s = user_data;
    mrp_list_hook_t *sp, *sn;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR register sink cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &s->id);
    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &s->state);

    printf("register sink cb: id %u, state %u\n",
            s->id, s->state);

    s->d->ctx->pending_dbus_calls--;

    if (s->d->ctx->pending_dbus_calls > 0)
        return;

    /* start creating gateways now when the sinks and sources have been
     * registered */

    find_gateway_ids(s->d->ctx);

    if (s->d->ctx->pending_dbus_calls > 0)
        return;

    /* no extra queries needed to me made */

    register_gateways(s->d->ctx);
}

static bool register_sink(sink_t *s, uint16_t id, char *name, uint16_t domain_id,
        int32_t klass, int16_t volume, bool visible, int32_t avail_status,
        int32_t avail_reason, int16_t mute, int16_t mainvolume)
{
    int16_t i;
    uint32_t visible_u = !!visible;
    int16_t zero = 0;
    int16_t one = 1;

    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(s->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_SINK);

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    /* qsqinb(ii)nn */

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, name);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &klass);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &volume);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_BOOLEAN, &visible_u);

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &avail_status);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &avail_reason);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &mute);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &mainvolume);

    /* sound properties a(in) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(in)");

    for (i = 1; i < 3; i++) {
        mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &i);
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

        mrp_dbus_msg_close_container(msg);
    }

    mrp_dbus_msg_close_container(msg);

    /* connection formats ai */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "i");
    {
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &one);
    }

    mrp_dbus_msg_close_container(msg);

    /* main sound properties a(in) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(in)");

    for (i = 1; i < 3; i++) {
        mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &i);
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

        mrp_dbus_msg_close_container(msg);
    }

    mrp_dbus_msg_close_container(msg);

    /* main notification a(iin) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(iin)");

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_close_container(msg);

    /* notification  a(iin) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(iin)");

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_close_container(msg);

    /* close the main container */
    mrp_dbus_msg_close_container(msg);

    mrp_dbus_send(s->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_SINK, -1,
            register_sink_cb, s, msg);

    mrp_dbus_msg_unref(msg);

    s->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool register_source(source_t *s, uint16_t id, char *name,
        uint16_t domain_id, uint16_t klass, uint16_t state, int16_t volume,
        bool visible, int16_t avail_status, int16_t avail_reason,
        uint16_t interrupt)
{
    int16_t i;
    uint32_t visible_u = !!visible;
    int16_t zero = 0;
    int16_t one = 1;

    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(s->d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_SOURCE);

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    /* qqsqinb(ii)q */

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, name);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &klass);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &state);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &volume);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_BOOLEAN, &visible_u);

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &avail_status);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &avail_reason);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &interrupt);

    /* sound properties a(in) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(in)");

    for (i = 1; i < 3; i++) {

        mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &i);
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

        mrp_dbus_msg_close_container(msg);
    }

    mrp_dbus_msg_close_container(msg);

    /* connection formats ai */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "i");
    {
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &one);
    }

    mrp_dbus_msg_close_container(msg);

    /* main sound properties a(in) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(in)");

    for (i = 1; i < 3; i++) {

        mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &i);
        mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

        mrp_dbus_msg_close_container(msg);
    }

    mrp_dbus_msg_close_container(msg);

    /* main notification a(iin) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(iin)");

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_close_container(msg);

    /* notification  a(iin) */

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_ARRAY, "(iin)");

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);
    
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT32, &zero);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &zero);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_close_container(msg);

    /* close the main container */
    mrp_dbus_msg_close_container(msg);

    mrp_dbus_send(s->d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_SOURCE, -1,
            register_source_cb, s, msg);

    mrp_dbus_msg_unref(msg);

    s->d->ctx->pending_dbus_calls++;

    return TRUE;
}

static bool unregister_domain(domain_t *d, uint16_t domain_id)
{
    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_DEREGISTER_DOMAIN);

    printf("unregistering domain %u\n", domain_id);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_id);

    mrp_dbus_send_msg(d->ctx->dbus, msg);

    mrp_dbus_msg_unref(msg);

    mrp_dbus_remove_method(d->ctx->dbus, d->dbus_path, d->dbus_interface,
        AUDIOMGR_CONNECT_ACK, method_cb, d->ctx);

    mrp_dbus_remove_method(d->ctx->dbus, d->dbus_path, d->dbus_interface,
        AUDIOMGR_DISCONNECT_ACK, method_cb, d->ctx);

    return TRUE;
}

void register_domain_cb(mrp_dbus_t *dbus, mrp_dbus_msg_t *reply,
        void *user_data)
{
    domain_t *domain = user_data;
    ctx_t *ctx = domain->ctx;
    mrp_list_hook_t *kp, *kn, *sp, *sn;

    if (mrp_dbus_msg_is_error(reply)) {
        printf("ERROR registering domain cb\n");
        return;
    }

    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &domain->domain_id);
    mrp_dbus_msg_read_basic(reply, DBUS_TYPE_UINT16, &domain->state);

    printf("register domain cb: domain %u, state %u\n",
            domain->domain_id, domain->state);

    ctx->pending_dbus_calls--;

    if (ctx->pending_dbus_calls > 0)
        return; /* still domains to register */

    /* register sinks and sources that belong to all domains */

    printf("All domains are now registered, register sinks and sources next\n");

    mrp_list_foreach(&ctx->domains, kp, kn) {
        domain_t *d;
        d = mrp_list_entry(kp, typeof(*d), hook);

        mrp_list_foreach(&d->sinks, sp, sn) {
            sink_t *s;
            s = mrp_list_entry(sp, typeof(*s), hook);

            register_sink(s, 0, s->name, d->domain_id, s->klass, s->volume,
                    s->visible, s->availability_status, s->availability_reason,
                    s->mute, s->main_volume);
        }

        mrp_list_foreach(&d->sources, sp, sn) {
            source_t *s;
            s = mrp_list_entry(sp, typeof(*s), hook);

            register_source(s, 0, s->name, d->domain_id, s->klass, 1,
                    s->volume, s->visible, s->availability_status,
                    s->availability_reason, s->interrupt);
        }
    }
}


static bool register_domain(domain_t *d, uint16_t domain_id, char *name,
        char *bus_name, char *node_name, bool early, int16_t state)
{
    mrp_dbus_msg_t *msg = mrp_dbus_msg_method_call(d->ctx->dbus,
            AUDIOMGR_DBUS_NAME, AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_DOMAIN);

    uint32_t early_u = !!early;
    uint32_t complete_u = 1;
    uint32_t error = E_OK;

    /* register methods */

    if (!mrp_dbus_export_method(d->ctx->dbus,
                d->dbus_path, d->dbus_interface, AUDIOMGR_CONNECT,
                method_cb, d->ctx)) {
        printf("Failed to register " AUDIOMGR_CONNECT " method\n");
        exit(1);
    }

    if (!mrp_dbus_export_method(d->ctx->dbus,
                d->dbus_path, d->dbus_interface, AUDIOMGR_DISCONNECT,
                method_cb, d->ctx)) {
        printf("Failed to register " AUDIOMGR_DISCONNECT " method\n");
        exit(1);
    }

    mrp_dbus_msg_open_container(msg, MRP_DBUS_TYPE_STRUCT, NULL);

    /* qsssbbn */
 
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, name);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, bus_name);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, node_name);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_BOOLEAN, &early_u);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_BOOLEAN, &complete_u);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_INT16, &state);

    mrp_dbus_msg_close_container(msg);

    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, d->ctx->dbus_address);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, d->dbus_path);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_STRING, d->dbus_interface);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &domain_id);
    mrp_dbus_msg_append_basic(msg, MRP_DBUS_TYPE_UINT16, &error);

    mrp_dbus_send(d->ctx->dbus, AUDIOMGR_DBUS_NAME,
            AUDIOMGR_DBUS_ROUTE_PATH,
            AUDIOMGR_DBUS_INTERFACE, AUDIOMGR_REGISTER_DOMAIN, -1,
            register_domain_cb, d, msg);

    mrp_dbus_msg_unref(msg);

    d->ctx->pending_dbus_calls++;

    return TRUE;
}

char *get_string_value(ini_parser_keyvaluepair_t *pair)
{
    int len = strlen(pair->value);

    /* remove '"' characters */

    if (len > 2)
        return strndup(pair->value+1, len-2);

    return NULL;
}

bool get_boolean_value(ini_parser_keyvaluepair_t *pair)
{
    if (strcmp(pair->value, "TRUE") == 0)
        return TRUE;

    return FALSE;
}

int32_t get_number_value(ini_parser_keyvaluepair_t *pair)
{
    char *endptr;
    int32_t v;

    v = strtold(pair->value, &endptr);

    if (pair->value == endptr)
        return 0;

    return v;
}

bool parse_gateway(ctx_t *ctx, ini_parser_section_t *section)
{
    mrp_list_hook_t *kp, *kn, *sp, *sn;
    bool found = FALSE;
    gateway_t *gw = mrp_allocz(sizeof(gateway_t));
    char *domain_name = NULL;
    domain_t *d = NULL;

    if (!gw)
        return FALSE;

    mrp_list_init(&gw->hook);

    mrp_list_foreach(&section->pairs, kp, kn) {

        ini_parser_keyvaluepair_t *pair;

        pair = mrp_list_entry(kp, typeof(*pair), hook);

        if (strcmp(pair->key, CONFIG_NAME) == 0) {
            gw->name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DOMAIN_NAME) == 0) {
            domain_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_SINK_DOMAIN) == 0) {
            gw->sink_domain_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_SOURCE_DOMAIN) == 0) {
            gw->source_domain_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_SINK) == 0) {
            gw->sink_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_SOURCE) == 0) {
            gw->source_name = get_string_value(pair);
        }
    }

    if (!domain_name || !gw->sink_name || !gw->source_name ||
            !gw->sink_domain_name || !gw->source_domain_name) {
        printf("ERROR: all mandatory strings not in place\n");
        goto error;
    }

    mrp_list_foreach(&ctx->domains, sp, sn) {
        d = mrp_list_entry(sp, typeof(*d), hook);

        if (strcmp(d->name, domain_name) == 0) {
            gw->d = d;
            mrp_list_append(&d->gateways, &gw->hook);
            printf("found GW domain (%s / %u)\n",
                    d->name, d->domain_id);
            found = TRUE;
            break;
        }
    }

    if (!found) {
        printf("ERROR: domain for gateway not found\n");
        goto error;
    }

    printf("adding gateway %s to list\n", gw->name);
    mrp_list_append(&d->gateways, &gw->hook);

    mrp_free(domain_name);

    return TRUE;

error:
    mrp_free(domain_name);

    return FALSE;
}

bool parse_sink(ctx_t *ctx, ini_parser_section_t *section)
{
    mrp_list_hook_t *kp, *kn, *sp, *sn;
    bool found = FALSE;
    sink_t *s = mrp_allocz(sizeof(sink_t));
    char *domain_name = NULL;

    if (!s)
        return FALSE;

    mrp_list_init(&s->hook);

    mrp_list_foreach(&section->pairs, kp, kn) {

        ini_parser_keyvaluepair_t *pair;

        pair = mrp_list_entry(kp, typeof(*pair), hook);

        if (strcmp(pair->key, CONFIG_NAME) == 0) {
            s->name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DOMAIN_NAME) == 0) {
            domain_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_CLASS) == 0) {
            s->klass = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_VOLUME) == 0) {
            s->volume = get_number_value(pair);
        }
         else if (strcmp(pair->key, CONFIG_MAIN_VOLUME) == 0) {
            s->main_volume = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_VISIBLE) == 0) {
            s->visible = get_boolean_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_MUTE) == 0) {
            s->mute = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_AVAILABILITY_REASON) == 0) {
            s->availability_reason = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_AVAILABILITY_STATUS) == 0) {
            s->availability_status = get_number_value(pair);
        }
    }

    /* TODO: check that we have all the strings */

    if (!s->name || !domain_name)
        goto error;

    mrp_list_foreach(&ctx->domains, sp, sn) {
        domain_t *d = mrp_list_entry(sp, typeof(*d), hook);

        if (strcmp(d->name, domain_name) == 0) {
            s->d = d;
            mrp_list_append(&d->sinks, &s->hook);
            found = TRUE;
            break;
        }
    }

    if (!found)
        goto error;

    mrp_free(domain_name);
    return TRUE;

error:
    destroy_sink(s);
    mrp_free(domain_name);
    return FALSE;
}

bool parse_source(ctx_t *ctx, ini_parser_section_t *section)
{
    mrp_list_hook_t *kp, *kn, *sp, *sn;
    bool found = FALSE;
    source_t *s = mrp_allocz(sizeof(source_t));
    char *domain_name = NULL;

    if (!s)
        return FALSE;

    mrp_list_init(&s->hook);

    mrp_list_foreach(&section->pairs, kp, kn) {

        ini_parser_keyvaluepair_t *pair;

        pair = mrp_list_entry(kp, typeof(*pair), hook);

        if (strcmp(pair->key, CONFIG_NAME) == 0) {
            s->name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DOMAIN_NAME) == 0) {
            domain_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_CLASS) == 0) {
            s->klass = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_VOLUME) == 0) {
            s->volume = get_number_value(pair);
        }
         else if (strcmp(pair->key, CONFIG_MAIN_VOLUME) == 0) {
            s->main_volume = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_VISIBLE) == 0) {
            s->visible = get_boolean_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_INTERRUPT) == 0) {
            s->interrupt = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_AVAILABILITY_REASON) == 0) {
            s->availability_reason = get_number_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_AVAILABILITY_STATUS) == 0) {
            s->availability_status = get_number_value(pair);
        }
    }

    /* TODO: check that we have all the strings */

    if (!s->name || !domain_name)
        goto error;

    mrp_list_foreach(&ctx->domains, sp, sn) {
        domain_t *d = mrp_list_entry(sp, typeof(*d), hook);

        if (strcmp(d->name, domain_name) == 0) {
            s->d = d;
            mrp_list_append(&d->sources, &s->hook);
            found = TRUE;
            break;
        }
    }

    if (!found)
        goto error;

    mrp_free(domain_name);
    return TRUE;

error:
    destroy_source(s);
    mrp_free(domain_name);
    return FALSE;
}

bool parse_domain(ctx_t *ctx, ini_parser_section_t *section)
{
    mrp_list_hook_t *kp, *kn;

    domain_t *d = mrp_allocz(sizeof(domain_t));

    if (!d)
        return FALSE;

    mrp_list_init(&d->hook);
    mrp_list_init(&d->sinks);
    mrp_list_init(&d->sources);
    mrp_list_init(&d->gateways);
    mrp_list_init(&d->connections);

    mrp_list_foreach(&section->pairs, kp, kn) {

        ini_parser_keyvaluepair_t *pair;

        pair = mrp_list_entry(kp, typeof(*pair), hook);

        if (strcmp(pair->key, CONFIG_NAME) == 0) {
            d->name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_BUS_NAME) == 0) {
            d->bus_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_NODE_NAME) == 0) {
            d->node_name = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_EARLY) == 0) {
            d->early = get_boolean_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DBUS_PATH) == 0) {
            d->dbus_path = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DBUS_INTERFACE) == 0) {
            d->dbus_interface = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_CONNECTION_SCRIPT) == 0) {
            d->connection_script = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DISCONNECTION_SCRIPT) == 0) {
            d->disconnection_script = get_string_value(pair);
        }
    }

    if (!d->name || !d->bus_name || !d->node_name || !d->dbus_path
            || !d->dbus_interface)
        goto error;

    /* add domain to the context domain list */

    d->ctx = ctx;

    mrp_list_append(&ctx->domains, &d->hook);

    return TRUE;

error:
    printf("ERROR parsing domain section\n");
    destroy_domain(d);
    return FALSE;
}

bool parse_config(ctx_t *ctx, ini_parser_section_t *section)
{
    mrp_list_hook_t *kp, *kn;

    mrp_list_foreach(&section->pairs, kp, kn) {

        ini_parser_keyvaluepair_t *pair;

        pair = mrp_list_entry(kp, typeof(*pair), hook);

        if (strcmp(pair->key, CONFIG_DBUS_BUS) == 0) {
            ctx->dbus_bus = get_string_value(pair);
        }
        else if (strcmp(pair->key, CONFIG_DBUS_ADDRESS) == 0) {
            ctx->dbus_address = get_string_value(pair);
        }
    }

    if (!ctx->dbus_bus || !ctx->dbus_address)
        goto error;

    return TRUE;

error:
    printf("ERROR parsing configuration section\n");
    return FALSE;
}

bool init_data_structures(ctx_t *ctx, ini_parser_result_t *result)
{
    mrp_list_hook_t *sp, *sn;

    mrp_list_foreach(&result->sections, sp, sn) {

        ini_parser_section_t *section;

        section = mrp_list_entry(sp, typeof(*section), hook);

        if (strcmp(section->name, CONFIG_CONFIG) == 0) {
            if (!parse_config(ctx, section))
                goto error;
        }
        else if (strcmp(section->name, CONFIG_DOMAIN) == 0) {
            if (!parse_domain(ctx, section))
                goto error;
        }
        else if (strcmp(section->name, CONFIG_SINK) == 0) {
            if (!parse_sink(ctx, section))
                goto error;
        }
        else if (strcmp(section->name, CONFIG_SOURCE) == 0) {
            if (!parse_source(ctx, section))
                goto error;
        }
        else if (strcmp(section->name, CONFIG_GATEWAY) == 0) {
            if (!parse_gateway(ctx, section))
                goto error;
        }
        else {
            printf("WARNING: unknown section '%s' in configuration file\n",
                    section->name);
        }
    }

    return TRUE;

error:
    return FALSE;
}

void sig_cb(mrp_sighandler_t *sighandler, int sig, void *user_data)
{
    ctx_t *ctx = (ctx_t *) user_data;

    if (sig == SIGINT) {
        mrp_mainloop_quit(ctx->ml, 0);
    }
}

int main(int argc, char **argv)
{
    ctx_t ctx;
    mrp_dbus_err_t err;
    ini_parser_result_t result;
    mrp_list_hook_t *sp, *sn, *kp, *kn;

    char *bus = "session";
    char *filename = "config.ini";

    if (argc > 1) {
        filename = argv[1];
    }

    if (argc > 2) {
        bus = argv[2];
    }

    /* parse configuration file */

    init_result(&result);

    if (!mrp_parse_ini_file(filename, &result)) {
        printf("failed to parse configuration file '%s'\n", filename);
        exit(1);
    }

    /* initialize the context */

    ctx.ml = mrp_mainloop_create();

    ctx.sighandler = mrp_add_sighandler(ctx.ml, SIGINT, sig_cb, &ctx);

    mrp_list_init(&ctx.domains);

    ctx.pending_dbus_calls = 0;

    if (!init_data_structures(&ctx, &result)) {
        printf("failed to get required information from configuration file\n");
        exit(1);
    }

    /* connect to D-Bus */

    ctx.dbus = mrp_dbus_connect(ctx.ml, bus, mrp_dbus_error_init(&err));

    if (!ctx.dbus) {
        printf("failed to connect to '%s' bus: %s\n", bus, err.message);
        exit(1);
    }

    if (!mrp_dbus_acquire_name(ctx.dbus, ctx.dbus_address, NULL)) {
        printf("Failed to acquire name %s on D-Bus\n", ctx.dbus_address);
        exit(1);
    }

    mrp_list_foreach(&ctx.domains, sp, sn) {
        domain_t *d = mrp_list_entry(sp, typeof(*d), hook);

        register_domain(d, 0, d->name, d->bus_name, d->node_name, d->early,
                DS_CONTROLLED);
    }

    mrp_mainloop_run(ctx.ml);

    mrp_list_foreach(&ctx.domains, sp, sn) {
        domain_t *d = mrp_list_entry(sp, typeof(*d), hook);

        mrp_list_foreach(&d->sinks, kp, kn) {
            sink_t *s = mrp_list_entry(kp, typeof(*s), hook);
            unregister_sink(d->ctx, s->id);
        }

        mrp_list_foreach(&d->sources, kp, kn) {
            source_t *s = mrp_list_entry(kp, typeof(*s), hook);
            unregister_source(d->ctx, s->id);
        }

        mrp_list_foreach(&d->gateways, kp, kn) {
            gateway_t *gw = mrp_list_entry(kp, typeof(*gw), hook);
            unregister_gateway(d->ctx, gw->id);
        }

        unregister_domain(d, d->domain_id);
    }

    mrp_list_foreach(&ctx.domains, sp, sn) {
        domain_t *d = mrp_list_entry(sp, typeof(*d), hook);

        /* TODO: free memory in the unregister callbacks? */
        destroy_domain(d);
    }

    mrp_dbus_release_name(ctx.dbus, ctx.dbus_address, NULL);
    mrp_del_sighandler(ctx.sighandler);
    mrp_mainloop_destroy(ctx.ml);

    printf("Exiting\n");

    exit(0);
}
