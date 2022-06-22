﻿#include <iostream>
#include <float.h>
#include "kompasUtils.hpp"
#include "selectPlane.hpp"
#include "optimizeRounding.hpp"

#import "ksconstants.tlb" no_namespace named_guids
#import "ksConstants3D.tlb" no_namespace named_guids
#import "kAPI5.tlb" no_namespace named_guids rename( "min", "Imin" ) rename( "max", "Imax" ) rename( "ksFragmentLibrary", "ksIFragmentLibrary" )
#import "kAPI7.tlb" no_namespace named_guids rename( "CreateWindow", "ICreateWindow" ) rename( "PostMessage", "IPostMessage" ) rename( "MessageBoxEx", "IMessageBoxEx" )


int main() {
    CoInitialize(nullptr);
    KompasObjectPtr kompas = kompasInit();
    PlaneEq planeEq;
    ksFaceDefinitionPtr face = getSelectedPlane(kompas, &planeEq);
    if (face) {
        double radius;
        double angle;
        if (kompas->ksReadDouble("Радиус: ", 0.0, -DBL_MIN, DBL_MAX, &radius) == 1 && kompas->ksReadDouble("Граничный угол: ", 60, -DBL_MIN, DBL_MAX, &angle) == 1) {
            optimizeByRounding(kompas, face, planeEq, radius, angle);
        }
    }
}
