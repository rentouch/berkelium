/*  Berkelium Implementation
 *  Root.hpp
 *
 *  Copyright (c) 2009, Patrick Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "base/mac/scoped_nsautorelease_pool.h"
#include "berkelium/Platform.hpp"
#include "berkelium/Berkelium.hpp"
#include "Singleton.hpp"
#include "chrome/browser/profiles/profile.h"
#include "content/common/notification_service.h"
#include "base/memory/ref_counted.h"
#include "base/message_loop.h"
#include "base/memory/scoped_ptr.h"
#include "content/browser/browser_thread.h"

class BrowserRenderProcessHost;
class ProcessSingleton;
class URLRequestContext;
namespace base {
    class SystemMonitor;
}
namespace net {
class URLRequestContextGetter;
}
class HighResolutionTimerManager;
class HistogramSynchronizer;
class ScopedTempDir;
class StatisticsRecorder;
namespace chrome_browser_net {
class PredictorInit;
}
namespace Berkelium {

class MemoryRenderViewHostFactory;
class ErrorDelegate;

class BerkeliumMainDelegate;

//singleton class that contains chromium singletons. Not visible outside of Berkelium library core
class Root : public Singleton<Root> {
    scoped_ptr<BerkeliumMainDelegate> mBerkeliumMainDelegate;
    scoped_ptr<MemoryRenderViewHostFactory> mRenderViewHostFactory;

public:
    Root();
    ~Root();

    bool init(FileString homeDirectory, FileString subprocessDirectory);

    bool initialized() {
        // TODO(ewencp) figure out where to check for initialization
        return true;
        // return mProf ? true : false;
    }

    void runUntilStopped();
    void stopRunning();
    void update();

    inline Profile* getProfile() {
        // TODO(ewencp) figure out where to get Profile* from.
        return NULL;
        //return mProf;
    }

    static void SetUpGLibLogHandler();

    net::URLRequestContextGetter *getDefaultRequestContext() {
        // TODO(ewencp) figure out where to get net::URLRequestContextGetter* from.
        return NULL;
        //return mDefaultRequestContext;
    }
};

}
