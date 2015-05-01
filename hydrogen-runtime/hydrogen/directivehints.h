//
//  directivehints.h
//  hydrogen
//
//  Created by Patrick Kage on 4/10/15.
//  Copyright (c) 2015 Patrick Kage. All rights reserved.
//

#ifndef hydrogen_directivehints_h
#define hydrogen_directivehints_h

namespace hyd {
    namespace dir {
        namespace dhints {
            enum {UNKNOWN, EMPTY, COMMENT, DECLAREVAR, ASSIGNMENT, COMPARATION, ENDCOMPARATION, IMPORT, EXTERNALCALL, INTERNALCALL, FUNCTIONDEF, ENDFUNCTIONDEF, GOTO, LABEL, RETURN};
        }
    }
}

#endif
