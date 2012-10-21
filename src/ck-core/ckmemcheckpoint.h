#ifndef _CK_MEM_CHECKPT_
#define _CK_MEM_CHECKPT_

#include "CkMemCheckpoint.decl.h"

extern CkGroupID ckCheckPTGroupID;

class CkArrayCheckPTReqMessage: public CMessage_CkArrayCheckPTReqMessage {
public: 
  CkArrayCheckPTReqMessage()  {}
};

class CkArrayCheckPTMessage: public CMessage_CkArrayCheckPTMessage {
public:
	CkArrayID  aid;
	CkGroupID  locMgr;
	CkArrayIndex index;
	double *packData;
	int bud1, bud2;
	int len;
	int cp_flag;          // 1: from checkpoint 0: from recover
};

class CkCheckPTMessage: public CMessage_CkArrayCheckPTMessage {
public:
	double *packData;
	int bud1, bud2;
	int len;
	int cp_flag;          // 1: from checkpoint 0: from recover
};

class CkProcCheckPTMessage: public CMessage_CkProcCheckPTMessage {
public:
	int pe;
	int reportPe;		// chkpt starter
	int failedpe;
	int cur_restart_phase;
	int len;
	char *packData;
};

// table entry base class
class CkCheckPTInfo {
   friend class CkMemCheckPT;
protected:
   CkArrayID aid;
   CkGroupID locMgr;
   CkArrayIndex index;
   int pNo;   //another buddy
public:
   CkCheckPTInfo();
   CkCheckPTInfo(CkArrayID a, CkGroupID loc, CkArrayIndex idx, int pno):
                  aid(a), locMgr(loc), index(idx), pNo(pno)   {}
   virtual ~CkCheckPTInfo() {}
   virtual void updateBuffer(CkArrayCheckPTMessage *data) = 0;
   virtual CkArrayCheckPTMessage * getCopy() = 0;
   virtual void updateBuddy(int b1, int b2) = 0;
   virtual int getSize() = 0;
};

/// memory or disk checkpointing
#define CkCheckPoint_inMEM   1
#define CkCheckPoint_inDISK  2

class CkMemCheckPT: public CBase_CkMemCheckPT {
public:
  CkMemCheckPT(int w);
  CkMemCheckPT(CkMigrateMessage *m):CBase_CkMemCheckPT(m) {};
  virtual ~CkMemCheckPT();
  void pup(PUP::er& p);
  inline int BuddyPE(int pe);
  void doItNow(int sp, CkCallback &);
  void restart(int diePe);
  void removeArrayElements();
  void createEntry(CkArrayID aid, CkGroupID loc, CkArrayIndex index, int buddy);
  void recvData(CkArrayCheckPTMessage *);
  void gotData();
  void recvProcData(CkProcCheckPTMessage *);
  void cpFinish();
  void syncFiles(CkReductionMsg *);
  void report();
  void recoverBuddies();
  void recoverEntry(CkArrayCheckPTMessage *msg);
  void recoverArrayElements();
  void quiescence(CkCallback &);
  void resetReductionMgr();
  void finishUp();
  void gotReply();
  void inmem_restore(CkArrayCheckPTMessage *m);
  void updateLocations(int n, CkGroupID *g, CkArrayIndex *idx,int nowOnPe);
  void resetLB(int diepe);
  int  isFailed(int pe);
  void pupAllElements(PUP::er &p);
  void startArrayCheckpoint();
  void recvArrayCheckpoint(CkArrayCheckPTMessage *m);
  void recoverAll(CkArrayCheckPTMessage * msg, CkVec<CkGroupID> * gmap, CkVec<CkArrayIndex> * imap);
public:
  static CkCallback  cpCallback;

  static int inRestarting;
  static int inLoadbalancing;
  static double startTime;
  static char*  stage;
private:
  CkVec<CkCheckPTInfo *> ckTable;
  CkArrayCheckPTMessage * chkpTable[2];

  int recvCount, peCount;
  int expectCount, ackCount;
    /// the processor who initiate the checkpointing
  int cpStarter;
  CkVec<int> failedPes;
  int thisFailedPe;

    /// to use memory or disk checkpointing
  int    where;
private:
  inline int isMaster(int pe);

  void failed(int pe);
  int  totalFailed();

  void sendProcData();
};

// called in initCharm
void CkMemRestart(const char *, CkArgMsg *);

// called by user applications
// to start a checkpointing
void CkStartMemCheckpoint(CkCallback &cb);

// true if inside a restarting phase
extern "C" int CkInRestarting(); 
extern "C" int CkInLdb(); 
extern "C" void CkSetInLdb(); 
extern "C" void CkResetInLdb();

extern "C" int CkHasCheckpoints();

extern "C" void CkDieNow();

#endif
