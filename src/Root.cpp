/*  Berkelium Implementation
 *  Root.cpp
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

// Berkelium headers
#include "berkelium/Berkelium.hpp"
#include "Root.hpp"
#include "MemoryRenderViewHost.hpp"

#include "chrome_main.h"

BERKELIUM_SINGLETON_INSTANCE(Berkelium::Root);
namespace Berkelium {

class BerkeliumMainDelegate : public BerkeliumChromeMain::ChromeMainDelegate {
public:
    BerkeliumMainDelegate(FileString homedir, FileString subprocessdir)
     : homeDirectory(homedir),
       subprocessDirectory(subprocessdir)
    {}

    virtual void PreParseCommandLine(int* pargc, char*** pargv) {
        FilePath subprocess;
        {
            // From <base/command_line.h>:
            // Initialize the current process CommandLine singleton.  On Windows,
            // ignores its arguments (we instead parse GetCommandLineW()
            // directly) because we don't trust the CRT's parsing of the command
            // line, but it still must be called to set up the command line.
            // This means that on windows, we have to call ::Init with whatever we feel
            // like (since this is the only way to create the static CommandLine instance),
            // and then we have manually call ParseFromString.
            // (InitFromArgv does not exist on OS_WIN!)
#if defined(OS_WIN)
            FilePath module_dir;
            if (subprocessDirectory.size()) {
                module_dir = FilePath(subprocessDirectory.get<FilePath::StringType>());
            } else {
                PathService::Get(base::DIR_MODULE, &module_dir);
            }
#ifdef _DEBUG
            subprocess = module_dir.Append(L"berkelium_d.exe");
#else
            subprocess = module_dir.Append(L"berkelium.exe");
#endif

            // Sandbox process doesn't work properly right now -- can't
            // customize path so it tries to use the same binary
            // (/proc/self/exe) rather than the berkelium binary
            std::wstring subprocess_str = L"berkelium --no-sandbox --browser-subprocess-path=";
            subprocess_str += L"\"";
            subprocess_str += subprocess.value();
            subprocess_str += L"\"";
            // TODO(ewencp) override parameters for windows version?
#elif defined(OS_MACOSX) || defined(OS_POSIX)
            FilePath app_contents;
            PathService::Get(base::FILE_EXE, &app_contents);
            FilePath module_dir;
            if (subprocessDirectory.size()) {
                module_dir = FilePath(subprocessDirectory.get<FilePath::StringType>());
            } else {
                module_dir = app_contents.DirName();
            }
            subprocess = module_dir.Append("berkelium");
            std::string subprocess_str = "--browser-subprocess-path=";
            subprocess_str += subprocess.value();

            // Construct new args. The data here will end up leaking. We could
            // try to clean it up, but it's not really worth it.
            argv.push_back("berkelium");
            // Sandbox process doesn't work properly right now -- can't
            // customize path so it tries to use the same binary
            // (/proc/self/exe) rather than the berkelium binary
            argv.push_back("--no-sandbox");
            argv.push_back( (new std::string(subprocess_str))->c_str() );

            // Some helper debugging options you can switch on
#if 0
            // Single process mode -- no forking
            argv.push_back("--single-process");
#elif 0
            // Catch renderer processes in a debugger
            argv.push_back("--disable-seccomp-sandbox");
            argv.push_back("--renderer-cmd-prefix='xterm -title renderer -e gdb --args'");
#endif

            // Provide NULL but don't include it in the length since some code
            // relies on the NULL terminator
            argv.push_back(NULL);
            *pargc = argv.size()-1;
            *pargv = (char**) &(argv[0]);
#endif

        }
    }
private:
    FileString homeDirectory;
    FileString subprocessDirectory;

    // We store this so it lives past the initial construction in
    // PreParseCommandLine
    std::vector<const char*> argv;
}; // class BerkeliumMainDelegate

Root::Root () {
}

bool Root::init(FileString homeDirectory, FileString subprocessDirectory) {
    mBerkeliumMainDelegate.reset(
        new BerkeliumMainDelegate(homeDirectory, subprocessDirectory)
    );

#if defined(OS_WIN)
    content::ContentMain(instance, sandbox_info, mBerkeliumMainDelegate.get());
#elif defined(OS_POSIX) || defined(OS_MACOSX)
    // Bogus args just to get things moving. They get reset in .
    const char* argv[] = { "berkelium", NULL };
    content::ContentMain(arraysize(argv)-1, (char**)argv, mBerkeliumMainDelegate.get());
#endif

    return true;
}

void Root::runUntilStopped() {
    MessageLoopForUI::current()->MessageLoop::Run();
}

void Root::stopRunning() {
    MessageLoopForUI::current()->Quit();
}

void Root::update() {
    MessageLoopForUI::current()->RunAllPending();
}

Root::~Root(){
    mRenderViewHostFactory.reset();
    mBerkeliumMainDelegate.reset();
}


}
