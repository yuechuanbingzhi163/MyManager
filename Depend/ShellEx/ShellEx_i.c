

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Apr 23 14:17:04 2015
 */
/* Compiler settings for ShellEx.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IOverlayIcon,0x4C2F6DD5,0x556D,0x465E,0xA7,0x5F,0x08,0xCE,0x38,0x26,0xE6,0xF5);


MIDL_DEFINE_GUID(IID, IID_IContextMenuEx,0x68C6FAD9,0x8EF3,0x4D40,0x8F,0x66,0x73,0xFF,0x3F,0xFE,0x31,0x34);


MIDL_DEFINE_GUID(IID, IID_IPropPageEx,0x707999AD,0x02F0,0x497A,0xAC,0x71,0x3A,0xB9,0x57,0x34,0x7E,0xC2);


MIDL_DEFINE_GUID(IID, LIBID_ShellExLib,0x1AD8F1E0,0x94CA,0x4F4E,0x80,0x86,0xA1,0x38,0xEF,0xAD,0x64,0x97);


MIDL_DEFINE_GUID(CLSID, CLSID_OverlayIcon,0xC339E6B5,0x55AF,0x41E1,0x91,0xC6,0xFF,0x5B,0x66,0x9D,0x24,0x4E);


MIDL_DEFINE_GUID(CLSID, CLSID_ContextMenuEx,0xA4250BAE,0xCDC3,0x48F9,0xA2,0x5B,0x9C,0xFE,0xAF,0xEE,0xD0,0x5A);


MIDL_DEFINE_GUID(CLSID, CLSID_PropPageEx,0xE3D874A1,0xF261,0x48F5,0x9D,0xE6,0x9E,0x12,0xD5,0x0C,0x9B,0xB4);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



