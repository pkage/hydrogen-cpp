//
//  directive.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_directive_h
#define hydrogen_directive_h
using namespace std;
#include "directivehints.h"

namespace hyd {
    namespace dir {
        class Directive {
        private:
            vector<string> pieces;
            string original;
            int hint;
        public:
            Directive() {
                hint = dhints::UNKNOWN;
            }
            Directive(string str) {
                this->set(str);
            }

            void splitIntoPieces() {
                pieces.clear();
                string tmp;
                bool insideQuotes = false;
                for (int c = 0; c < original.length(); c++) {
                    char curr = original[c];
                    if (curr == '\"' && (c != 0 && original[c-1] != '\\')) {
                        insideQuotes = !insideQuotes;
                    }
                    tmp += curr;
                    if ((curr == ' ' && !insideQuotes) || c + 1 == original.length()) {
                        if (tmp[tmp.length() - 1] == ' ') {
                            tmp = tmp.substr(0, tmp.length() - 1);
                        }
                        pieces.push_back(tmp);
                        tmp = "";

                    }


                }
            }

            // classify the directive
            void classify() {
                splitIntoPieces();
                if (pieces.size() == 0) {
                    hint = dhints::EMPTY;
                    return;
                }
                string fp = pieces[0];
                if (fp[0] == '$') {
                    hint = dhints::ASSIGNMENT;
                } else if (fp == "if") {
                    hint = dhints::COMPARATION;
                } else if (fp == "import") {
                    hint = dhints::IMPORT;
                } else if (fp == "declare") {
                    hint = dhints::DECLAREVAR;
                } else if ((fp[0] == '/' && fp[1] == '/') || fp[0] == '#') {
                    hint = dhints::COMMENT;
                } else if (fp[0] == '@' || (pieces.size() >= 3 && pieces[2][0] == '@')) {
                    hint = dhints::EXTERNALCALL;
                } else if (fp == "end") {
                    hint = dhints::ENDFUNCTIONDEF;
                } else if (fp == "endif") {
                    hint = dhints::ENDCOMPARATION;
                } else if (fp == "def") {
                    hint = dhints::FUNCTIONDEF;
                } else if (fp[0] == '.') {
                    hint = dhints::INTERNALCALL;
                } else if (fp == "label") {
                    hint = dhints::LABEL;
                } else if (fp == "goto") {
                    hint = dhints::GOTO;
                } else if (fp == "return") {
                    hint = dhints::RETURN;
                } else {
                    hint = dhints::UNKNOWN;
                }
            }

            int getHint() {
                return hint;
            }

            string getOriginal() {
                return original;
            }

            bool validPieceIndex(int index) {
                return (index >= 0 && index < pieces.size());
            }

            string getPiece(int index) {
                if (validPieceIndex(index)) {
                    return pieces.at(index);
                }
                return "";
            }

            void reset() {
                hint = dhints::UNKNOWN;
                original = "";
                pieces.clear();
            }

            int numberOfPieces() {
                return (int)pieces.size();
            }

            void set(string str) {
                original = str;
                classify();
            }
        };
    }
}

#endif
