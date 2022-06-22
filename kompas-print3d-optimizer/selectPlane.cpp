#include "StdAfx.h"
#include "selectPlane.hpp"
#include <iostream>
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
            } else if ((x * a) + (y * b) + (z * c) + d < -eps) {
                (*s2)++;
            }
        }
    }
}

bool PlaneEq::isVertical(ksEdgeDefinitionPtr edge, double cos_angle) {
    if (edge) {
        /*try
        {*/
        double x1, x2, y1, y2, z1, z2, x, y, z;
        ksVertexDefinitionPtr vertex1(edge->GetVertex(true));
        ksVertexDefinitionPtr vertex2(edge->GetVertex(false));
        if (vertex1 && vertex2) {
            vertex1->GetPoint(&x1, &y1, &z1);
            vertex2->GetPoint(&x2, &y2, &z2);
            x = x2 - x1;
            y = y2 - y1;
            z = z2 - z1;
            double n_mul_vect = (a * x) + (b * x) + (c * z);
            double n_abs = sqrt((a * a) + (b * b) + (c * c));
            double vect_abs = sqrt((x * x) + (y * y) + (z * z));
            if ((n_abs > 0.001) && (vect_abs > 0.001)) {
                double cos_n_vect = n_mul_vect / (n_abs * vect_abs);
                return abs(cos_n_vect) > cos_angle;
            }
        }
        /*
    }
    catch (_com_error& e)
    {
        return false;
    }*/
    }
    return false;
}

/** ���������� ��������� ������������� �����, ���� ��� ������������� ���� �����������. A ����� ����� �������� *planeEq, ������� ����. ��������� ���������.
**/
ksFaceDefinitionPtr getSelectedPlane(KompasObjectPtr kompas, PlaneEq* planeEq) {
    std::setlocale(LC_ALL, "Russian");
    if (kompas) {
        ksDocument3DPtr doc3d = kompas->ActiveDocument3D();
        ksSelectionMngPtr selectionMng(doc3d->GetSelectionMng());
        std::cout << "���������� ��������� ���������:" << selectionMng->GetCount() << "\n";
        if (selectionMng->GetCount() == 1 && selectionMng->GetObjectType(0) == 105) {
            ksEntityPtr element(selectionMng->GetObjectByIndex(0));
            if (element) {
                std::cout << "���:" << element->type << "\n";
                if (element->type == 6) {
                    std::cout << "���� ������� �����" << "\n";
                    ksFaceDefinitionPtr face(element->GetDefinition());
                    if (face->IsPlanar()) {
                        std::cout << "����� �������� �������" << "\n";

                        ksSurfacePtr surface(face->GetSurface());
                        ksPlaneParamPtr planeParam(surface->GetSurfaceParam());
                        ksPlacementPtr placement(planeParam->GetPlacement());
                        double x0, y0, z0;

                        // ��������� ���������� ����� �������
                        placement->GetOrigin(&x0, &y0, &z0);
                        double a, b, c, d;

                        surface->GetNormal(surface->GetParamUMax(), surface->GetParamVMax(), &a, &b, &c);

                        d = -((a * x0) + (b * y0) + (c * z0));

                        std::cout << "������ �������: X=" << a << " Y="
                            << b << " Z=" << c << "\n";

                        std::cout << "���������� ����� �������: X=" << x0 << " Y="
                            << y0 << " Z=" << z0 << "\n";

                        std::cout << "��������� ����. ��������� �����: A=" << a << " B="
                            << b << " C=" << c << " D=" << d << "\n";

                        int s1 = 0, s2 = 0;

                        std::cout << "��� �������� �� ���������..." << "\n";

                        checkPlane(kompas, a, b, c, d, &s1, &s2);
                        std::cout << "������� 1:" << s1 << " ������� 2:" << s2 << " \n";

                        if (s1 == 0 && s2 > 0) {
                            std::cout << "��������� ��������� ��������" << "\n";
                            a = -a;
                            b = -b;
                            c = -c;
                            d = -d;
                            int buff = s2;
                            s2 = s1;
                            s1 = buff;
                        }


                        if (!((s1 > 0) && (s2 == 0))) {
                            kompas->ksMessage("��������� ������ ���������� ������!");
                        } else {
                            kompas->ksMessage("��������� ������ ������� �������!");
                            planeEq->a = a;
                            planeEq->b = b;
                            planeEq->c = c;
                            planeEq->d = d;
                            return face;
                        }

                    } else {
                        kompas->ksMessage("��������� ����� ������ ���� �������!");
                    }
                } else {
                    kompas->ksMessage("��������� ������� �� �������� ������!");
                }
            } else {
                std::cout << "WHAT?" << "\n";
            }

        } else if (selectionMng->GetCount() > 0) {
            kompas->ksMessage("������ ��� ���� ������ ������ ���� ������� � ���� ������� �����!");
        } else {
            kompas->ksMessage("��������� ������ �� �������!");
        }

    }
    return nullptr;
}
