//
//  context.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_context_h
#define hydrogen_context_h
using namespace std;
#include <map>

namespace hyd {
    namespace mem {
        class Context {
        private:
            map<string, Value> internal;
        public:
            bool exists(Reference ref) {
                for (auto key : internal) {
                    if (ref.name == key.first) {
                        return true;
                    }
                }
                return false;
            }
            
            Value resolve(Reference ref) {
                for (auto key : internal) {
                    if (ref.name == key.first) {
                        return key.second;
                    }
                }
                return null_value;
            }
            
            
            void set(Reference ref, Value val) {
                internal[ref.name] = val;
            }
            
            
            void integrate(Context cxt) {
                for (auto key : cxt.internal) {
                    set(key.first, key.second);
                }
            }
        };
    }
}

#endif
