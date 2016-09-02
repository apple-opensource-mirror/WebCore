/*
 *  WebCoreTelephoneParser.h
 *  WebCore
 *
 *  Copyright (C) 2007, 2008, 2009 Apple Inc.  All rights reserved.
 */

#ifndef WebCoreTelephoneParser_h
#define WebCoreTelephoneParser_h

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif
    
/*
 method: WebCoreFindTelephoneNumber(UniChar [], unsigned *, int *, int *)
 description: parses string looking for a phone number
 in string: Array of UniChars to parse
 in len: Number of characters in 'string'
 out startPos: index of first charcater in phone number, -1 if not found
 out endPos: index of last character in phone number, -1 if not found.
 */
void WebCoreFindTelephoneNumber(const UniChar string[], unsigned len, int *startPos, int *endPos);

#ifdef __cplusplus
}
#endif

#endif // WebCoreTelephoneParser_h