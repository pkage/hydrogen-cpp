//
//  nativeprovider.h
//  hydrogen
//
//  Created by Patrick Kage on 4/16/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_nativeprovider_h
#define hydrogen_nativeprovider_h
#include <map>
#include <dlfcn.h>
#include <limits>

namespace hyd {
    namespace lib {

        typedef mem::Value (*native)(vector<mem::Value>, err::ErrorStack*);
        typedef vector< pair<string, string> > (*native_descriptor)();

        class NativeProvider {
        private:
            vector<pair<string, vector<pair<string, string> > > > funcs;
            vector<pair<string, string> > fdict;
            err::ErrorStack *es;
            string loadLocation;

            struct ll_d {
                ll_d *next;
                bool hasNext = false;
                bool top = false;
                string pkg;
                string name;
                string refname;
            };
            ll_d ll_head;
            ll_d* getLast() {
                ll_d* head = &ll_head;
                while (head->hasNext) {
                    head = head->next;
                }
                return head;
            }
            void appendToLast(string pkg, string name, string refname) {
                ll_d* last = getLast();

                last->next = new ll_d;
                last->hasNext = true;
                last = last->next;

                last->pkg = pkg;
                last->name = name;
                last->refname = refname;
            }
            int getLinkedListLength() {
                ll_d* head = &ll_head;
                int c = 0;
                while (head->hasNext) {
                    c++;
                    head = head->next;
                }
                return c;
            }
        public:

            NativeProvider () {
                ll_head.hasNext = false;
                ll_head.top = true;
            }

            void setErrorStack(err::ErrorStack &es) {
                this->es = &es;
            }

            void setLoadLocation(string ll) {
                loadLocation = ll;
            }

            string getLoadLocation() {
                return loadLocation;
            }

            bool load(string name) {
                ll_head.top = true;
                string orig = name; name = ((loadLocation.back() == '/') ? loadLocation : loadLocation + "/") + name + ".hylib";

                // get the handle
                dlerror();
                void* handle = dlopen(name.c_str(), RTLD_LAZY);
                if (!handle) {
                    es->add(err::Error("NativeProvider", "load", string(dlerror())));
                    return false;
                }

                // get the manifest
                native_descriptor sdesc = (native_descriptor) dlsym(handle, "getinfo");
                if (!sdesc) {
                    es->add(err::Error("NativeProvider", "load", string(dlerror())));
                    return false;
                }
                vector< pair<string, string> > desc = sdesc();

                for (int c = 0; c < desc.size(); c++) {
                    pair<string, string> pr = desc.at(c);
                    appendToLast(orig, pr.first, pr.second);
                }
                dlclose(handle);
                return true;
            }

            void dump() {
                cout << "\n***** NativeProvider.dump() *****\n";
                cout << "Functions loaded: " << getLinkedListLength() << " (MAX_INT: " << numeric_limits<unsigned long int>::max() << ") \n";
                ll_d* head = &ll_head;
                cout << "Loaded functions:\n";
                while (head->hasNext) {
                    head = head->next;
                    cout << "\t@" << head->pkg << "." << head->name << "[" << head->refname << "]\n";

                }
                cout << "***** End NativeProvider.dump() *****\n";
            }

            bool hasPkg(string pkg) {
                ll_d* head = &ll_head;
                while (head->hasNext) {
                    head = head->next;
                    if (head->pkg == pkg) {
                        return true;
                    }
                }
                return false;
            }

            bool hasFunc(string pkg, string func) {
                if (!hasPkg(pkg)) {
                    return false;
                }

                ll_d* head = &ll_head;
                while (head->hasNext) {
                    head = head->next;
                    if (head->pkg == pkg && head->name == func) {
                        return true;
                    }
                }
                return false;

            }

            mem::Value call(string pkg, string name, vector<mem::Value> args) {
                if (!hasFunc(pkg, name)) {
                    es->add(err::Error("NativeProvider", "call", "No such function \"@" + pkg + "." + name + "\"!"));
                    return false;
                }

                string rname;
                ll_d* head = &ll_head;
                while (head->hasNext) {
                    head = head->next;
                    if (head->pkg == pkg && head->name == name) {
                        rname = head->refname;
                    }
                }

                dlerror();
                pkg = ((loadLocation.back() == '/') ? loadLocation : loadLocation + "/") + pkg + ".hylib";
                void* handle = dlopen(pkg.c_str(), RTLD_LAZY);
                if (!handle) {
                    es->add(err::Error("NativeProvider", "call", string(dlerror())));
                    return false;
                }

                dlerror();
                native fn = (native) dlsym(handle, rname.c_str());
                if (!fn) {
                    es->add(err::Error("NativeProvider", "call", "Function \"@" + pkg + "." + name + "\" is in manifest, but doesn't exist!"));
                    return false;
                }
                
                return fn(args, es);
            }



        };


    }
}

#endif
