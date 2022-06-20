﻿#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <set>

#include "kompasUtils.hpp"

#import "ksconstants.tlb" no_namespace named_guids
#import "ksConstants3D.tlb" no_namespace named_guids
#import "kAPI5.tlb" no_namespace named_guids rename( "min", "Imin" ) rename( "max", "Imax" ) rename( "ksFragmentLibrary", "ksIFragmentLibrary" )
#import "kAPI7.tlb" no_namespace named_guids rename( "CreateWindow", "ICreateWindow" ) rename( "PostMessage", "IPostMessage" ) rename( "MessageBoxEx", "IMessageBoxEx" )




void checkPlane(KompasObjectPtr kompas, double a, double b, double c, double d, int* s1, int* s2) {
    double eps = 0.0000001;
    *s1 = 0;
    *s2 = 0;
    //EntityCollection entityCollection();
    IApplicationPtr api7 = kompas->ksGetApplication7();
    IKompasDocument3DPtr document3d(api7->GetActiveDocument());
    IPart7Ptr topPart(document3d->GetTopPart());
    ksPartPtr part = kompas->TransferInterface(topPart, 1, 0);
    ksFeaturePtr feature(part->GetFeature());
    ksEntityCollectionPtr entityCollection(feature->EntityCollection(o3d_vertex));
    entityCollection->GetCount();
    for (int i = 0; i < entityCollection->GetCount(); i++) {
        ksEntityPtr entity(entityCollection->GetByIndex(i));
        ksVertexDefinitionPtr vertex(entity->GetDefinition());
        double x, y, z;
        if (vertex) {
            vertex->GetPoint(&x, &y, &z);
            if ((x * a) + (y * b) + (z * c) + d > eps) {
                (*s1)++;
            }
            else if ((x * a) + (y * b) + (z * c) + d < -eps) {
                (*s2)++;
            }
        }
    }
}

typedef struct {
    double a, b, c, d;
} PlaneEq;

/** Возвращает выбранную пользователем грань, если она удовлетворяет всем требованиям. A также через аргумент *planeEq, передаёт коэф. уравнения плоскости.
**/
ksFaceDefinitionPtr getSelectedPlane(KompasObjectPtr kompas, PlaneEq* planeEq) {
    std::setlocale(LC_ALL, "Russian");
    if (kompas) {
        ksDocument3DPtr doc3d = kompas->ActiveDocument3D();
        ksSelectionMngPtr selectionMng(doc3d->GetSelectionMng());
        std::cout << "Количество выбранных элементов:" << selectionMng->GetCount() << "\n";
        if (selectionMng->GetCount() == 1 && selectionMng->GetObjectType(0) == 105) {
            ksEntityPtr element(selectionMng->GetObjectByIndex(0));
            if (element) {
                std::cout << "Тип:" << element->type << "\n";
                if (element->type == 6) {
                    std::cout << "Была выбрана грань" << "\n";
                    ksFaceDefinitionPtr face(element->GetDefinition());
                    if (face->IsPlanar()) {
                        std::cout << "Грань является плоской" << "\n";

                        ksSurfacePtr surface(face->GetSurface());
                        ksPlaneParamPtr planeParam(surface->GetSurfaceParam());
                        ksPlacementPtr placement(planeParam->GetPlacement());
                        double x0, y0, z0;

                        // получааем координаты точки отсчёта
                        placement->GetOrigin(&x0, &y0, &z0);
                        double a, b, c, d;

                        surface->GetNormal(surface->GetParamUMax(), surface->GetParamVMax(), &a, &b, &c);

                        d = -((a * x0) + (b * y0) + (c * z0));

                        std::cout << "Вектор нормали: X=" << a << " Y="
                            << b << " Z=" << c << "\n";

                        std::cout << "Координаты точки отсчёта: X=" << x0 << " Y="
                            << y0 << " Z=" << z0 << "\n";

                        std::cout << "Полученые коэф. уравнения плоск: A=" << a << " B="
                            << b << " C=" << c << " D=" << d << "\n";

                        int s1 = 0, s2 = 0;

                        std::cout << "Идёт проверка на перечения..." << "\n";

                        checkPlane(kompas, a, b, c, d, &s1, &s2);
                        std::cout << "Сторона 1:" << s1 << " Сторона 2:" << s2 << " \n";

                        if (s1 == 0 && s2 > 0) {
                            std::cout << "Уравнение плоскости отражено" << "\n";
                            a = -a;
                            b = -b;
                            c = -c;
                            d = -d;
                            int buff = s2;
                            s2 = s1;
                            s1 = buff;
                        }


                        if (!((s1 > 0) && (s2 == 0))) {
                            kompas->ksMessage("Плоскость печати пересекает деталь!");
                        }
                        else {
                            kompas->ksMessage("Плоскость печати успешно выбрана!");
                            planeEq->a = a;
                            planeEq->b = b;
                            planeEq->c = c;
                            planeEq->d = d;
                            return face;
                        }

                    }
                    else {
                        kompas->ksMessage("Выбранная грань должна быть плоской!");
                    }
                }
                else {
                    kompas->ksMessage("Выбранный элемент не является гранью!");
                }
            }
            else {
                std::cout << "WHAT?" << "\n";
            }

        }
        else {
            kompas->ksMessage("Должен быть был выбран только один элемент в виде плоской грани!");
        }

    }
    return nullptr;
}


int main() {
    CoInitialize(nullptr);
    KompasObjectPtr kompas = kompasInit();
    PlaneEq planeEq;
    ksFaceDefinitionPtr face = getSelectedPlane(kompas, &planeEq);
}