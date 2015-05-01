//
//  directivebundle.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_directivebundle_h
#define hydrogen_directivebundle_h
using namespace std;
#include "directive.h"
#include "errors.h"

namespace hyd {
    namespace dir {
        class DirectiveBundle {
        private:
            vector<Directive> vec;
        public:
            bool valid(int index) {
                return (index >= 0 && index < vec.size());
            }
            Directive& get(int index) {
                if (valid(index)) {
                    return vec[index];
                }
                throw err::overflowError;
            }
            int length() {
                return (int)vec.size();
            }
            void clear() {
                vec.clear();
            }
            void append(Directive dir) {
                vec.push_back(dir);
            }
            bool replace(int index, Directive dir) {
                if (valid(index)) {
                    vec[index] = dir;
                    return true;
                }
                return false;
            }
            void classifyAll() {
                for (int c = 0; c < vec.size(); c++) {
                    vec[c].classify();
                }
                
            }
            void initialize(storage::LineBundle &lb) {
                clear();
                for (int c = 0; c < lb.length(); c++) {
                    append(Directive(lb.get(c)));
                    get(c).classify();
                }
            }
        };
    }
}

#endif
