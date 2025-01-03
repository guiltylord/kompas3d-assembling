#include "pch.h"
#include "Assembler.h"

#include <cmath>

#include <comutil.h>
#include "afxdialogex.h"
#include <atlsafe.h>

#include "framework.h"

#include <string>

# define M_PI           3.14159265358979323846  /* pi */

using namespace Kompas6API5;
using namespace std;

KompasObjectPtr pKompasApp5;

Assembler::Assembler()
{
}

Assembler::~Assembler()
{
}

SealData Assembler::GetSeal(int type)
{
	auto seal = SealData();
	switch (type) {
	case 1:
		seal.BaseRad = 23;
		seal.BaseAngleBig = 65 * M_PI / 180;
		seal.BaseAngleSmall = 55 * M_PI / 180;
		seal.BaseDepth = 3;
		
		seal.HexRad = 21.9 / 2;
		seal.HexDepth = 15;

		seal.AxHoleRad = 7;
		seal.AxHoleThruRad = 4;

		seal.X_Pin = 34/2;
		seal.GrooveRad = 8 / 2;
		seal.GrooveDepth = 2;
		seal.PinRad = 4.5 / 2;
	}

	return seal;
}

ScrewData Assembler::GetScrew(int type)
{
	auto screw = ScrewData();
	switch (type) {
	case 1:
		screw.FullHeight = 17;
		screw.LegThick = 3;

		screw.HexDepth = 3;
		screw.HexRad = 21.9 / 2;

		screw.AxHoleRad = 5;

		screw.GasketHeight = 1;
		screw.GasketWidth = 2;
	}

	return screw;
}

PuckData Assembler::GetPuck(int type)
{
	auto puck = PuckData();
	switch (type) {
	case 1:
		puck.RadIn = 3.2;
		puck.RadOut = 7;
		puck.Height = 1;
	}

	return puck;
}

void Assembler::CreateSeal()
{
	ksDocument3DPtr p3DDoc;
	CComPtr<IUnknown> pKompasAppUnk = nullptr;

	if (!pKompasApp5)
	{
		// �������� CLSID ��� ������
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// ��������� ���� �� ���������� ��������� ������
		//���� ���� �������� IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// ���������� ��������� ������ ����� ��� ��� ����������� ���
			// ����� �������� IUnknown ��� ������ ��� ����������� ���������� ������
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// �������� ��������� ���������� ������
		hRes = pKompasAppUnk->QueryInterface(__uuidof(KompasObject), (void**)&pKompasApp5);
		if (FAILED(hRes)) {
			return;
		}
	}

	pKompasApp5->Visible = true;

	p3DDoc = pKompasApp5->Document3D();
	p3DDoc->Create(false, true);
	p3DDoc = pKompasApp5->ActiveDocument3D();
	ksPartPtr pPart = p3DDoc->GetPart(pTop_Part);
	ksDocument2DPtr p2DDoc;

	SealData Seal = GetSeal(1);

	//����� Base
	ksEntityPtr pBaseSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pBaseSketchDef = pBaseSketch->GetDefinition();
	pBaseSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pBaseSketch->Create();
	p2DDoc = pBaseSketchDef->BeginEdit();

	auto cosBig = cos(Seal.BaseAngleBig);
	auto sinBig = sin(Seal.BaseAngleBig);
	auto cosSmall = cos(Seal.BaseAngleSmall);
	auto sinSmall = sin(Seal.BaseAngleSmall);

	auto X1 = Seal.BaseRad * cosBig;
	auto Y1 = Seal.BaseRad * sinBig;
	auto X2 = Seal.BaseRad * cosSmall;
	auto Y2 = Seal.BaseRad * sinSmall;
	auto X3 = Seal.BaseRad * cos(Seal.BaseAngleSmall+120*M_PI/180);
	auto Y3 = Seal.BaseRad * sin(Seal.BaseAngleSmall+120*M_PI/180);

	p2DDoc->ksLineSeg(X1, Y1, X2, Y2, 1);
	p2DDoc->ksLineSeg(X2, Y2, X2, -Y2, 1);
	p2DDoc->ksLineSeg(X2, -Y2, X1, -Y1, 1);
	p2DDoc->ksLineSeg(X1, -Y1, X3, -Y3, 1);
	p2DDoc->ksLineSeg(X3, -Y3, X3, Y3, 1);
	p2DDoc->ksLineSeg(X3, Y3, X1, Y1, 1);

	pBaseSketchDef->EndEdit();

	//������������ Base
	ksEntityPtr pBaseExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pBaseBossExtrusionDef = pBaseExtrude->GetDefinition();
	pBaseBossExtrusionDef->directionType = dtNormal;
	pBaseBossExtrusionDef->SetSketch(pBaseSketch);
	pBaseBossExtrusionDef->SetSideParam(true, 0, Seal.BaseDepth, 0, false);
	pBaseExtrude->Create();

	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	auto t = 0;
	auto c = flFaces->GetCount();
	for (int i = 0; i < c; i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pBaseExtrude) {
			if (def->IsPlanar()) {
				//ksEntityPtr* OldFace = (ksEntityPtr *&)face;
				ksEdgeCollectionPtr col = def->EdgeCollection();
				int cEdges = col->GetCount();
				for (int k = 0; k < cEdges; k++) {

					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);
					ksVertexDefinitionPtr p2 = d->GetVertex(false);


					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);

					double x2, y2, z2;
					p2->GetPoint(&x2, &y2, &z2);

					if (z1 == 3.f && cEdges == 6) {
						t++;
					}
				}
				if (t == 6) {
					face->Putname("Face4Assembly1");
					face->Update();
					//break;
					t = 0;
				}

			}
		}
	}

	//flFaces->Clear();

	//��������� ���������
	ksEntityPtr pBasePlane = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pBaseOffsetPlane = pBasePlane->GetDefinition();
	pBaseOffsetPlane->direction = true;
	pBaseOffsetPlane->offset = Seal.BaseDepth;
	pBaseOffsetPlane->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pBasePlane->Create();

	//����� Hex
	ksEntityPtr pHexSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pHexSketchDef = pHexSketch->GetDefinition();
	pHexSketchDef->SetPlane(pBasePlane);
	pHexSketch->Create();

	auto X_hex2 = Seal.HexRad * cos(30*M_PI/180);
	auto Y_hex2 = Seal.HexRad / 2;

	p2DDoc = pHexSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Seal.HexRad, X_hex2, Y_hex2, 1);
	p2DDoc->ksLineSeg(X_hex2, Y_hex2, X_hex2, -Y_hex2, 1);
	p2DDoc->ksLineSeg(X_hex2, -Y_hex2, 0, -Seal.HexRad, 1);
	p2DDoc->ksLineSeg(0, -Seal.HexRad, -X_hex2, -Y_hex2, 1);
	p2DDoc->ksLineSeg(-X_hex2, -Y_hex2, -X_hex2, Y_hex2, 1);
	p2DDoc->ksLineSeg(-X_hex2, Y_hex2, 0, Seal.HexRad, 1);

	pHexSketchDef->EndEdit();

	//����� ��������� ����������� Groove
	ksEntityPtr pGrooveSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pGrooveSketchDef = pGrooveSketch->GetDefinition();
	pGrooveSketchDef->SetPlane(pBasePlane);
	pGrooveSketch->Create();

	p2DDoc = pGrooveSketchDef->BeginEdit();
	p2DDoc->ksCircle(-Seal.X_Pin, 0, Seal.GrooveRad, 1);

	pGrooveSketchDef->EndEdit();

	//����� ��������� ��������� Pin
	ksEntityPtr pPinSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pPinSketchDef = pPinSketch->GetDefinition();
	pPinSketchDef->SetPlane(pBasePlane);
	pPinSketch->Create();

	p2DDoc = pPinSketchDef->BeginEdit();
	p2DDoc->ksCircle(-Seal.X_Pin, 0, Seal.PinRad, 1);

	pPinSketchDef->EndEdit();

	//������������ Hex
	ksEntityPtr pHexExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHexBossExtrusionDef = pHexExtrude->GetDefinition();
	pHexBossExtrusionDef->directionType = dtNormal;
	pHexBossExtrusionDef->SetSketch(pHexSketch);
	pHexBossExtrusionDef->SetSideParam(true, 0, Seal.HexDepth, 0, false);
	pHexExtrude->Create();

	//��������� Groove
	ksEntityPtr pGrooveCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pGrooveCutExtrudeDef = pGrooveCutExtrude->GetDefinition();
	pGrooveCutExtrudeDef->directionType = dtNormal;
	pGrooveCutExtrudeDef->SetSketch(pGrooveSketchDef);
	pGrooveCutExtrudeDef->SetSideParam(true, etBlind, Seal.GrooveDepth, 0, false);
	pGrooveCutExtrude->Create();

	//��������� Pin
	ksEntityPtr pPinCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pPinCutExtrudeDef = pPinCutExtrude->GetDefinition();
	pPinCutExtrudeDef->directionType = dtNormal;
	pPinCutExtrudeDef->SetSketch(pPinSketchDef);
	pPinCutExtrudeDef->SetSideParam(true, etThroughAll, 0, 0, false);
	pPinCutExtrude->Create();

	//������ Groove+Pin
	ksEntityPtr pCircCopy = pPart->NewEntity(o3d_circularCopy); 
	ksCircularCopyDefinitionPtr pCircCopyDef = pCircCopy->GetDefinition();

	pCircCopyDef->Putcount2(3);
	pCircCopyDef->SetAxis(pPart->GetDefaultEntity(o3d_axisOZ)); 

	ksEntityCollectionPtr pEntColl = pCircCopyDef->GetOperationArray();
	pEntColl->Clear();
	pEntColl->Add(pGrooveCutExtrude);
	pEntColl->Add(pPinCutExtrude);

	pCircCopy->Create();

	//��������� ��������� ��� ������� ���������
	ksEntityPtr pTopPlane = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pTopPlaneDef = pTopPlane->GetDefinition();
	pTopPlaneDef->direction = true;
	pTopPlaneDef->offset = Seal.BaseDepth + Seal.HexDepth;
	pTopPlaneDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pTopPlane->Create();

	//����� ������� ��������� 
	ksEntityPtr pAxialHoleSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pAxialHoleSketchDef = pAxialHoleSketch->GetDefinition();
	pAxialHoleSketchDef->SetPlane(pTopPlane);
	pAxialHoleSketch->Create();
	p2DDoc = pAxialHoleSketchDef->BeginEdit();
	p2DDoc->ksCircle(0, 0, Seal.AxHoleRad, 1);
	pAxialHoleSketchDef->EndEdit();

	//��������� ������� ���������
	ksEntityPtr pAxialHoleCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pAxialHoleCutExtrudeDef = pAxialHoleCutExtrude->GetDefinition();
	pAxialHoleCutExtrudeDef->directionType = dtNormal;
	pAxialHoleCutExtrudeDef->SetSketch(pAxialHoleSketchDef);
	pAxialHoleCutExtrudeDef->SetSideParam(true, etBlind, Seal.HexDepth, 0, false);
	pAxialHoleCutExtrude->Create();

	//����� ������� ��������� ������
	ksEntityPtr pAxialHoleSketch2 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pAxialHoleSketchDef2 = pAxialHoleSketch2->GetDefinition();
	pAxialHoleSketchDef2->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pAxialHoleSketch2->Create();
	p2DDoc = pAxialHoleSketchDef2->BeginEdit();
	p2DDoc->ksCircle(0, 0, Seal.AxHoleThruRad, 1);
	pAxialHoleSketchDef2->EndEdit();

	//��������� ������� ���������
	ksEntityPtr pAxialHoleCutExtrude2 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pAxialHoleCutExtrudeDef2 = pAxialHoleCutExtrude2->GetDefinition();
	pAxialHoleCutExtrudeDef2->directionType = dtReverse;
	pAxialHoleCutExtrudeDef2->SetSketch(pAxialHoleSketchDef2);
	pAxialHoleCutExtrudeDef2->SetSideParam(true, etBlind, Seal.BaseDepth, 0, false);
	pAxialHoleCutExtrude2->Create();

	//p3DDoc->hideAllPlanes=true;	
	//p3DDoc->hideInComponentsMode; //doesnt work
	

	flFaces = pPart->EntityCollection(o3d_face);
	t = 0;
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pAxialHoleCutExtrude2) {
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				//auto n = Seal.AxHoleThruRad / 2;
				if (r == Seal.AxHoleThruRad) {
					face->Putname("Cylinder4Assembly1");
					face->Update();
				}
			}


		}
		if (def->GetOwnerEntity() == pHexExtrude) {
			if (def->IsPlanar()) {
				ksEdgeCollectionPtr col = def->EdgeCollection(); 
				int cEdges = col->GetCount();
				for (int k = 0; k < cEdges; k++) {

					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);
					ksVertexDefinitionPtr p2 = d->GetVertex(false);

					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);

					double x2, y2, z2;
					p2->GetPoint(&x2, &y2, &z2);

					if (z1 == 18.f && cEdges == 7) {
						t++;
					}
				}
				if (t == 7) {
					face->Putname("Face4Assembly0");
					face->Update();
					//break;

					t = 0;
				}
			}
		}
	}












	//�������� ���������� ������
	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "������ ��������";
	path += name+".m3d";


	pPart->SetAdvancedColor(RGB(150, 0, 0), 1, 1, 1, 1, 1, 0.5);
	pPart->Update();


	p3DDoc->fileName = _bstr_t(CString(name.c_str()));
	p3DDoc->SaveAs(_bstr_t(CString(path.c_str())));
}

void Assembler::CreateScrew()
{
	ksDocument3DPtr p3DDoc;
	CComPtr<IUnknown> pKompasAppUnk = nullptr;

	if (!pKompasApp5)
	{
		// �������� CLSID ��� ������
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// ��������� ���� �� ���������� ��������� ������
		//���� ���� �������� IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// ���������� ��������� ������ ����� ��� ��� ����������� ���
			// ����� �������� IUnknown ��� ������ ��� ����������� ���������� ������
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// �������� ��������� ���������� ������
		hRes = pKompasAppUnk->QueryInterface(__uuidof(KompasObject), (void**)&pKompasApp5);
		if (FAILED(hRes)) {
			return;
		}
	}

	pKompasApp5->Visible = true;

	p3DDoc = pKompasApp5->Document3D();
	p3DDoc->Create(false, true);
	p3DDoc = pKompasApp5->ActiveDocument3D();
	ksPartPtr pPart = p3DDoc->GetPart(pTop_Part);
	ksDocument2DPtr p2DDoc;

	//����� Hexagon
	ksEntityPtr pHexSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pHexSketchDef = pHexSketch->GetDefinition();
	pHexSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOZ));
	pHexSketch->Create();
	
	ScrewData Screw = GetScrew(1);
	//�������� ���
	auto X_hex = Screw.HexRad * cos(30 * M_PI / 180);
	auto Y_hex = Screw.HexRad / 2;

	p2DDoc = pHexSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Screw.HexRad, X_hex, Y_hex, 1);
	p2DDoc->ksLineSeg(X_hex, Y_hex, X_hex, -Y_hex, 1);
	p2DDoc->ksLineSeg(X_hex, -Y_hex, 0, -Screw.HexRad, 1);
	p2DDoc->ksLineSeg(0, -Screw.HexRad, -X_hex, -Y_hex, 1);
	p2DDoc->ksLineSeg(-X_hex, -Y_hex, -X_hex, Y_hex, 1);
	p2DDoc->ksLineSeg(-X_hex, Y_hex, 0, Screw.HexRad, 1);

	pHexSketchDef->EndEdit();

	//������������ Hexagon
	ksEntityPtr pHexExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHexExtrusionDef = pHexExtrude->GetDefinition();
	pHexExtrusionDef->directionType = dtNormal;
	pHexExtrusionDef->SetSketch(pHexSketch);
	pHexExtrusionDef->SetSideParam(true, 0, Screw.HexDepth, 0, false);
	pHexExtrude->Create();
	
	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	auto t = 0;
	auto c = flFaces->GetCount();
	auto isFirst = true;
	for (int i = 0; i < c; i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pHexExtrude) {
			if (def->IsPlanar()) {
				//ksEntityPtr* OldFace = (ksEntityPtr *&)face;
				ksEdgeCollectionPtr col = def->EdgeCollection();
				int cEdges = col->GetCount();
				for (int k = 0; k < cEdges; k++) {

					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);
					ksVertexDefinitionPtr p2 = d->GetVertex(false);


					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);

					double x2, y2, z2;
					p2->GetPoint(&x2, &y2, &z2);
					auto j = round(y1);
					int n = int(j);
					if (n == 3 && cEdges == 6) {
						t++;
					}

					if (cEdges == 4 && isFirst == true) {
						face->Putname("Face4Assembly21");
						face->Update();
						isFirst = false;
					}
				}
				if (t == 6) {
					face->Putname("Face4Assembly2");
					face->Update();
					//break;
					t = 0;
				}

			}
		}
	}

	flFaces->Clear();

	//� ����� ������ ����� ������� ����� �� �������� ����������. ������� ���� ���, ����� ������ ��� �������� �� ���� � ����� ����
	//����� Leg
	ksEntityPtr pLegSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pLegSketchDef = pLegSketch->GetDefinition();
	pLegSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pLegSketch->Create();

	auto X_ax = Screw.AxHoleRad;
	auto Y1_ax = Screw.HexDepth;
	auto Y2_ax = Screw.FullHeight;
	auto X_out = X_ax + Screw.LegThick;
	auto Y1_out = Y1_ax + Screw.GasketHeight;
	auto X_in = X_ax + Screw.GasketWidth;

	p2DDoc = pLegSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Y1_ax, 0, Y2_ax, 1);
	p2DDoc->ksLineSeg(0, Y2_ax, X_out-1, Y2_ax, 1); //-1 ��� �������
	p2DDoc->ksLineSeg(X_out - 1, Y2_ax, X_out, Y2_ax-1, 1);
	p2DDoc->ksLineSeg(X_out, Y2_ax-1, X_out, Y1_out, 1);
	p2DDoc->ksLineSeg(X_out, Y1_out, X_in, Y1_out, 1);
	p2DDoc->ksLineSeg(X_in, Y1_out, X_in, Y1_ax, 1);
	p2DDoc->ksLineSeg(X_in, Y1_ax, 0, Y1_ax, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3);
	
	pLegSketchDef->EndEdit();

	//�������� Leg
	ksEntityPtr pRotate = pPart->NewEntity(o3d_bossRotated);
	ksBossRotatedDefinitionPtr pRotDef = pRotate->GetDefinition();
	pRotDef->SetSketch(pLegSketch);
	pRotDef->SetSideParam(FALSE, 360);
	//ksEntityCollectionPtr fl = pRotDef->GetSketch(); //��������� ��� �����
	pRotate->Create();

	//����� ����� Leg
	ksEntityPtr pLegSketch1 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pLegSketchDef1 = pLegSketch1->GetDefinition();
	pLegSketchDef1->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pLegSketch1->Create();
	p2DDoc = pLegSketchDef1->BeginEdit();
	p2DDoc->ksLineSeg(0, 0, 0, Y2_ax, 1);
	p2DDoc->ksLineSeg(0, Y2_ax, X_ax, Y2_ax, 1);
	p2DDoc->ksLineSeg(X_ax, Y2_ax, X_ax, 1, 1);
	p2DDoc->ksLineSeg(X_ax, 1, X_ax+1, 0, 1); //+1 ��� �������
	p2DDoc->ksLineSeg(X_ax+1, 0, 0,0, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); 

	pLegSketchDef1->EndEdit();

	//�������� ����� 
	ksEntityPtr pRotate2 = pPart->NewEntity(o3d_cutRotated);
	ksCutRotatedDefinitionPtr pRotDef2 = pRotate2->GetDefinition();
	pRotDef2->SetSketch(pLegSketch1);
	pRotDef2->SetSideParam(FALSE, 360);
	pRotate2->Create();

	//�������
	//ksEntityPtr pChamfer = pPart->NewEntity(o3d_chamfer);
	//ksChamferDefinitionPtr pChamferDef = pChamfer->GetDefinition();
	//pChamferDef->SetChamferParam(true, 1, 1);
	//fl = pChamferDef->array();
	//fl->Clear();
	//ksEntityCollectionPtr flEdges = pPart->EntityCollection(o3d_edge);
	//for (int i = 0; i < flEdges->GetCount(); i++) {
	//	ksEntityPtr ed = flEdges->GetByIndex(i);
	//	ksEdgeDefinitionPtr def = ed->GetDefinition();
	//	if (def->GetOwnerEntity() == pRotate2) { 
	//		if (def->IsCircle()) {
	//			fl->Add(ed);
	//		}
	//	}
	//}
	//pChamfer->Create();
	//fl->Clear();

	flFaces = pPart->EntityCollection(o3d_face);
	t = 0;
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pRotate) {
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				if (r == Screw.AxHoleRad + Screw.LegThick) {
					face->Putname("Cylinder4Assembly2");
					face->Update();
				}
			}
		}
	}


	pPart->SetAdvancedColor(RGB(0, 150, 0), 1, 1, 1, 1, 1, 0.5);
	pPart->Update();


	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "����� ��������";
	path += name + ".m3d";

	p3DDoc->fileName = _bstr_t(CString(name.c_str()));
	p3DDoc->SaveAs(_bstr_t(CString(path.c_str())));
}

void Assembler::CreatePuck()
{
	ksDocument3DPtr p3DDoc;
	CComPtr<IUnknown> pKompasAppUnk = nullptr;

	if (!pKompasApp5)
	{
		// �������� CLSID ��� ������
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// ��������� ���� �� ���������� ��������� ������
		//���� ���� �������� IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// ���������� ��������� ������ ����� ��� ��� ����������� ���
			// ����� �������� IUnknown ��� ������ ��� ����������� ���������� ������
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// �������� ��������� ���������� ������
		hRes = pKompasAppUnk->QueryInterface(__uuidof(KompasObject), (void**)&pKompasApp5);
		if (FAILED(hRes)) {
			return;
		}
	}

	pKompasApp5->Visible = true;

	p3DDoc = pKompasApp5->Document3D();
	p3DDoc->Create(false, true);
	p3DDoc = pKompasApp5->ActiveDocument3D();
	ksPartPtr pPart = p3DDoc->GetPart(pTop_Part);
	ksDocument2DPtr p2DDoc;

	auto Puck = GetPuck(1);

	//�����
	ksEntityPtr pSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pSketchDef = pSketch->GetDefinition();
	pSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pSketch->Create();

	auto X1 = Puck.RadIn;
	auto X2 = Puck.RadOut;
	auto Y = Puck.Height;

	p2DDoc = pSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(X1, 0, X1, Y, 1);
	p2DDoc->ksLineSeg(X1, Y, X2, Y, 1);
	p2DDoc->ksLineSeg(X2, Y, X2, 0, 1);
	p2DDoc->ksLineSeg(X2, 0, X1, 0, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); 
	pSketchDef->EndEdit();

	//��������
	ksEntityPtr pRot = pPart->NewEntity(o3d_bossRotated);
	ksBossRotatedDefinitionPtr pRotDef = pRot->GetDefinition();
	pRotDef->SetSketch(pSketch);
	pRotDef->SetSideParam(FALSE, 360);
	pRot->Create();

	pPart->SetAdvancedColor(RGB(200, 150, 30), 1, 1, 1, 1, 1, 0.5);
	pPart->SetMaterial(L"���-26 ���� 18376-79", 1.83); //����������
	pPart->Update();


	//�������� ������
	ksEntityPtr pTHread = pPart->NewEntity(o3d_thread);
	ksThreadDefinitionPtr pTHreadDef = pTHread->GetDefinition();
	pTHreadDef->length = 5;
	pTHreadDef->dr = 8;
	pTHreadDef->faceValue = true;
	pTHreadDef->p = 1;
	ksEntityCollectionPtr Collection = pPart->EntityCollection(o3d_face);
	Collection->SelectByPoint(X1, 0.5, 0);
	pTHreadDef->SetBaseObject(Collection->First());
	pTHread->Create();
	Collection->Clear();


	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pRot) {
			if (def->IsPlanar()) {
				face->Putname("Face4Assembly3");
				face->Update();
				//break;
			}
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				if (r == Puck.RadIn) {
					face->Putname("Cylinder4Assembly3");
					face->Update();
				}
			}
		}
	}


	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "�����";
	path += name + ".m3d";

	p3DDoc->fileName = _bstr_t(CString(name.c_str()));
	p3DDoc->SaveAs(_bstr_t(CString(path.c_str())));
}

void Assembler::ass()
{
	ksDocument3DPtr p3DDoc;

	CComPtr<IUnknown> pKompasAppUnk = nullptr;

	if (!pKompasApp5)
	{
		// �������� CLSID ��� ������
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// ��������� ���� �� ���������� ��������� ������
		//���� ���� �������� IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// ���������� ��������� ������ ����� ��� ��� ����������� ���
			// ����� �������� IUnknown ��� ������ ��� ����������� ���������� ������
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// �������� ��������� ���������� ������
		hRes = pKompasAppUnk->QueryInterface(__uuidof(KompasObject), (void**)&pKompasApp5);
		if (FAILED(hRes)) {
			return;
		}
	}

	pKompasApp5->Visible = true;

	p3DDoc = pKompasApp5->Document3D();
	p3DDoc->Create(false, false);
	p3DDoc = pKompasApp5->ActiveDocument3D();
	ksPartPtr pAssemble = p3DDoc->GetPart(pTop_Part);
	ksDocument2DPtr p2DDoc;

	ksPartPtr pSeal, pScrew, pPuck;
	p3DDoc->SetPartFromFile("C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\������ ��������.m3d", pAssemble, true);
	p3DDoc->SetPartFromFile("C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\����� ��������.m3d", pAssemble, true);
	p3DDoc->SetPartFromFile("C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\�����.m3d", pAssemble, true);

	pSeal = p3DDoc->GetPart(0);
	pScrew = p3DDoc->GetPart(1);
	pPuck = p3DDoc->GetPart(2);

	ksEntityCollectionPtr colSeal = pSeal->EntityCollection(o3d_face);
	ksEntityCollectionPtr colScrew = pScrew->EntityCollection(o3d_face);
	ksEntityCollectionPtr colPuck = pPuck->EntityCollection(o3d_face);

	ksEntityPtr pSealPlaneZY = pSeal->GetDefaultEntity(o3d_planeYOZ);
	ksEntityPtr pAssemblePlaneZY = pAssemble->GetDefaultEntity(o3d_planeYOZ);
	
	ksEntityPtr BossFace4Assemly0 = colSeal->GetByName("Face4Assembly0", true, true);
	ksEntityPtr BossFace4Assemly1 = colSeal->GetByName("Face4Assembly1", true, true);
	ksEntityPtr Cylinder4Assembly1 = colSeal->GetByName("Cylinder4Assembly1", true, true);

	ksEntityPtr BossFace4Assemly2 = colScrew->GetByName("Face4Assembly2", true, true);
	ksEntityPtr BossFace4Assemly21 = colScrew->GetByName("Face4Assembly21", true, true);
	ksEntityPtr Cylinder4Assembly2 = colScrew->GetByName("Cylinder4Assembly2", true, true);
	
	ksEntityPtr BossFace4Assemly3 = colPuck->GetByName("Face4Assembly3", true, true);
	ksEntityPtr Cylinder4Assembly3 = colPuck->GetByName("Cylinder4Assembly3", true, true);

	ksPlacementPtr pl = pSeal->GetPlacement();
	pl->SetOrigin(0, 0, 0);
	pl->SetAxes(1, 0, 0, 0, 1, 0);
	pSeal->SetPlacement(pl);
	pSeal->UpdatePlacement();

	p3DDoc->AddMateConstraint(mc_Coincidence, BossFace4Assemly1, BossFace4Assemly3, -1, 1, 0);
	p3DDoc->AddMateConstraint(mc_Concentric, Cylinder4Assembly1, Cylinder4Assembly3, 0, 1, 0);

	p3DDoc->AddMateConstraint(mc_Coincidence, BossFace4Assemly0, BossFace4Assemly2, -1, 1, 0);
	p3DDoc->AddMateConstraint(mc_Concentric, Cylinder4Assembly1, Cylinder4Assembly2, 0, 1, 0);

	p3DDoc->AddMateConstraint(mc_Parallel, BossFace4Assemly21, pSealPlaneZY, 0, 1, 0);
	p3DDoc->AddMateConstraint(mc_Parallel, pAssemblePlaneZY, pSealPlaneZY, 0, 1, 0);
	
	p3DDoc->RebuildDocument();
	//pBoss->PutfixedComponent(TRUE);

	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "������";
	path += name + ".a3d";

	p3DDoc->fileName = _bstr_t(CString(name.c_str()));
	p3DDoc->SaveAs(_bstr_t(CString(path.c_str())));
}
