//
//  errors.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_errors_h
#define hydrogen_errors_h
using namespace std;
#include <exception>

namespace hyd {
    namespace err {
        class NativeException : public exception {
            string reason;
            public:
            NativeException(string reason) {
                this->reason = reason;
            };
            virtual const char* what() {
                return reason.c_str();
            };
        };
        
        // preinstalled exceptions
        NativeException fileNotFound("File not found!");
        NativeException overflowError("Internal overflow!");
    }
}

#endif
