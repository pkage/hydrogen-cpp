//
//  errorstack.h
//  hydrogen
//
//  Created by Patrick Kage on 4/13/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_errorstack_h
#define hydrogen_errorstack_h
using namespace std;
#include <queue>

namespace hyd {
    namespace err {
        class Error {
        private:
            string reason, suborigin, origin;
        public:
            Error(string origin, string suborigin, string reason) {
                set(origin, suborigin, reason);
            }
            void set(string origin, string suborigin, string reason) {
                this->reason = reason;
                this->suborigin = suborigin;
                this->origin = origin;
            }
            string getOrigin() {
                return origin;
            }
            string getSubOrigin() {
                return suborigin;
            }
            string getReason() {
                return reason;
            }
        };

        class ErrorStack {
        private:
            queue<Error> internal;
        public:
            void add(Error err) {
                internal.push(err);
            }
            string pop() {
                string log;
                while (internal.size() > 0) {
                    Error tmp = internal.back();
                    log += tmp.getOrigin() + "." + tmp.getSubOrigin() + ": " + tmp.getReason() + "\n";
                    internal.pop();
                }
                return log;
            }
            bool hasError() {
                return (internal.size() > 0);
            }
        };
    }
}

#endif
