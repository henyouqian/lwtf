//
// Prefix header for all source files of the 'lwtf' target in the 'lwtf' project
//

#ifdef __OBJC__
    #import <Foundation/Foundation.h>
#endif

#define HAVE_STDINT_H 1
#define HAVE_STAT 1
#define HAVE_USLEEP 1
#define HAVE_UNISTD_H 1

#ifdef __cplusplus
#include "lwPreInclude.h"
#include "lwLog.h"
#endif
