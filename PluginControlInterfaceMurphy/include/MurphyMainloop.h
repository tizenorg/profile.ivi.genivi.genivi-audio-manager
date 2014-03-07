/**
 * XXX TODO: add suitable license and copyright
 */

#ifndef MURPHY_MAINLOOP_H_
#define MURPHY_MAINLOOP_H_

#include <string>
#include <list>
#include <map>
#include "config.h"
#include "shared/CAmSocketHandler.h"

#include <murphy/common/mainloop.h>

using namespace am;

namespace mctl
{

  class MurphyMainloop
  {
  public:
    virtual ~MurphyMainloop();
    static mrp_mainloop_t *getMurphyMainloop(CAmSocketHandler *sh);
    mrp_mainloop_t *get_mainloop() { return m_ml; };

    void *addIO(int fd, int16_t events, void *iop);
    void delIO(void *handlep);
    void *addTimer(unsigned int msecs, void *tp);
    void delTimer(void *tp);
    void modTimer(unsigned int msecs, void *handlep);
    void *addDefer(void *tp);
    void delDefer(void *tp);

  private:
    MurphyMainloop(CAmSocketHandler *sh);

    CAmSocketHandler *m_sh;
    mrp_mainloop_t   *m_ml;

    bool registerMainloop();
    void unregisterMainloop();

    TAmShPollPrepare<MurphyMainloop> m_io_prepare_cb;
    TAmShPollCheck<MurphyMainloop> m_io_check_cb;
    TAmShPollFired<MurphyMainloop> m_io_fire_cb;
    TAmShPollDispatch<MurphyMainloop> m_io_dispatch_cb;
    TAmShTimerCallBack<MurphyMainloop> m_timer_cb;
    TAmShTimerCallBack<MurphyMainloop> m_defer_cb;

    void ioPrepareCB(const sh_pollHandle_t h, void* userData);
    bool ioCheckCB(const sh_pollHandle_t h, void *userData);
    void ioFireCB(const pollfd pfd, const sh_pollHandle_t h, void *userData);
    bool ioDispatchCB(const sh_pollHandle_t h, void *userData);
    void timerCB(sh_timerHandle_t h, void *userData);
    void deferCB(sh_timerHandle_t h, void *userData);

    void registerLogger();
  };
}

#endif /* MURPHY_MAINLOOP_H_ */
