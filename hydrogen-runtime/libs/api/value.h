//
//  value.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_value_h
#define hydrogen_value_h
using namespace std;
#include "types.h"
#include <sstream>
#include "comparationresults.h"

namespace hyd {
    namespace mem {
        class Value {
        private:
            int type;
            string sval;
            float nval;
        public:
            Value() {
                type = types::UNK;
            }
            Value(string s) {
                set(s);
            }
            Value(float f) {
                set(f);
            }
            Value(int type) {
                this->type = type;
            }
            
            
            void set(string sval) {
                this->type = types::STRING;
                this->sval = sval;
            }
            void set(float nval) {
                this->type = types::NUMBER;
                this->nval = nval;
            }
            int getType() {
                return type;
            }
            string gets() {
                return sval;
            }
            float getn() {
                return nval;
            }
            
            string toString() {
                if (type == types::STRING) {
                    return sval;
                } else if (type == types::NUMBER) {
                    stringstream ss; ss << nval;
                    return ss.str();
                } else {
                    return "";
                }
            }
            
            int compare(Value v) {
                if (v.getType() != getType()) {
                    return cr::INVALID;
                }
                if (getType() == types::STRING) {
                    return ((v.gets() == gets()) ? cr::EQUALTO : cr::NOTEQUALTO);
                } else if (getType() == types::NUMBER) {
                    if (getn() > v.getn()) {
                        return cr::GREATERTHAN;
                    } else if (getn() < v.getn()) {
                        return cr::LESSTHAN;
                    } else if (getn() == v.getn()) {
                        return cr::EQUALTO;
                    }
                }
                return cr::INVALID;
            }
            
        };
        
        Value null_value;
    }
}

#endif
