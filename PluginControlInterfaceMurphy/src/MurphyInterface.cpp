/**
 * XXX TODO: add an appropriate copyright and license notice
 */

#include <cassert>
#include <sstream>

#include <murphy/common/log.h>
#include <murphy/common/mm.h>

#include "CAmControlSenderMurphy.h"

#include "MurphyInterface.h"
#include "Connection.h"

//
// tables we export to and import from Murphy
//
#define AM_SINK_TABLE_ID   0
#define AM_SINK_TABLE     "audio_manager_sinks"
#define AM_SINK_NCOLUMN    4
#define AM_SINK_COLUMNS   "id        integer    ," \
                          "name      varchar(32)," \
                          "available integer    ," \
                          "visible   integer     "
#define AM_SINK_INDEX     "id"

#define AM_SOURCE_TABLE_ID 1
#define AM_SOURCE_TABLE   "audio_manager_sources"
#define AM_SOURCE_NCOLUMN  4
#define AM_SOURCE_COLUMNS "id        integer    ," \
                          "name      varchar(32)," \
                          "available integer    ," \
                          "visible   integer     "
#define AM_SOURCE_INDEX   "id"

#define PLAYBACK_TABLE_ID  0
#define PLAYBACK_TABLE     "audio_playback_users"
#define PLAYBACK_COLUMNS   "rsetid,source_id,sink_id,connid,decision"
#define PLAYBACK_RSETID    0
#define PLAYBACK_CONNID    3
#define PLAYBACK_VERDICT   4
#define PLAYBACK_SELECT    NULL

#define RECORDING_TABLE_ID 1
#define RECORDING_TABLE    "audio_recording_users"
#define RECORDING_COLUMNS  "rsetid,source_id,sink_id,connid,decision"
#define RECORDING_CONNID   3
#define RECORDING_VERDICT  4
#define RECORDING_SELECT   NULL

using namespace mctl;

am::CAmSocketHandler *MurphyInterface::mSocketHandler = NULL;
MurphyInterface *MurphyInterface::mMurphyInterface = NULL;


/* static */
void MurphyInterface::setSocketHandler(am::CAmSocketHandler *socketHandler)
{
  assert(mSocketHandler == NULL);
  assert(socketHandler != NULL);

  MurphyInterface::mSocketHandler = socketHandler;
}


/* static */
MurphyInterface *MurphyInterface::getMurphyInterface()
{
  if (mMurphyInterface == NULL)
    mMurphyInterface = new MurphyInterface();

  return mMurphyInterface;
}


void MurphyInterface::setControlReceiver(am::IAmControlReceive *receiver)
{
  mControlReceiver = receiver;
}


void MurphyInterface::setControlSender(am::CAmControlSenderMurphy *sender)
{
  mControlSender = sender;
}


MurphyInterface::MurphyInterface()
  : mControlReceiver(NULL),
    mControlSender(NULL),
    mMainloop(NULL),
    mDomCtl(NULL),
    mSsuTimer(NULL),
    mSsuExpectMore(false)
{
  assert(mSocketHandler != NULL);
  CreateMainloop();
  assert(mMainloop != NULL);
  CreateDomainController();
  RegisterDomainFunctions();
}


MurphyInterface::~MurphyInterface()
{
  CancelSinkAndSourceUpdate();
  DestroyDomainController();
  DestroyMainloop();
}


void MurphyInterface::CreateMainloop()
{
  assert(mSocketHandler != NULL);

  mMainloop = MurphyMainloop::getMurphyMainloop(mSocketHandler);
}


void MurphyInterface::DestroyMainloop()
{
  MurphyMainloop::getMurphyMainloop(NULL);
  mMainloop = NULL;
}


void MurphyInterface::CreateDomainController()
{
  mrp_domctl_table_t tables[] = {
    MRP_DOMCTL_TABLE(AM_SINK_TABLE, AM_SINK_COLUMNS, AM_SINK_INDEX),
    MRP_DOMCTL_TABLE(AM_SOURCE_TABLE, AM_SOURCE_COLUMNS, AM_SOURCE_INDEX)
  };
  int ntable = MRP_ARRAY_SIZE(tables);
  mrp_domctl_watch_t watches[] = {
    MRP_DOMCTL_WATCH(PLAYBACK_TABLE , PLAYBACK_COLUMNS , PLAYBACK_SELECT , 0),
    MRP_DOMCTL_WATCH(RECORDING_TABLE, RECORDING_COLUMNS, RECORDING_SELECT, 0),
  };
  int nwatch = MRP_ARRAY_SIZE(watches);

  mDomCtl = mrp_domctl_create("audio-manager", mMainloop,
                              tables, ntable, watches, nwatch,
                              &MurphyInterface::DomainConnectionCB,
                              &MurphyInterface::DataImportCB,
                              (void *)this);

  assert(mDomCtl != NULL);
  assert(mrp_domctl_connect(mDomCtl, MRP_DEFAULT_DOMCTL_ADDRESS, 0));

  ExpectSinkAndSourceChanges(true);
}


void MurphyInterface::DestroyDomainController()
{
  mrp_domctl_disconnect(mDomCtl);
  mrp_domctl_destroy(mDomCtl);
  mDomCtl = NULL;
}


static int connect_request_cb(mrp_domctl_t *ctl,
                              uint32_t narg, mrp_domctl_arg_t *args,
                              uint32_t *nout, mrp_domctl_arg_t *outs,
                              void *user_data)
{
  MurphyInterface           *self = static_cast<MurphyInterface *>(user_data);
  const char                *error;
  am::am_sinkID_t            sinkID;
  am::am_sourceID_t          sourceID;
  am::am_mainConnectionID_t  connID;
  uint32_t                   rsetID;

  MRP_UNUSED(ctl);

  if (narg != 3) {
    error = "called with invalid number of arguments (source and sink expected)";
    goto error;
  }

  if (args[0].type != MRP_DOMCTL_UINT16 ||
      args[1].type != MRP_DOMCTL_UINT16 ||
      args[2].type != MRP_DOMCTL_UINT32) {
    error = "called with invalid types of arguments";
    goto error;
  }

  *nout    = 0;
  sourceID = args[0].u16;
  sinkID   = args[1].u16;
  rsetID   = args[2].u32;

  mrp_log_info("Got connect request %u -> %u (rset: #%u).", sourceID, sinkID,
               rsetID);

  if (self->ConnectSourceToSink(sourceID, sinkID, rsetID, connID))
    mrp_log_info("Connected %u -> %u as %u.", sourceID, sinkID, connID);
  else {
    mrp_log_error("Failed to connect %u -> %u.", sourceID, sinkID);
    connID = 0;
  }

  return connID;

 error:
  mrp_log_error("Connect request failed (error: %s).",
                error ? "error" : "<unknown>");

  *nout        = 1;
  outs[0].type = MRP_DOMCTL_STRING;
  outs[0].str  = mrp_strdup(error);

  return 0;
}


bool MurphyInterface::RegisterDomainFunctions()
{
  mrp_domctl_method_def_t methods[] = {
    { "connect", 1, connect_request_cb, this },
  };

  if (mrp_domctl_register_methods(mDomCtl, methods, MRP_ARRAY_SIZE(methods)))
    return true;
  else
    return false;
}


bool MurphyInterface::ConnectSourceToSink(am::am_sourceID_t source,
                                          am::am_sinkID_t sink,
                                          uint32_t rset,
                                          am::am_mainConnectionID_t & conn)
{
  if (mControlSender->CreateConnection(source, sink, rset, conn) != am::E_OK)
    return false;
  else
    return true;
#if 0
  if (mControlSender->hookUserConnectionRequest(source, sink, conn) != am::E_OK)
    return false;
  else
    return true;
#endif
}


static void ssu_update_cb(mrp_timer_t *t, void *user_data)
{
  MRP_UNUSED(t);

  MurphyInterface *self = static_cast<MurphyInterface *>(user_data);

  self->ExportSinksAndSources();
}


void MurphyInterface::ExpectSinkAndSourceChanges(bool more)
{
  mSsuExpectMore = more;
}


void MurphyInterface::ScheduleSinkAndSourceUpdate()
{
  unsigned int delay;

  if (mSsuExpectMore)
    delay = 100;
  else
    delay = 0;

  if (mSsuTimer != NULL) {
    mrp_debug("rescheduling update of sinks and sources");
    mrp_mod_timer(mSsuTimer, delay);
  }
  else {
    mrp_debug("scheduling update of sinks and sources");
    mSsuTimer = mrp_add_timer(mMainloop, delay, ssu_update_cb, (void *)this);
  }
}


void MurphyInterface::CancelSinkAndSourceUpdate()
{
  mrp_del_timer(mSsuTimer);
  mSsuTimer = NULL;
}


void MurphyInterface::ExportSinksAndSources()
{
  std::vector<am::am_Sink_s> sinks;
  std::vector<am::am_Source_s> sources;
  mrp_domctl_data_t  *tables, *tbl;
  mrp_domctl_value_t *values, *v;
  int nsink, nsource, ntable, nvalue, row;

  mrp_del_timer(mSsuTimer);
  mSsuTimer = NULL;
  ExpectSinkAndSourceChanges(false);

  mrp_log_info("Exporting sinks and sources to Murphy.");

  if (mControlReceiver->getListSources(sources) != am::E_OK ||
      mControlReceiver->getListSinks(sinks) != am::E_OK) {
    mrp_log_error("Failed to get list of sources and/or sinks.");
    return;
  }

  nsink   = sinks.size();
  nsource = sources.size();
  ntable  = 2;
  nvalue  = nsink * AM_SINK_NCOLUMN + nsource * AM_SOURCE_NCOLUMN;

  tables  = (mrp_domctl_data_t  *)alloca(sizeof(*tables) * ntable);
  values  = (mrp_domctl_value_t *)alloca(sizeof(*values) * nvalue);
  v       = values;

  tbl = tables;

  tbl->id      = tbl - tables;
  tbl->ncolumn = AM_SINK_NCOLUMN;
  tbl->nrow    = nsink;
  tbl->rows    = (mrp_domctl_value_t **)alloca(tbl->nrow * sizeof(tbl->rows[0]));

  std::vector<am::am_Sink_s>::iterator sink(sinks.begin());
  row = 0;
  while (sink != sinks.end()) {
    tbl->rows[row] = v;

    v[0].type = MRP_DOMCTL_INTEGER;
    v[0].s32 = sink->sinkID;
    v[1].type = MRP_DOMCTL_STRING;
    v[1].str = sink->name.c_str();
    v[2].type = MRP_DOMCTL_INTEGER;
    v[2].s32 = sink->available.availability == am::A_AVAILABLE;
    v[3].type = MRP_DOMCTL_INTEGER;
    v[3].s32 = sink->visible ? 1 : 0;

    v += AM_SINK_NCOLUMN;
    sink++;
    row++;
  }

  tbl++;
  tbl->id      = tbl - tables;
  tbl->ncolumn = AM_SOURCE_NCOLUMN;
  tbl->nrow    = nsource;
  tbl->rows    = (mrp_domctl_value_t **)alloca(tbl->nrow * sizeof(tbl->rows[0]));

  std::vector<am::am_Source_s>::iterator source(sources.begin());
  row = 0;
  while (source != sources.end()) {
    tbl->rows[row] = v;

    v[0].type = MRP_DOMCTL_INTEGER;
    v[0].s32 = source->sourceID;
    v[1].type = MRP_DOMCTL_STRING;
    v[1].str = source->name.c_str();
    v[2].type = MRP_DOMCTL_INTEGER;
    v[2].s32 = source->available.availability == am::A_AVAILABLE;
    v[3].type = MRP_DOMCTL_INTEGER;
    v[3].s32 = source->visible ? 1 : 0;

    v += AM_SOURCE_NCOLUMN;
    source++;
    row++;
  }

  if (!mrp_domctl_set_data(mDomCtl, tables, ntable, &MurphyInterface::DataExportCB, this))
    mrp_log_error("Failed to export of sinks and sources.");
}


static void request_route_reply_cb(mrp_domctl_t *dc, int error, int retval,
                                   uint32_t narg, mrp_domctl_arg_t *args,
                                   void *user_data)
{
  MurphyInterface      *self = mctl::MurphyInterface::getMurphyInterface();
  mctl::MainConnection *conn = static_cast<mctl::MainConnection *>(user_data);
  uint16_t             *route;
  size_t                nhop;

  MRP_UNUSED(dc);

  if (error != 0 || retval != 0) {
    mrp_log_error("Route request failed with error %d.", error);
    return;
  }

  if (narg != 1) {
    mrp_log_error("Received invalid route reply (%d args).", narg);
    return;
  }

  if (args[0].type != MRP_DOMCTL_ARRAY(UINT16)) {
    mrp_log_error("Received invalid route reply (wrong type of argument).");
    return;
  }

  route = (uint16_t *)args[0].arr;
  nhop  = args[0].size;

  if (nhop < 2) {
    mrp_log_error("Received invalid route reply (fewer than 2 hops).");
    return;
  }

  self->ProcessRouteRequestReply(conn, route, nhop);
}


bool MurphyInterface::RequestRoute(MainConnection *conn,
                                   std::vector<am::am_Route_s> routes)
{
  am::am_sourceID_t  source = conn->mainSourceID();
  am::am_sinkID_t    sink   = conn->mainSinkID();
  mrp_domctl_arg_t  *args;
  int                narg, i;
  uint16_t           ends[2], *path;
  size_t             j, len;

  mrp_log_info("Requesting route for %u -> %u", source, sink);

  narg = 3 + routes.size();
  args = (mrp_domctl_arg_t *)alloca(narg * sizeof(args[0]));

  ends[0] = source;
  ends[1] = sink;
  args[0].type = (mrp_domctl_type_t)MRP_DOMCTL_ARRAY(UINT16);
  args[0].arr  = ends;
  args[0].size = 2;

  args[1].type = (mrp_domctl_type_t)MRP_DOMCTL_UINT16;
  args[1].u32  = conn->getID();

  args[2].type = (mrp_domctl_type_t)MRP_DOMCTL_UINT32;
  args[2].u32  = conn->rsetID();

  std::vector<am::am_Route_s>::iterator route(routes.begin());

  i = 3;
  while (route != routes.end()) {
    std::vector<am::am_RoutingElement_s>::iterator re(route->route.begin());
    len  = route->route.size();
    path = (uint16_t *)alloca(len * sizeof(path[0]));

    args[i].type = (mrp_domctl_type_t)MRP_DOMCTL_ARRAY(UINT16);
    args[i].size = len * 2;
    args[i].arr  = path;

    j = 0;
    while (re != route->route.end()) {
      path[2 * j + 0] = re->sourceID;
      path[2 * j + 1] = re->sinkID;
      j++;
      re++;
    }

    i++;
    route++;
  }

  if (mrp_domctl_invoke(mDomCtl, "request_route", narg, args,
                        request_route_reply_cb, (void *)conn))
    return true;
  else
    return false;
}


void MurphyInterface::ProcessRouteRequestReply(mctl::MainConnection *conn,
                                               uint16_t *path, size_t nhop)
{
  std::vector<am::am_Route_s> routes;
  am::am_sourceID_t           source;
  am::am_sinkID_t             sink;
  size_t                      len, i;
  bool                        found;

  source = conn->mainSourceID();
  sink   = conn->mainSinkID();

  mrp_log_info("Got route of %zd hops for %u -> %u.", nhop, source, sink);

  mControlReceiver->getRoute(false, source, sink, routes);

  found = false;
  std::vector<am::am_Route_s>::iterator route(routes.begin());

  while (route != routes.end()) {
    std::vector<am::am_RoutingElement_s>::iterator re(route->route.begin());

    len = route->route.size();

    if (nhop != 2 * len)
      goto next_route;

    i = 0;
    while (re != route->route.end()) {
      if (path[2 * i + 0] != re->sourceID || path[2 * i + 1] != re->sinkID)
        break;
      i++;
      re++;
    }

    if (re == route->route.end()) {
      found = true;
      break;
    }

  next_route:
    route++;
  }

  if (!found) {
    mrp_log_error("Chosen routing path does not exist.");
    return;
  }

  conn->setRoute(*route);

#if 0
  std::vector<am::am_Route_s> chosen;
  chosen.push_back(*route);

  mctl::ConnectEvent connev(chosen);
  conn->queueEvent(connev);

  if (conn->getStateID() == mctl::STATE_END)
    delete conn;
#endif
}


bool MurphyInterface::NotifyDisconnect(MainConnection *conn)
{
  mrp_domctl_arg_t args[1];
  int              narg;
  uint16_t         connid;


  connid = conn->getID();
  mrp_log_info("Notifying disconnection of connection %u.", connid);

  narg = 1;
  args[0].type = MRP_DOMCTL_UINT16;
  args[0].u16  = connid;

  mrp_domctl_invoke(mDomCtl, "notify_disconnect", narg, args, NULL, 0);

  return true;
}


void MurphyInterface::SetDecisions(mrp_domctl_data_t *tables, int ntable)
{
  mTables = tables;
  mNtable = ntable;
}


void MurphyInterface::ClearDecisions(void)
{
  SetDecisions(NULL, 0);
}


MurphyInterface::Decision MurphyInterface::Verdict(MainConnection *conn)
{
  mrp_domctl_data_t  *playback = mTables;
  mrp_domctl_value_t *row;
  int                 i;
  uint16_t            cid;
  uint32_t            rsid;
  const char         *verdict;

  cid  = (uint16_t)conn->getID();
  rsid = conn->rsetID();

  mrp_log_info("Checking verdict for connection %u (rsetid %u)", cid, rsid);

  for (i = 0; i < playback->nrow; i++) {
    row = playback->rows[i];

    if (row[PLAYBACK_CONNID].u32 != cid && row[PLAYBACK_RSETID].u32 != rsid)
        continue;

    verdict = row[PLAYBACK_VERDICT].str;

    mrp_log_info("Got verdict '%s' for connection %u", verdict, cid);

    if (!strcmp(verdict, "disconnected")) // no route, stop
      return DECISION_DISCONNECT;
    if (!strcmp(verdict, "connected"))    // route and play
      return DECISION_CONNECT;
    if (!strcmp(verdict, "suspended"))    // route but stop
      return DECISION_SUSPEND;
    if (!strcmp(verdict, "<not yet>"))    // well...
        return DECISION_NOOP;

    return DECISION_TEARDOWN;
  }

  mrp_log_info("No explicit verdict for connection %u => TEARDOWN", cid);

  return DECISION_TEARDOWN;
}


/* static */
void MurphyInterface::DomainConnectionCB(mrp_domctl_t *ctl, int connected, int errcode,
                                         const char *errmsg, void *user_data)
{
  MurphyInterface *self = static_cast<MurphyInterface *>(user_data);

  MRP_UNUSED(ctl);

  if (connected) {
    mrp_log_info("Domain-controller connection established.");
    self->ScheduleSinkAndSourceUpdate();
  }
  else
    mrp_log_error("Domain-controller connection down (%d: %s).",
                  errcode, errmsg);
}


static void dump_data(mrp_domctl_data_t *table)
{
    mrp_domctl_value_t *row;
    int                 i, j;
    char                buf[1024], *p;
    const char         *t;
    int                 n, l;

    mrp_log_info("Table #%d: %d rows x %d columns", table->id,
                 table->nrow, table->ncolumn);

    for (i = 0; i < table->nrow; i++) {
        row = table->rows[i];
        p   = buf;
        n   = sizeof(buf);

        for (j = 0, t = ""; j < table->ncolumn; j++, t = ", ") {
            switch (row[j].type) {
            case MRP_DOMCTL_STRING:
                l  = snprintf(p, n, "%s'%s'", t, row[j].str);
                p += l;
                n -= l;
                break;
            case MRP_DOMCTL_INTEGER:
                l  = snprintf(p, n, "%s%d", t, row[j].s32);
                p += l;
                n -= l;
                break;
            case MRP_DOMCTL_UNSIGNED:
                l  = snprintf(p, n, "%s%u", t, row[j].u32);
                p += l;
                n -= l;
                break;
            case MRP_DOMCTL_DOUBLE:
                l  = snprintf(p, n, "%s%f", t, row[j].dbl);
                p += l;
                n -= l;
                break;
            default:
                l  = snprintf(p, n, "%s<invalid column 0x%x>",
                              t, row[j].type);
                p += l;
                n -= l;
            }
        }

        mrp_log_info("row #%d: { %s }", i, buf);
    }
}


/* static */
void MurphyInterface::DataImportCB(mrp_domctl_t *ctl, mrp_domctl_data_t *tables,
                                   int ntable, void *user_data)
{
  MurphyInterface *self = static_cast<MurphyInterface *>(user_data);
  int              i;

  MRP_UNUSED(ctl);

  mrp_log_info("Data import notification (%d tables) from Murphy.", ntable);

  for (i = 0; i < ntable; i++) {
    dump_data(tables + i);
  }


  self->SetDecisions(tables, ntable);
  self->mControlSender->ExecuteActions();
  self->ClearDecisions();

}


/* static */
void MurphyInterface::DataExportCB(mrp_domctl_t *ctl, int errcode, const char *errmsg,
                                   void *user_data)
{
  MurphyInterface *self = static_cast<MurphyInterface *>(user_data);

  MRP_UNUSED(ctl);
  MRP_UNUSED(self);

    if (errcode == 0)
      mrp_log_info("Domain-controller data export ok.");
    else
      mrp_log_error("Domain-controller data export failed (%d: %s).",
                    errcode, errmsg);
}
