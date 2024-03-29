#include "kompasUtils.hpp"

#include "StdAfx.h"

#import <kAPI5.tlb> no_namespace named_guids rename( "min", "Imin" ) rename( "max", "Imax" ) rename( "ksFragmentLibrary", "ksIFragmentLibrary" )

KompasObjectPtr getKompasObjectPtr() {
    KompasObjectPtr kompas(NULL);
    CString filename;
    if (::GetModuleFileName(NULL, filename.GetBuffer(255), 255)) {
        filename.ReleaseBuffer(255);
        CString libname = "kAPI5.dll";
        filename.Replace(filename.Right(filename.GetLength() - (filename.ReverseFind('\\') + 1)), libname);

        HINSTANCE hAppAuto = LoadLibrary(filename); // идентификатор kAPI5.dll
        if (hAppAuto) {
            typedef LPDISPATCH(WINAPI* FCreateKompasObject)();
            FCreateKompasObject pCreateKompasObject = (FCreateKompasObject)GetProcAddress(hAppAuto, "CreateKompasObject");
            if (pCreateKompasObject) {
                kompas = IDispatchPtr(pCreateKompasObject(), false);
            }
            FreeLibrary(hAppAuto);
        }
    }
    return kompas;
}
