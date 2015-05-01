//
//  interpreter.h
//  hydrogen
//
//  Created by Patrick Kage on 4/22/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_interpreter_h
#define hydrogen_interpreter_h
using namespace std;
#include "linebundle.h"
#include "fextractor.h"
#include "directivebundle.h"
#include "errorstack.h"
#include "dpspooler.h"

namespace hyd {

    class Interpreter {
    private:
        dir::DirectiveBundle db;
        dir::DPSpooler dps;
        dir::FunctionDictionary fd;
        lib::NativeProvider np;
        err::ErrorStack es;

    public:
        Interpreter() {
            initialize();
        }
        void initialize() {
            np.setErrorStack(es);
        }

        void setBinaryLocation(string loc) {
            np.setLoadLocation(loc);
        }

        void loadFromFile(string fn) {
            storage::LineBundle lb;
            lb = fe::preprocess(fn);
            db.initialize(lb);
            fd.extractFunctions(db);

            dps.setErrorStack(es);
            dps.setNativeProvider(np);
            dps.setFunctionDictionary(fd);

            dps.init(db);
        }

        void tick() {
            if (!dps.done()) {
                dps.tick();
            }
        }

        bool done() {
            return dps.done();
        }

        bool hasErrors() {
            return es.hasError();
        }

        string getErrorReport() {
            return es.pop();
        }
    };
}


#endif
