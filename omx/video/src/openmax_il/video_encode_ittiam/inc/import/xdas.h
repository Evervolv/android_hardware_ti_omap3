/*
 *  Copyright 2002 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) XDAS 2.51.00 02-20-03 (xdas-d20)" */
/*
 *  ======== xdas.h ========
 *  This header defines types and constants used in the XDAS interfaces.
 *  The types are mapped to the types defined in std.h
 */
#ifndef XDAS_
#define XDAS_

#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XDAS_TRUE       1
#define XDAS_FALSE      0

typedef Void            XDAS_Void;
typedef SmUns           XDAS_Bool;

typedef SmInt           XDAS_Int8;      /* Actual size chip dependent */
typedef SmUns           XDAS_UInt8;     /* Actual size chip dependent */
typedef MdInt           XDAS_Int16;     /* Actual size of type is 16 bits */
typedef MdUns           XDAS_UInt16;    /* Actual size of type is 16 bits */
typedef LgInt           XDAS_Int32;     /* Actual size of type is 32 bits */
typedef LgUns           XDAS_UInt32;    /* Actual size of type is 32 bits */

#ifdef __cplusplus
}
#endif

#endif  /* XDAS_ */
