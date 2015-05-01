//
//  reference.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_reference_h
#define hydrogen_reference_h
using namespace std;

namespace hyd {
    namespace mem {
        class Reference {
        public:
            string name;
            Reference(string s) {
                name = s;
            }
        };
    }
}

#endif
