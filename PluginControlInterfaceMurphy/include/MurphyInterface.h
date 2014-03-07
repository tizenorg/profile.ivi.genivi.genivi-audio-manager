/**
 * XXX TODO: add an appropriate copyright and license notice
 */

#ifndef MURPHYINTERFACE_H_
#define MURPHYINTERFACE_H_

#include <murphy/common/mainloop.h>
#include <murphy/domain-control/client.h>

#include "config.h"
#include "shared/CAmSocketHandler.h"
#include "control/IAmControlReceive.h"
#include "control/IAmControlSend.h"
#include "MurphyMainloop.h"
#include "Connection.h"

namespace am {
  class CAmControlSenderMurphy;
}

namespace mctl
{
  class MurphyInterface;

  class MurphyInterface
  {
  public:
    static void setSocketHandler(am::CAmSocketHandler *socketHandler);
    static MurphyInterface *getMurphyInterface();
    void setControlReceiver(am::IAmControlReceive *receiver);
    void setControlSender(am::CAmControlSenderMurphy *sender);

    virtual ~MurphyInterface();

  private:
    MurphyInterface();

    void CreateMainloop();
    void CreateDomainController();
    void DestroyMainloop();
    void DestroyDomainController();
    bool RegisterDomainFunctions();

    static am::CAmSocketHandler *mSocketHandler;
    static MurphyInterface *mMurphyInterface;
    am::IAmControlReceive *mControlReceiver;
    am::CAmControlSenderMurphy *mControlSender;
    mrp_mainloop_t *mMainloop;
    mrp_domctl_t *mDomCtl;

    static void DomainConnectionCB(mrp_domctl_t *ctl, int connected, int errcode,
                                   const char *errmsg, void *user_data);
    static void DataImportCB(mrp_domctl_t *ctl, mrp_domctl_data_t *tables,
                             int ntable, void *user_data);
    static void DataExportCB(mrp_domctl_t *ctl, int errcode, const char *errmsg,
                             void *user_data);

  public:
    void ScheduleSinkAndSourceUpdate();
    void CancelSinkAndSourceUpdate();
    void ExportSinksAndSources();
    bool ConnectSourceToSink(am::am_sourceID_t source, am::am_sinkID_t sink,
                             uint32_t rset, am::am_mainConnectionID_t & conn);
    void ExpectSinkAndSourceChanges(bool more);

  private:
    mrp_timer_t *mSsuTimer;
    bool         mSsuExpectMore;

  public:
    bool RequestRoute(MainConnection *conn, std::vector<am::am_Route_s> routes);
bool NotifyDisconnect(MainConnection *conn);

    void ProcessRouteRequestReply(MainConnection *conn,
                                  uint16_t *path, size_t len);

  private:
    mrp_domctl_data_t *mTables;
    int                mNtable;

  public:
    enum Decision {
      DECISION_TEARDOWN,
      DECISION_NOOP,
      DECISION_DISCONNECT,
      DECISION_CONNECT,
      DECISION_SUSPEND
    };


    void SetDecisions(mrp_domctl_data_t *tables, int ntable);
    void ClearDecisions();

    Decision Verdict(MainConnection *conn);
  };
}

#endif /* MURPHYINTERFACE_H_ */
