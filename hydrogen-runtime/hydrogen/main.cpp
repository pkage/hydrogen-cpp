//
//  main.cpp
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include "errors.h"
#include "errorstack.h"
#include "fextractor.h"
#include "directivebundle.h"
#include "reference.h"
#include "value.h"
#include "context.h"
#include "directiveprocessor.h"
#include "dpspooler.h"
#include "functiondict.h"
#include "dpspooler_req.h"
#include "nativeprovider.h"

#include "interpreter.h"

string getworkingdir() {
    char cwd_tmp[256];
    getcwd(cwd_tmp, 256);
    string cwd(cwd_tmp);
    return cwd;
}

int main(int argc, const char * argv[]) {
    //cout << "running at" << getworkingdir() << "\n";

    if (argc != 2) {
        cout << "no file to run!\n";
        return -1;
    }
    string fn = argv[1];

    hyd::Interpreter interpreter;
    interpreter.loadFromFile(fn);
    interpreter.setBinaryLocation("/usr/local/hydrogen/lib");
    while (!interpreter.done()) {
        if (interpreter.hasErrors()) {
            break;
        }
        interpreter.tick();
    }
    cout << interpreter.getErrorReport();


    return 0;
}
