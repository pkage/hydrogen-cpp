//
//  linebundle.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_linebundle_h
#define hydrogen_linebundle_h
using namespace std;

#include <vector>

namespace hyd {
    namespace storage {
        class LineBundle {
        private:
            vector<string> vec;
        public:
            bool valid(int index) {
                return (index >= 0 && index < vec.size());
            }
            string get(int index) {
                if (valid(index)) {
                    return vec[index];
                }
                return "";
            }
            int length() {
                return (int)vec.size();
            }
            void clear() {
                vec.clear();
            }
            void append(string str) {
                vec.push_back(str);
            }
            bool replace(int index, string str) {
                if (valid(index)) {
                    vec[index] = str;
                    return true;
                }
                return false;
            }
        };
    }
}

#endif
