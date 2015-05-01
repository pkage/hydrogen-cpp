//
//  dpspooler_req.h
//  hydrogen
//
//  Created by Patrick Kage on 4/15/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_dpspooler_req_h
#define hydrogen_dpspooler_req_h
using namespace std;
#include <vector>
#include "types.h"

namespace hyd {
    namespace dir {
        class DPSpooler_req {
        private:
            string name;
            vector<mem::Value> args;
            int futureReturnType;
        public:
            void setName(string name) {
                this->name = name;
            }
            void addArg(mem::Value val) {
                args.push_back(val);
            }

            vector<mem::Value> getVector() {
                return args;
            }
            string getName() {
                return name;
            }

            void setFutureReturnType(int type) {
                futureReturnType = type;
            }

            int getFutureReturnType() {
                return futureReturnType;
            }
            void clear() {
                futureReturnType = mem::types::UNK;
                args.clear();
                name = "";
            }
        };
    }
}

#endif
