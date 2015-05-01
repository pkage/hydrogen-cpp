//
//  functiondict.h
//  hydrogen
//
//  Created by Patrick Kage on 4/15/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_functiondict_h
#define hydrogen_functiondict_h
#include <map>

namespace hyd {
    namespace dir {
        class FunctionDictionary {
        private:
            map<string, DirectiveBundle> internal;
        public:
            bool exists(string ref) {
                for (auto key : internal) {
                    if (ref == key.first) {
                        return true;
                    }
                }
                return false;
            }
            
            DirectiveBundle resolve(string ref) {
                for (auto key : internal) {
                    if (ref == key.first) {
                        return key.second;
                    }
                }
                return DirectiveBundle();
            }
            
            // DEBUG
            void dump() {
                cout << "Elements in map: " << internal.size() << "\n";
                for (auto key : internal) {
                    cout << key.first << ":\n";
                    for (int c = 0; c < key.second.length(); c++) {
                        cout << "\t" << key.second.get(c).getOriginal() << "\n";
                    }
                }
            }
            
            
            void set(string ref, DirectiveBundle val) {
                internal[ref] = val;
            }
            
            void extractFunctions(DirectiveBundle db) {
                bool reading = false;
                string readingname = "";
                DirectiveBundle tmp;
                for (int c = 0; c < db.length(); c++) {
                    if (db.get(c).getHint() == dhints::FUNCTIONDEF) {
                        reading = true;
                        readingname = db.get(c).getPiece(1);
                    }
                    if (reading) {
                        tmp.append(db.get(c));
                    }
                    if (db.get(c).getHint() == dhints::ENDFUNCTIONDEF && reading) {
                        reading = false;
                        tmp.classifyAll();
                        internal[readingname] = tmp;
                        tmp.clear();
                    }
                    
                }
                
            }
            
        };
    }
}

#endif
