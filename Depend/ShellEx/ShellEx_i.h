

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ShellEx_i_h__
#define __ShellEx_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOverlayIcon_FWD_DEFINED__
#define __IOverlayIcon_FWD_DEFINED__
typedef interface IOverlayIcon IOverlayIcon;
#endif 	/* __IOverlayIcon_FWD_DEFINED__ */


#ifndef __IContextMenuEx_FWD_DEFINED__
#define __IContextMenuEx_FWD_DEFINED__
typedef interface IContextMenuEx IContextMenuEx;
#endif 	/* __IContextMenuEx_FWD_DEFINED__ */


#ifndef __IPropPageEx_FWD_DEFINED__
#define __IPropPageEx_FWD_DEFINED__
typedef interface IPropPageEx IPropPageEx;
#endif 	/* __IPropPageEx_FWD_DEFINED__ */


#ifndef __OverlayIcon_FWD_DEFINED__
#define __OverlayIcon_FWD_DEFINED__

#ifdef __cplusplus
typedef class OverlayIcon OverlayIcon;
#else
typedef struct OverlayIcon OverlayIcon;
#endif /* __cplusplus */

#endif 	/* __OverlayIcon_FWD_DEFINED__ */


#ifndef __ContextMenuEx_FWD_DEFINED__
#define __ContextMenuEx_FWD_DEFINED__

#ifdef __cplusplus
typedef class ContextMenuEx ContextMenuEx;
#else
typedef struct ContextMenuEx ContextMenuEx;
#endif /* __cplusplus */

#endif 	/* __ContextMenuEx_FWD_DEFINED__ */


#ifndef __PropPageEx_FWD_DEFINED__
#define __PropPageEx_FWD_DEFINED__

#ifdef __cplusplus
typedef class PropPageEx PropPageEx;
#else
typedef struct PropPageEx PropPageEx;
#endif /* __cplusplus */

#endif 	/* __PropPageEx_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IOverlayIcon_INTERFACE_DEFINED__
#define __IOverlayIcon_INTERFACE_DEFINED__

/* interface IOverlayIcon */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IOverlayIcon;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4C2F6DD5-556D-465E-A75F-08CE3826E6F5")
    IOverlayIcon : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IOverlayIconVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOverlayIcon * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOverlayIcon * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOverlayIcon * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IOverlayIcon * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IOverlayIcon * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IOverlayIcon * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IOverlayIcon * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IOverlayIconVtbl;

    interface IOverlayIcon
    {
        CONST_VTBL struct IOverlayIconVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOverlayIcon_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IOverlayIcon_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IOverlayIcon_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IOverlayIcon_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IOverlayIcon_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IOverlayIcon_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IOverlayIcon_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOverlayIcon_INTERFACE_DEFINED__ */


#ifndef __IContextMenuEx_INTERFACE_DEFINED__
#define __IContextMenuEx_INTERFACE_DEFINED__

/* interface IContextMenuEx */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IContextMenuEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("68C6FAD9-8EF3-4D40-8F66-73FF3FFE3134")
    IContextMenuEx : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IContextMenuExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContextMenuEx * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContextMenuEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContextMenuEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IContextMenuEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IContextMenuEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IContextMenuEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IContextMenuEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IContextMenuExVtbl;

    interface IContextMenuEx
    {
        CONST_VTBL struct IContextMenuExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContextMenuEx_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IContextMenuEx_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IContextMenuEx_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IContextMenuEx_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IContextMenuEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IContextMenuEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IContextMenuEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IContextMenuEx_INTERFACE_DEFINED__ */


#ifndef __IPropPageEx_INTERFACE_DEFINED__
#define __IPropPageEx_INTERFACE_DEFINED__

/* interface IPropPageEx */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IPropPageEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("707999AD-02F0-497A-AC71-3AB957347EC2")
    IPropPageEx : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IPropPageExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropPageEx * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropPageEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropPageEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPropPageEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPropPageEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPropPageEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPropPageEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IPropPageExVtbl;

    interface IPropPageEx
    {
        CONST_VTBL struct IPropPageExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropPageEx_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropPageEx_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropPageEx_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropPageEx_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPropPageEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPropPageEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPropPageEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPropPageEx_INTERFACE_DEFINED__ */



#ifndef __ShellExLib_LIBRARY_DEFINED__
#define __ShellExLib_LIBRARY_DEFINED__

/* library ShellExLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_ShellExLib;

EXTERN_C const CLSID CLSID_OverlayIcon;

#ifdef __cplusplus

class DECLSPEC_UUID("C339E6B5-55AF-41E1-91C6-FF5B669D244E")
OverlayIcon;
#endif

EXTERN_C const CLSID CLSID_ContextMenuEx;

#ifdef __cplusplus

class DECLSPEC_UUID("A4250BAE-CDC3-48F9-A25B-9CFEAFEED05A")
ContextMenuEx;
#endif

EXTERN_C const CLSID CLSID_PropPageEx;

#ifdef __cplusplus

class DECLSPEC_UUID("E3D874A1-F261-48F5-9DE6-9E12D50C9BB4")
PropPageEx;
#endif
#endif /* __ShellExLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


