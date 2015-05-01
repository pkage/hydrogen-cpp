//
//  fextractor.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_fextractor_h
#define hydrogen_fextractor_h
using namespace std;
#include <fstream>
#include "linebundle.h"
#include "errors.h"
#include "directive.h"

namespace hyd {
    namespace fe {
        storage::LineBundle extractFromFile(string filename) {
            storage::LineBundle lb;
            ifstream ifs;
            ifs.open(filename.c_str());
            if (!ifs) {
                throw err::fileNotFound;
            }
            string ln;
            while (!ifs.eof()) {
                getline(ifs, ln);
                lb.append(ln);
            }
            ifs.close();
            return lb;
        }

        bool has_inc(storage::LineBundle lb) {
            bool has_inc = false;
            for (int c = 0; c < lb.length(); c++) {
                string tmp = lb.get(c);
                if (tmp.length() > 7 && tmp.substr(0, 7) == "include") {
                    has_inc = true;
                }
            }
            return has_inc;
        }
        storage::LineBundle replaceLineWithBundle(storage::LineBundle orig, storage::LineBundle insert, int index) {
            storage::LineBundle ret;
            for (int c = 0; c < orig.length(); c++) {
                if (c == index) {
                    for (int d = 0; d < insert.length(); d++) {
                        ret.append(insert.get(d));
                    }
                } else {
                    ret.append(orig.get(c));
                }
            }
            return ret;
        }

        storage::LineBundle preprocess(string filename) {
            storage::LineBundle lb = extractFromFile(filename);

            while (has_inc(lb)) {
                storage::LineBundle lb_tmp;
                for (int c = 0; c < lb.length(); c++) {
                    string tmp = lb.get(c);

                    if (tmp.length() > 7 && tmp.substr(0, 7) == "include") {
                        storage::LineBundle imported;
                        try {
                        imported = extractFromFile(tmp.substr(8, tmp.length() - 1));
                        } catch (err::NativeException &e) {
                            cerr << e.what();
                            return lb;
                        }
                        for (int d = 0; d < imported.length(); d++) {
                            lb_tmp.append(imported.get(d));
                        }
                    } else {
                        lb_tmp.append(tmp);
                    }
                }
                lb = lb_tmp;
            }

            return lb;

        }


    }
}

#endif
