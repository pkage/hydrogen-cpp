//
//  directiveprocessor.h
//  hydrogen
//
//  Created by Patrick Kage on 4/13/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_directiveprocessor_h
#define hydrogen_directiveprocessor_h
#include "errorstack.h"
#include "context.h"
#include "dpspooler_req.h"
#include "nativeprovider.h"

namespace hyd {
    namespace dir {
        class DirectiveProcessor {
        private:
            mem::Context cx;
            DirectiveBundle db;
            err::ErrorStack *es;
            lib::NativeProvider *np;
            int head;
            bool needfncall, forcedone;
            DPSpooler_req* dps_r;
        public:
            DirectiveProcessor() {
                head = 0;
                needfncall = false;
                forcedone = false;
            }

            DirectiveProcessor(DirectiveBundle db) {
                setBundle(db);
                needfncall = false;
                forcedone = false;
                head = 0;
            }

            void setDPSpoolerReq(DPSpooler_req &dps_r) {
                this->dps_r = &dps_r;
            }

            void setNativeProvider(lib::NativeProvider &np) {
                this->np = &np;
            }

            void setErrorStack(err::ErrorStack &es) {
                this->es = &es;
            }

            bool getFnCallStatus() {
                return needfncall;
            }

            void setFnCallStatus(bool flag) {
                needfncall = flag;
            }


            void setBundle(DirectiveBundle db) {
                this->db = db;
                this->db.classifyAll();
            }

            int getHead() {
                return head;
            }

            mem::Value resolveToValue(string piece) {
                if (piece[0] == '$') {
                    if (cx.exists(mem::Reference(piece))) {
                        return cx.resolve(mem::Reference(piece));
                    } else {
                        es->add(err::Error("DirectiveProcessor", "resolveToValue", "Unregistered variable \"" + piece + "\"!"));
                        return mem::Value();
                    }
                }
                if (piece[0] == '\"') {
                    string stripped = piece.substr(1, piece.length() - 2);
                    string tmp = "";
                    for (int c = 0; c < stripped.length(); c++) {
                        if (stripped[c] == '\\' && c + 1 != stripped.length()) {
                            switch(stripped[c + 1]) {
                                case 'n':
                                    tmp += "\n";
                                    break;
                                case 'r':
                                    tmp += "\r";
                                    break;

                                case 't':
                                    tmp += "\t";
                                    break;

                                default:
                                    break;
                            }
                            c++;
                            continue;
                        } else {
                            tmp += stripped[c];
                        }
                    }
                    return mem::Value(tmp);
                }
                float fl;
                try {
                    fl = (float)atof(piece.c_str());
                    return mem::Value(fl);
                } catch(exception &e) {
                    es->add(err::Error("DirectiveProcessor", "resolveToValue", "Error resolving \"" + piece + "\" to a value!"));
                }
                return mem::Value();
            }

            bool hasExport() {
                return (cx.exists(mem::Reference("$export")));
            }

            mem::Value getExport() {
                if (cx.exists(mem::Reference("$export"))) {
                    return cx.resolve(mem::Reference("$export"));
                }
                return mem::Value();
            }

            void setVariable(mem::Reference ref, mem::Value val) {
                cx.set(ref, val);
            }

            bool done() {
                return (head == db.length() || forcedone);
            }

            void tick() {
                if (done()) {
                    return;
                }
                Directive cd = db.get(head);
                if (cd.getPiece(0) == "print") {
                    cout << resolveToValue(cd.getPiece(1)).toString();
                    head++;
                    return;
                }
                switch (cd.getHint()) {
                    case dhints::DECLAREVAR:
                        cx.set(mem::Reference(cd.getPiece(1)), mem::Value( ((cd.getPiece(3) == "string") ? mem::types::STRING : mem::types::NUMBER) ));
                        break;

                    case dhints::ASSIGNMENT:
                        resolveAssignment(cd);
                        break;

                    case dhints::COMPARATION:
                        if (!resolveComparison(cd)) {
                            advanceHeadToBlockClose();
                        }
                        break;

                    case dhints::INTERNALCALL:
                        needfncall = true;
                        dps_r->setName(cd.getPiece(0));
                        for (int c = 1; c < cd.numberOfPieces(); c++) {
                            dps_r->addArg(resolveToValue(cd.getPiece(c)));
                        }
                        break;

                    case dhints::FUNCTIONDEF:
                        advanceHeadToBlockClose();
                        break;

                    case dhints::IMPORT:
                        np->load(cd.getPiece(1));
                        break;

                    case dhints::EXTERNALCALL:
                        resolveExternal(cd);
                        break;

                    case dhints::GOTO:
                        jumpTo(searchForLabel(resolveToValue(cd.getPiece(1))));
                        break;

                    case dhints::LABEL:
                    case dhints::ENDCOMPARATION:
                    case dhints::ENDFUNCTIONDEF:
                    case dhints::COMMENT:
                    case dhints::EMPTY:
                        break;

                    case dhints::UNKNOWN:
                    default:
                        es->add(err::Error("DirectiveProcessor", "tick", "Unknown linetype \"" + cd.getOriginal() + "\""));
                        break;
                }
                head++;
            }

            void integrateContext(mem::Context &cx) {
                this->cx.integrate(cx);
            }




            int searchForLabel(mem::Value lbl) {
                for (int c = 0; c < db.length(); c++) {
                    if (db.get(c).getHint() == dhints::LABEL) {
                        if (resolveToValue(db.get(c).getPiece(1)).compare(lbl) == mem::cr::EQUALTO) {
                            return c;
                        }
                    }
                }
                es->add(err::Error("DirectiveProcessor", "searchForLabel", "No label with handle \"" + lbl.toString() + "\"!"));
                return -1;
            }

            void jumpTo(int pos) {
                if (pos < 0 || pos >= db.length()) {
                    return;
                }
                head = pos;
            }

            bool resolveComparison(Directive cd) {
                string op = cd.getPiece(2);
                mem::Value left = resolveToValue(cd.getPiece(1));
                mem::Value right = resolveToValue(cd.getPiece(3));

                int result = left.compare(right);
                if (result == mem::cr::INVALID) {
                    es->add(hyd::err::Error("DirectiveProcessor", "resolveComparison", "Type mismatch in comparison between \"" + cd.getPiece(1) + "\" and \"" + cd.getPiece(3)));
                    return false;
                }
                if (op == "==") {
                    if (result == mem::cr::EQUALTO) {
                        return true;
                    }
                } else if (op == "!=") {
                    if (result != mem::cr::EQUALTO) {
                        return true;
                    }
                } else if (op == ">") {
                    if (result == mem::cr::GREATERTHAN) {
                        return true;
                    }
                } else if (op == "<") {
                    if (result == mem::cr::LESSTHAN) {
                        return true;
                    }
                } else if (op == ">=") {
                    if (result == mem::cr::EQUALTO || result == mem::cr::GREATERTHAN) {
                        return true;
                    }
                } else if (op == "<=") {
                    if (result == mem::cr::EQUALTO || result == mem::cr::LESSTHAN) {
                        return true;
                    }
                }

                return false;
            }

            void advanceHeadToBlockClose() {
                //cout << "advancing head to block close after \"" << db.get(head).getOriginal() << "\"\n";
                int count = 1;
                int thead = head;
                while (count != 0) {
                    thead++;
                    if (thead == db.length()) {
                        es->add(err::Error("DirectiveProcessor", "advanceHeadToBlockClose", "Unclosed block (to match \"" + db.get(head).getOriginal() + "\")!"));
                        return;
                    }
                    if (db.get(thead).getHint() == dhints::COMPARATION || db.get(thead).getHint() == dhints::FUNCTIONDEF) {
                        count++;
                    }
                    if (db.get(thead).getHint() == dhints::ENDFUNCTIONDEF || db.get(thead).getHint() == dhints::ENDCOMPARATION) {
                        count--;
                    }
                }
                head = thead;
            }

            void forceHeadAdvance(int offset) {
                head += offset;
            }

            void resolveAssignment(Directive cd) {
                string op = cd.getPiece(1);
                mem::Reference lv(cd.getPiece(0));
                mem::Value lv_r = cx.resolve(lv);

                mem::Value rv;
                if (cd.getPiece(2)[0] == '@') {
                    rv = resolveExternal(cd, 2);
                } else {
                    rv = resolveToValue(cd.getPiece(2));
                }


                if (lv_r.getType() != rv.getType()) {
                    es->add(err::Error("DirectiveProcessor", "resolveAssignment", "Type mismatch between \"" + cd.getPiece(0) + "\" and \"" + cd.getPiece(2) + "\"!"));
                    return;
                }

                if (op == "=") {
                    cx.set(lv, rv);
                    return;
                }
                if (op == "+=") {
                    if (lv_r.getType() == mem::types::STRING) {
                        cx.set(lv, lv_r.gets() + rv.gets());
                    } else {
                        cx.set(lv, lv_r.getn() + rv.getn());
                    }
                    return;
                }
                if (lv_r.getType() == mem::types::NUMBER) {
                    if (op == "-=") {
                        cx.set(lv, lv_r.getn() - rv.getn());
                        return;
                    }
                    if (op == "*=") {
                        cx.set(lv, lv_r.getn() * rv.getn());
                        return;
                    }
                    if (op == "/=") {
                        if (rv.getn() == 0) {
                            es->add(err::Error("DirectiveProcessor", "resolveAssignment", "Divide by zero!"));
                        }
                        cx.set(lv, lv_r.getn() / rv.getn());
                        return;
                    }
                }
                es->add(err::Error("DirectiveProcessor", "resolveAssignment", "Unknown operation \"" + op + "\""));
                return;
            }

            mem::Value resolveExternal(Directive cd) {
                return resolveExternal(cd, -1);
            }

            mem::Value resolveExternal(Directive cd, int callpos) {

                string call;
                int pos;
                if (callpos == -1) {
                    if (cd.getPiece(0)[0] == '@') {
                        pos = 1;
                        call = cd.getPiece(0);
                    } else if (cd.getPiece(2)[0] == '@') {
                        pos = 3;
                        call = cd.getPiece(2);
                    }
                } else {
                    call = cd.getPiece(callpos);
                    pos = callpos + 1;
                }

                string pkg = call.substr(1, call.find(".") - 1);
                string name = call.substr(call.find(".") + 1, call.length() - 1);
                vector<mem::Value> args;
                for (int c = pos; c < cd.numberOfPieces(); c++) {
                    args.push_back(resolveToValue(cd.getPiece(c)));
                }
                np->setErrorStack(*es);
                return np->call(pkg, name, args);
            }
        };




    }
}

#endif
