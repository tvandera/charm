/*
Threaded Charm++ "Framework Framework" 

This is the interface used by library writers--
people who use TCharm to build their own library.

Orion Sky Lawlor, olawlor@acm.org, 7/17/2002
*/
#ifndef __CHARM_TCHARMLIB_H
#define __CHARM_TCHARMLIB_H

#include "tcharm_impl.h"

/*
Library "fallback setup" routine.
From an initcall, you register one of your routines 
here in case the user doesn't write a TCharmUserSetup
routine.  Your fallback version sets up (only) your 
library by creating some TCharm threads and attaching to them,
like:

void myLibFallbackSetupFn(void) {
	TCharmCreate(TCharmGetNumChunks(),myLibUserStart);
        myLib_Attach();
}
*/
typedef void (*TCharmFallbackSetupFn)(void);
void TCharmSetFallbackSetup(TCharmFallbackSetupFn f);


/*
These two routines need to be called from your Attach routine.
The "start" call finds the last-created set of tcharm threads,
copies their arrayID into retTCharmArray, and returns an 
opts object bound the the tcharm threads.  You pass this opts
object to your array's ckNew.
The "finish" call registers your newly-created array as a
client of this set of TCharm threads.
*/
CkArrayOptions TCharmAttachStart(CkArrayID *retTCharmArray,int *retNumElts=0);
void TCharmAttachFinish(const CkArrayID &libraryArray);


/*
A simple client array that can be bound to a tcharm array.
You write a TCharm library by inheriting your array from this class.
You'll have to pass the TCharm arrayID to our constructor, and
then call tcharmClientInit().
As soon as your startup work is done, you MUST call "thread->ready();"
*/
class TCharmClient1D : public ArrayElement1D {
  CProxy_TCharm threadProxy; //Proxy for our bound TCharm array
 protected:
  /* This is the actual TCharm object that manages our thread.
     You use this like "thread->suspend();", "thread->resume();",
     etc.  
   */
  TCharm *thread; 
  
  //Clients need to override this function to set their
  // thread-private variables.  You usually use something like:
  //  CtvAccessOther(forThread,_myFooPtr)=this;
  virtual void setupThreadPrivate(CthThread forThread) =0;
  
 public:
  TCharmClient1D(const CkArrayID &threadArrayID) 
    :threadProxy(threadArrayID)
  {
    //Argh!  Can't call setupThreadPrivate yet, because
    // virtual functions don't work within constructors!
    thread=NULL;
  }
  TCharmClient1D(CkMigrateMessage *m) //Migration, etc. constructor
  {
    thread=NULL;
  }
  
  //You MUST call this from your constructor:
  inline void tcharmClientInit(void) {
    thread=threadProxy[thisIndex].ckLocal();  
    if (thread==NULL) CkAbort("FEM can't locate its thread!\n");
    setupThreadPrivate(thread->getThread());
  }
  
  virtual void ckJustMigrated(void);
  virtual void pup(PUP::er &p);
};


/*
Library API Calls.  The pattern:
  TCHARM_API_TRACE("myRoutineName","myLibName");
MUST be put at the start of every
user-callable library routine.  The string name is
used for debugging printouts, and eventually for
tracing (once tracing is generalized).
*/
#ifndef CMK_OPTIMIZE
#  define TCHARM_API_TRACE(routineName,libraryName) \
	TCharmAPIRoutine apiRoutineSentry;\
	TCharmApiTrace(routineName,libraryName)
#else
#  define TCHARM_API_TRACE(routineName,libraryName) \
	TCharmAPIRoutine apiRoutineSentry
#endif
void TCharmApiTrace(const char *routineName,const char *libraryName);


#endif /*def(thisHeader)*/
