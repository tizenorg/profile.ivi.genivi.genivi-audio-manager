/**
 * XXX TODO: sdd suitable license and copyright
 */

#include <config.h>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include "shared/CAmDltWrapper.h"
#include "shared/CAmSocketHandler.h"
#include "MurphyMainloop.h"

#include <stdlib.h>
#include <murphy/common/debug.h>
#include <murphy/common/log.h>
#include <murphy/common/mm.h>
#include <murphy/domain-control/client.h>


/*
 * DLT Murphy context
 */

DLT_DECLARE_CONTEXT(MURPHY)


/*
 * You can use the following environment variable to configure
 * the Murphy debugging infra. Unsetting, or explicitly setting
 * this variable to 'off' will disable debugging. Setting this
 * to a comma-separated list of values will treat those values
 * as Murphy debug sites and enable all of them. For instance:
 *
 *     export GAM_MURPHY_DEBUG="@foo.c,@bar.c,@foobar.cpp,foo_func"
 *
 * will enable the given debug sites.
 */

#define ENVVAR_DBG "GAM_MURPHY_DEBUG"
#define ENVVAR_LOG "GAM_MURPHY_LOG"


/*
 * Murphy superloop (mainloop integration)
 */

extern "C" {

  /*
   * Murphy mainloop AudioManager glue
   */

  typedef struct {
    void *am_ml;
  } am_glue_t;


  /*
   * superloop I/O watch, timer, and deferred callback abstractions
   */

  typedef struct {
    void  *am_h;
    void (*cb)(void *glue_data,
               void *id, int fd, mrp_io_event_t events,
               void *user_data);
    void  *user_data;
    void  *glue_data;
    int    fd;
    short  pending;
  } io_t;

  typedef struct {
    void  *am_h;
    void (*cb)(void *glue_data, void *id, void *user_data);
    void  *user_data;
    void  *glue_data;
  } tmr_t;

  typedef struct {
    void  *am_h;
    void (*cb)(void *glue_data, void *id, void *user_data);
    void  *user_data;
    void  *glue_data;
  } dfr_t;


  /*
   * superloop API operations
   */

  static void *add_io(void *glue_data, int fd, mrp_io_event_t events,
                      void (*cb)(void *glue_data, void *id, int fd,
                                 mrp_io_event_t events, void *user_data),
                      void *user_data);
  static void  del_io(void *glue_data, void *id);
  static void  io_cb(int fd, short mask, void *user_data);

  static void *add_timer(void *glue_data, unsigned int msecs,
                         void (*cb)(void *glue_data, void *id, void *user_data),
                         void *user_data);
  static void  del_timer(void *glue_data, void *id);
  static void  mod_timer(void *glue_data, void *id, unsigned int msecs);
  static void  timer_cb(void *user_data);

  static void *add_defer(void *glue_data,
                         void (*cb)(void *glue_data, void *id, void *user_data),
                         void *user_data);
  static void  del_defer(void *glue_data, void *id);
  static void  mod_defer(void *glue_data, void *id, int enabled);
  static void  defer_cb(void *user_data);

  static void unregister(void *data);


  /*
   * Murphy DLT logging backend
   */

  static void  gamlogger(void *data, mrp_log_level_t level, const char *file,
                         int line, const char *func,
                         const char *format, va_list ap);
} // extern "C"


/*
 * AudioManager mainloop API bridging
 */

static bool am_add_io(am_glue_t *glue, int fd, int mask, io_t *io);
static void am_del_io(am_glue_t *glue, io_t *io);

static bool am_add_timer(am_glue_t *glue, unsigned int msecs, tmr_t *t);
static void am_del_timer(am_glue_t *glue, tmr_t *t);
static void am_mod_timer(am_glue_t *glue, unsigned int msecs, tmr_t *t);

static bool am_add_defer(am_glue_t *glue, dfr_t *d);
static void am_del_defer(am_glue_t *glue, dfr_t *d);
static void am_mod_defer(am_glue_t *glue, dfr_t *d, bool enabled);


/*
 * Murphy superloop operation table
 */

static mrp_superloop_ops_t am_ops = {
  add_io, del_io,
  add_timer, del_timer, mod_timer,
  add_defer, del_defer, mod_defer,
  unregister
};


static void io_cb(int fd, short mask, void *user_data)
{
  io_t *io     = (io_t *)user_data;
  int   events = MRP_IO_EVENT_NONE;

  if (mask & POLLIN)    events |= (int)MRP_IO_EVENT_IN;
  if (mask & POLLOUT)   events |= (int)MRP_IO_EVENT_OUT;
  if (mask & POLLRDHUP) events |= (int)MRP_IO_EVENT_HUP;
  if (mask & POLLHUP)   events |= (int)MRP_IO_EVENT_HUP;
  if (mask & POLLERR)   events |= (int)MRP_IO_EVENT_ERR;

  mrp_debug("I/O CB (0x%x) for %p", events, user_data);

  io->cb(io->glue_data, io, fd, (mrp_io_event_t)events, io->user_data);
}


static void *add_io(void *glue_data, int fd, mrp_io_event_t events,
                    void (*cb)(void *glue_data, void *id, int fd,
                               mrp_io_event_t events, void *user_data),
                    void *user_data)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  int        mask = 0;
  io_t      *io;

  io = (io_t *)calloc(1, sizeof(*io));

  if (io != NULL) {
    if (events & MRP_IO_EVENT_IN)  mask |= POLLIN;
    if (events & MRP_IO_EVENT_OUT) mask |= POLLOUT;
    if (events & MRP_IO_EVENT_HUP) mask |= POLLHUP;
    if (events & MRP_IO_EVENT_ERR) mask |= POLLERR;

    io->fd        = fd;
    io->cb        = cb;
    io->user_data = user_data;
    io->glue_data = glue_data;

    mrp_debug("adding I/O for fd %d, events 0x%x", fd, (int)events);

    if (am_add_io(glue, fd, mask, io))
      return io;

    free(io);
  }

  return NULL;
}


static void del_io(void *glue_data, void *id)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  io_t      *io   = (io_t *)id;

  mrp_debug("deleting I/O %p", id);

  am_del_io(glue, io);

  free(io);
}


static void timer_cb(void *user_data)
{
    tmr_t *t = (tmr_t *)user_data;

    mrp_debug("timer CB for %p", user_data);

    t->cb(t->glue_data, t, t->user_data);
}


static void *add_timer(void *glue_data, unsigned int msecs,
                       void (*cb)(void *glue_data, void *id, void *user_data),
                       void *user_data)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  tmr_t     *t;

  t = (tmr_t *)calloc(1, sizeof(*t));

  if (t != NULL) {
    t->cb        = cb;
    t->user_data = user_data;
    t->glue_data = glue_data;

    mrp_debug("adding timer for %.2f secs", msecs / 1000.0);

    if (am_add_timer(glue, msecs, t))
      return t;

    free(t);
  }

  return NULL;
}


static void del_timer(void *glue_data, void *id)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  tmr_t     *t    = (tmr_t *)id;

  mrp_debug("deleting timer %p", id);

  am_del_timer(glue, t);

  free(t);
}


static void mod_timer(void *glue_data, void *id, unsigned int msecs)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  tmr_t     *t    = (tmr_t *)id;

  if (t != NULL)
    am_mod_timer(glue, msecs, t);
}


static void defer_cb(void *user_data)
{
  dfr_t *d = (dfr_t *)user_data;

  mrp_debug("deferred CB for %p", user_data);

  d->cb(d->glue_data, d, d->user_data);
}


static void *add_defer(void *glue_data,
                       void (*cb)(void *glue_data, void *id, void *user_data),
                       void *user_data)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  dfr_t     *d;

  d = (dfr_t *)calloc(1, sizeof(*d));

  if (d != NULL) {
    d->cb        = cb;
    d->user_data = user_data;
    d->glue_data = glue_data;

    mrp_debug("%s", "adding deferred");

    if (am_add_defer(glue, d))
      return d;

    free(d);
  }

  return NULL;
}


static void del_defer(void *glue_data, void *id)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  dfr_t     *d    = (dfr_t *)id;

  mrp_debug("%s", "deleting deferred");

  am_del_defer(glue, d);

  mrp_free(d);
}


static void mod_defer(void *glue_data, void *id, int enabled)
{
  am_glue_t *glue = (am_glue_t *)glue_data;
  dfr_t     *d    = (dfr_t *)id;

  mrp_debug("%sabling deferred %p", enabled ? "en" : "dis", id);

  am_mod_defer(glue, d, !!enabled);
}


static void unregister(void *data)
{
  free(data);
}


/*
 * AudioManager mainloop API bridging
 */

static bool am_add_io(am_glue_t *glue, int fd, int mask, io_t *io)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  if ((io->am_h = am_ml->addIO(fd, (int16_t)mask, (void *)io)) != NULL)
    return true;
  else
    return false;
}


static void am_del_io(am_glue_t *glue, io_t *io)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  am_ml->delIO(io->am_h);
}


static bool am_add_timer(am_glue_t *glue, unsigned int msecs, tmr_t *t)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  if ((t->am_h = am_ml->addTimer(msecs, (void *)t)) != NULL)
    return true;
  else
    return false;
}


static void am_del_timer(am_glue_t *glue, tmr_t *t)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  am_ml->delTimer(t->am_h);
}


static void am_mod_timer(am_glue_t *glue, unsigned int msecs, tmr_t *t)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  am_ml->modTimer(msecs, t->am_h);
}


static bool am_add_defer(am_glue_t *glue, dfr_t *d)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  if ((d->am_h = am_ml->addDefer((void *)d)) != NULL)
    return true;
  else
    return false;
}


static void am_del_defer(am_glue_t *glue, dfr_t *d)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  am_ml->delDefer(d->am_h);
}


static void am_mod_defer(am_glue_t *glue, dfr_t *d, bool enabled)
{
  mctl::MurphyMainloop *am_ml = (mctl::MurphyMainloop *)glue->am_ml;

  if (enabled && !d->am_h)
    d->am_h = am_ml->addDefer((void *)d);
  else if (!enabled && d->am_h) {
    am_ml->delDefer(d->am_h);
    d->am_h = NULL;
  }
}


namespace mctl {

  /*
   * singleton getter, constructor, destructor
   */

  mrp_mainloop_t *MurphyMainloop::getMurphyMainloop(am::CAmSocketHandler *sh)
  {
    static MurphyMainloop *mainLoop = NULL;

    if (sh == NULL) {
      if (mainLoop != NULL) {
        delete mainLoop;
        mainLoop = NULL;
      }

      return NULL;
    }

    if (mainLoop == NULL)
      mainLoop = new MurphyMainloop(sh);

    if (mainLoop != NULL)
      return mainLoop->get_mainloop();
    else
      return NULL;
  }


  MurphyMainloop::MurphyMainloop(am::CAmSocketHandler *sh) :
    m_sh(sh),
    m_ml(mrp_mainloop_create()),
    m_io_prepare_cb(this, &MurphyMainloop::ioPrepareCB),
    m_io_check_cb(this, &MurphyMainloop::ioCheckCB),
    m_io_fire_cb(this, &MurphyMainloop::ioFireCB),
    m_io_dispatch_cb(this, &MurphyMainloop::ioDispatchCB),
    m_timer_cb(this, &MurphyMainloop::timerCB),
    m_defer_cb(this, &MurphyMainloop::deferCB)
  {
    assert(m_sh != 0);
    assert(m_ml != NULL);

    registerLogger();
    assert(registerMainloop());
  }


  MurphyMainloop::~MurphyMainloop()
  {
    unregisterMainloop();
    mrp_mainloop_destroy(m_ml);
    m_sh = NULL;
    m_ml = NULL;
  }


  /*
   * AudioManager mainloop callback bridging
   */

  void MurphyMainloop::ioPrepareCB(const sh_pollHandle_t h, void* userData)
  {
    MRP_UNUSED(userData);

    mrp_debug("ioPrepareCB for %d", h);
  }


  bool MurphyMainloop::ioCheckCB(const sh_pollHandle_t h, void *userData)
  {
    MRP_UNUSED(userData);

    mrp_debug("ioCheckCB for %d", h);

    return true;
  }


  bool MurphyMainloop::ioDispatchCB(const sh_pollHandle_t h, void *userData)
  {
    io_t *io = (io_t *)userData;

    mrp_debug("ioDispatchCB for %p (%d)", userData, h);

    io_cb(io->fd, io->pending, io);
    io->pending = 0;

    return false;
  }


  void MurphyMainloop::ioFireCB(const pollfd pfd, const sh_pollHandle_t h,
                                   void *userData)
  {
    io_t *io = (io_t *)userData;

    mrp_debug("ioFireCB for %d", h);

    io->pending = pfd.revents;
  }


  void MurphyMainloop::timerCB(sh_timerHandle_t h, void *userData)
  {
    mrp_debug("timerCB for %p (%d)", userData, h);

    m_sh->restartTimer(h);

    timer_cb(userData);
  }


  void MurphyMainloop::deferCB(sh_timerHandle_t h, void *userData)
  {
    mrp_debug("deferCB for %p (%d)", userData, h);

    m_sh->restartTimer(h);

    defer_cb(userData);
  }


  /*
   * AudioManager mainloop API bridging
   */

  void *MurphyMainloop::addIO(int fd, int16_t events, void *iop)
  {
    sh_pollHandle_t *handle = new sh_pollHandle_t;

    if (m_sh->addFDPoll(fd, events, &m_io_prepare_cb, &m_io_fire_cb,
                        &m_io_check_cb, &m_io_dispatch_cb,
                        iop, *handle) == E_OK)
      return (void *)handle;
    else {
      mrp_log_error("failed to add native AudioManager I/O watch");

      delete handle;
      return NULL;
    }
  }


  void MurphyMainloop::delIO(void *handlep)
  {
    sh_pollHandle_t *handle = (sh_pollHandle_t *)handlep;

    if (handle)
      m_sh->removeFDPoll(*handle);
  }


  void *MurphyMainloop::addTimer(unsigned int msecs, void *tp)
  {
    sh_timerHandle_t *handle = new sh_timerHandle_t;
    timespec          ts;

    ts.tv_sec  = msecs / 1000;
    ts.tv_nsec = 1000000 * (msecs % 1000);

    if (ts.tv_sec == 0 && ts.tv_nsec == 0)
      ts.tv_nsec = 1;

    if (m_sh->addTimer(ts, &m_timer_cb, *handle, tp) == E_OK)
      return (void *)handle;
    else {
      mrp_log_error("failed to add native AudioManager timer");

      delete handle;
      return NULL;
    }
  }


  void MurphyMainloop::delTimer(void *handlep)
  {
    sh_timerHandle_t *handle = (sh_timerHandle_t *)handlep;

    if (handle)
      m_sh->removeTimer(*handle);
  }


  void MurphyMainloop::modTimer(unsigned int msecs, void *handlep)
  {
    sh_timerHandle_t *handle = (sh_timerHandle_t *)handlep;
    timespec          ts;

    if (handle) {
      ts.tv_sec  = msecs / 1000;
      ts.tv_nsec = 1000000 * (msecs % 1000);

      if (ts.tv_sec == 0 && ts.tv_nsec == 0)
        ts.tv_nsec = 1;

      if (m_sh->restartTimer(*handle) != E_OK ||
          m_sh->updateTimer(*handle, ts) != E_OK)
        mrp_log_error("failed to update native AudioManager timer");
    }
  }


  void *MurphyMainloop::addDefer(void *dp)
  {
    sh_timerHandle_t *handle = new sh_timerHandle_t;
    timespec          ts     = { 0, 1 };

    if (m_sh->addTimer(ts, &m_defer_cb, *handle, dp) == E_OK)
      return (void *)handle;
    else {
      delete handle;
      return NULL;
    }
  }


  void MurphyMainloop::delDefer(void *handlep)
  {
    sh_timerHandle_t *handle = (sh_timerHandle_t *)handlep;

    if (handle)
      m_sh->removeTimer(*handle);
  }


  /*
   * Murphy mainloop registration
   */

  bool MurphyMainloop::registerMainloop()
  {
    am_glue_t *glue;

    glue = (am_glue_t *)calloc(1, sizeof(*glue));

    if (glue != NULL) {
      glue->am_ml = (void *)this;

      if (mrp_set_superloop(m_ml, &am_ops, glue))
        return true;
      else
        free(glue);
    }

    return false;
  }


  void MurphyMainloop::unregisterMainloop()
  {
    mrp_mainloop_unregister(m_ml);
    m_ml = NULL;
  }


  /*
   * Murphy logging/debugging setup and DLT backend registration
   */

  void MurphyMainloop::registerLogger()
  {
    am::CAmDltWrapper *dlt = am::getWrapper();
    const char        *dbg, *log, *p, *n;
    char               site[1024];
    size_t             l;

    dlt->registerContext(MURPHY, "MRP", "Murphy Infrastructure");

    if (mrp_log_register_target("AudioManager", gamlogger, NULL))
        mrp_log_set_target("AudioManager");

    if ((log = getenv(ENVVAR_LOG)) == NULL)
      log = "info,error,warning";

    mrp_log_enable(mrp_log_parse_levels(log));

    if ((dbg = getenv(ENVVAR_DBG)) == NULL)
      dbg = "off";

    if (strcmp(dbg, "off")) {
      mrp_log_info("Enabling Murphy debugging (%s).", dbg);
      mrp_debug_enable(true);

      p = dbg;
      while (p != NULL) {
        n = strchr(p, ';');
        l = n ? n - p : strlen(p);

        if (l < sizeof(site) - 1) {
          strncpy(site, p, l);
          site[l] = '\0';
          mrp_log_info("Enabling Murphy debug site '%s'.", site);
          mrp_debug_set_config(site);
        }

        p = n ? n + 1 : NULL;
      }
    }
  }
}


/*
 * Murphy DLT logger backend
 */

static void dltlog(DltLogLevelType type, const char *prefix, const char *msg)
{
  am::CAmDltWrapper *dlt = am::getWrapper();

  dlt->init(type, &MURPHY);
  if (prefix)
    dlt->append(prefix);
  dlt->append(msg);
  dlt->send();
}


static void gamlogger(void *data, mrp_log_level_t level, const char *file,
                      int line, const char *func,
                      const char *format, va_list ap)
{
    va_list         cp;
    DltLogLevelType type;
    char            msg[1024], locbuf[1024], *loc;

    MRP_UNUSED(data);
    MRP_UNUSED(file);
    MRP_UNUSED(line);

    va_copy(cp, ap);
    switch (level) {
    case MRP_LOG_ERROR:   type = DLT_LOG_ERROR; loc = NULL; break;
    case MRP_LOG_WARNING: type = DLT_LOG_WARN;  loc = NULL; break;
    case MRP_LOG_INFO:    type = DLT_LOG_INFO;  loc = NULL; break;
    default:              type = DLT_LOG_INFO;  loc = NULL; break;
    case MRP_LOG_DEBUG:   type = DLT_LOG_DEBUG;
      snprintf(locbuf, sizeof(locbuf), "[%s] ", func ? func : "<unknown>");
      loc = locbuf;
    }

    if (vsnprintf(msg, sizeof(msg), format, cp) < (ssize_t)sizeof(msg))
      dltlog(type, loc, msg);

    va_end(cp);
}
