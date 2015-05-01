//
//  dpspooler.h
//  hydrogen
//
//  Created by Patrick Kage on 4/15/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_dpspooler_h
#define hydrogen_dpspooler_h
using namespace std;
#include <stack>
#include "functiondict.h"
#include "dpspooler_req.h"
#include "nativeprovider.h"
#include <sstream>

namespace hyd {
    namespace dir {
        class DPSpooler {
        private:
            stack<DirectiveProcessor> spool;
            FunctionDictionary* fd;
            lib::NativeProvider* np;
            err::ErrorStack* es;
            string itos(int i) {stringstream ss; ss << i; return ss.str();};
            DPSpooler_req dps_r;
        public:
            void setErrorStack(err::ErrorStack &es) {
                this->es = &es;

            }

            void setNativeProvider(lib::NativeProvider &np) {
                this->np = &np;
                this->np->setErrorStack(*this->es);
            }

            DPSpooler() {

            }

            DPSpooler(DirectiveBundle db) {
                init(db);
            }

            void init(DirectiveBundle db) {
                spool.push(DirectiveProcessor(db));
                spool.top().setErrorStack(*es);
                spool.top().setDPSpoolerReq(dps_r);
                spool.top().setNativeProvider(*np);
            }

            void setFunctionDictionary(FunctionDictionary& fd) {
                this->fd = &fd;
            }

            DirectiveProcessor getCurrent() {
                return spool.top();
            }

            void tick() {
                spool.top().tick();
                if (spool.top().getFnCallStatus()) {
                    spool.top().setFnCallStatus(false);
                    //cout << "DBG: Adding a dp to the spool\n";
                    if (!fd->exists(dps_r.getName())) {
                        es->add(err::Error("DPSpooler", "tick", "No function called \"" + dps_r.getName() + "\"!"));
                        return;
                    }
                    spool.push(DirectiveProcessor());
                    spool.top().setBundle(fd->resolve(dps_r.getName()));
                    spool.top().setErrorStack(*es);
                    spool.top().setNativeProvider(*np);
                    spool.top().setDPSpoolerReq(dps_r);
                    for (int c = 0; c < dps_r.getVector().size(); c++) {
                        spool.top().setVariable(mem::Reference("$" + itos(c + 1)), dps_r.getVector().at(c));
                    }
                    spool.top().forceHeadAdvance(1);
                    dps_r.clear();

                }
                if (spool.top().done()) {
                    //cout << "DBG: Popping a dp off the spool\n";
                    bool flag = spool.top().hasExport();
                    mem::Value exp;
                    if (flag) exp = spool.top().getExport();
                    spool.pop();
                    if (flag && !spool.empty()) spool.top().setVariable(mem::Reference("$export"), exp);
                }
            }


            bool done() {
                return (spool.size() == 0);
            }


        };



    }
}

#endif
