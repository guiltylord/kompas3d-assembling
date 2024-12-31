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
		seal.HBRad = 23;
		seal.HBAngleBig = 65 * M_PI / 180;
		seal.HBAngleSmall = 55 * M_PI / 180;
		seal.HBDepth = 3;
		
		seal.HTRad = 21.9 / 2;
		seal.HTDepth = 15;

		seal.X_Pin = 34/2;
		seal.GrooveRad = 8 / 2;
		seal.PinRad = 4.5 / 2;

		seal.AxHoleRad = 16 / 2;
		seal.AxHoleThruRad = 8 / 2;
	}

	return seal;
}

ScrewData Assembler::GetScrew(int type)
{
	auto screw = ScrewData();
	switch (type) {
	case 1:
		screw.LegHeight = 15;
		screw.LegThick = 3;

		screw.HexDepth = 3;
		screw.HexRad = 21.9 / 2;

		screw.AxHoleRad = 5;

		screw.GasketHeight = 3 / 2;
		screw.GasketWidth = 3 / 2;
	}

	return screw;
}

PuckData Assembler::GetPuck(int type)
{
	auto puck = PuckData();
	switch (type) {
	case 1:
		puck.Rad = 5;
		puck.Height = 1;
		puck.Width = 2.05;
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

	//����� ��� Hexagon Bottom
	ksEntityPtr pHBSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pHBSketchDef = pHBSketch->GetDefinition();
	pHBSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pHBSketch->Create();
	p2DDoc = pHBSketchDef->BeginEdit();

	SealData Seal = GetSeal(1);

	auto cosBig = cos(Seal.HBAngleBig);
	auto sinBig = sin(Seal.HBAngleBig);
	auto cosSmall = cos(Seal.HBAngleSmall);
	auto sinSmall = sin(Seal.HBAngleSmall);

	auto X1 = Seal.HBRad * cosBig;
	auto Y1 = Seal.HBRad * sinBig;
	auto X2 = Seal.HBRad * cosSmall;
	auto Y2 = Seal.HBRad * sinSmall;
	auto X3 = Seal.HBRad * cos(Seal.HBAngleSmall+120*M_PI/180);
	auto Y3 = Seal.HBRad * sin(Seal.HBAngleSmall+120*M_PI/180);

	p2DDoc->ksLineSeg(X1, Y1, X2, Y2, 1);
	p2DDoc->ksLineSeg(X2, Y2, X2, -Y2, 1);
	p2DDoc->ksLineSeg(X2, -Y2, X1, -Y1, 1);
	p2DDoc->ksLineSeg(X1, -Y1, X3, -Y3, 1);
	p2DDoc->ksLineSeg(X3, -Y3, X3, Y3, 1);
	p2DDoc->ksLineSeg(X3, Y3, X1, Y1, 1);

	pHBSketchDef->EndEdit();

	//������������ HB
	ksEntityPtr pHBExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHBBossExtrusionDef = pHBExtrude->GetDefinition();
	pHBBossExtrusionDef->directionType = dtNormal;
	pHBBossExtrusionDef->SetSketch(pHBSketch);
	pHBBossExtrusionDef->SetSideParam(true, 0, Seal.HBDepth, 0, false);
	pHBExtrude->Create();

	//��������� ���������
	ksEntityPtr pHBPlane = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pHBOffsetPlane = pHBPlane->GetDefinition();
	pHBOffsetPlane->direction = true;
	pHBOffsetPlane->offset = 3;
	pHBOffsetPlane->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pHBPlane->Create();

	//����� Hexagon Top
	ksEntityPtr pHTSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pHTSketchDef = pHTSketch->GetDefinition();
	pHTSketchDef->SetPlane(pHBPlane);
	pHTSketch->Create();

	auto X_hex2 = Seal.HTRad * cos(30*M_PI/180);
	auto Y_hex2 = Seal.HTRad / 2;

	p2DDoc = pHTSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Seal.HTRad, X_hex2, Y_hex2, 1);
	p2DDoc->ksLineSeg(X_hex2, Y_hex2, X_hex2, -Y_hex2, 1);
	p2DDoc->ksLineSeg(X_hex2, -Y_hex2, 0, -Seal.HTRad, 1);
	p2DDoc->ksLineSeg(0, -Seal.HTRad, -X_hex2, -Y_hex2, 1);
	p2DDoc->ksLineSeg(-X_hex2, -Y_hex2, -X_hex2, Y_hex2, 1);
	p2DDoc->ksLineSeg(-X_hex2, Y_hex2, 0, Seal.HTRad, 1);

	pHTSketchDef->EndEdit();

	//����� ��������� ����������� Groove
	ksEntityPtr pGrooveSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pGrooveSketchDef = pGrooveSketch->GetDefinition();
	pGrooveSketchDef->SetPlane(pHBPlane);
	pGrooveSketch->Create();

	p2DDoc = pGrooveSketchDef->BeginEdit();
	p2DDoc->ksCircle(-Seal.X_Pin, 0, Seal.GrooveRad, 1);

	pGrooveSketchDef->EndEdit();

	//����� ��������� ��������� Pin
	ksEntityPtr pPinSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pPinSketchDef = pPinSketch->GetDefinition();
	pPinSketchDef->SetPlane(pHBPlane);
	pPinSketch->Create();

	p2DDoc = pPinSketchDef->BeginEdit();
	p2DDoc->ksCircle(-Seal.X_Pin, 0, Seal.PinRad, 1);

	pPinSketchDef->EndEdit();

	//������������ HT
	ksEntityPtr pTopExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pTopBossExtrusionDef = pTopExtrude->GetDefinition();
	pTopBossExtrusionDef->directionType = dtNormal;
	pTopBossExtrusionDef->SetSketch(pHTSketch);
	pTopBossExtrusionDef->SetSideParam(true, 0, Seal.HTDepth, 0, false);
	pTopExtrude->Create();

	//��������� Groove
	ksEntityPtr pGrooveCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pGrooveCutExtrudeDef = pGrooveCutExtrude->GetDefinition();
	pGrooveCutExtrudeDef->directionType = dtNormal;
	pGrooveCutExtrudeDef->SetSketch(pGrooveSketchDef);
	pGrooveCutExtrudeDef->SetSideParam(true, etBlind, 2, 0, false);
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
	pTopPlaneDef->offset = Seal.HBDepth + Seal.HTDepth;
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
	pAxialHoleCutExtrudeDef->SetSideParam(true, etBlind, Seal.HTDepth, 0, false);
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
	pAxialHoleCutExtrudeDef2->SetSideParam(true, etBlind, Seal.HBDepth, 0, false);
	pAxialHoleCutExtrude2->Create();

	//p3DDoc->hideAllPlanes=true;
	
	//p3DDoc->hideInComponentsMode; //doesnt work
	
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

	//����� ��� Hexagon
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
	ksEntityPtr pHBExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHBBossExtrusionDef = pHBExtrude->GetDefinition();
	pHBBossExtrusionDef->directionType = dtNormal;
	pHBBossExtrusionDef->SetSketch(pHexSketch);
	pHBBossExtrusionDef->SetSideParam(true, 0, Screw.HexDepth, 0, false);
	pHBExtrude->Create();

	//� ����� ������ ����� ������� ����� �� �������� ����������. ������� ���� ���, ����� ������ ��� �������� �� ���� � ����� ����
	//����� Leg
	ksEntityPtr pLegSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pLegSketchDef = pLegSketch->GetDefinition();
	pLegSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pLegSketch->Create();

	auto X_ax = (Screw.AxHoleRad);
	auto Y1_ax = (Screw.HexDepth);
	auto Y2_ax = (Screw.LegHeight);
	auto X_out = (X_ax + Screw.LegThick);
	auto Y1_out = (Y1_ax + Screw.GasketHeight);
	auto X_in = (X_ax + Screw.GasketWidth);

	p2DDoc = pLegSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Y1_ax, 0, Y2_ax, 1);
	p2DDoc->ksLineSeg(0, Y2_ax, X_out-1, Y2_ax, 1); //-1 ��� �������
	p2DDoc->ksLineSeg(X_out - 1, Y2_ax, X_out, Y2_ax-1, 1);
	p2DDoc->ksLineSeg(X_out, Y2_ax-1, X_out, Y1_out, 1);
	p2DDoc->ksLineSeg(X_out, Y1_out, X_in, Y1_out, 1);
	p2DDoc->ksLineSeg(X_in, Y1_out, X_in, Y1_ax, 1);
	p2DDoc->ksLineSeg(X_in, Y1_ax, 0, Y1_ax, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); //������ ��� ���� ��������((((
	
	pLegSketchDef->EndEdit();


	//�������� Leg � ���� ��� ����? � ��� �������? �� ��� ���� ��������????
	ksEntityPtr pRotate = pPart->NewEntity(o3d_bossRotated);
	ksBossRotatedDefinitionPtr pRotDef = pRotate->GetDefinition();
	pRotDef->SetSketch(pLegSketch);
	pRotDef->SetSideParam(FALSE, 360);
	//ksEntityCollectionPtr fl = pRotDef->GetSketch(); //��������� ��� �����
	pRotate->Create();

	//����� ����� Leg //�� ��� ������� �����������. �� ���� ������� ����� �������� � ������ �� ��������, \
	� �� ����� ����� ��������  �������������� ���� ����. � �� ���� ����� �������������� �������. ���� ����������� � ������ ����� �����.
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

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); //������ ��� ���� ��������((((

	pLegSketchDef1->EndEdit();

	//�������� ����� 
	ksEntityPtr pRotate2 = pPart->NewEntity(o3d_cutRotated);
	ksCutRotatedDefinitionPtr pRotDef2 = pRotate2->GetDefinition();
	pRotDef2->SetSketch(pLegSketch1);
	pRotDef2->SetSideParam(FALSE, 360);
	ksEntityCollectionPtr fl = pRotDef2->GetSketch(); //��������� ��� �����
	pRotate2->Create();

	//�������)))
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

	auto X1 = Puck.Rad;
	auto X2 = Puck.Rad + Puck.Width;
	auto Y = Puck.Height;

	p2DDoc = pSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(X1, 0, X1, Y, 1);
	p2DDoc->ksLineSeg(X1, Y, X2, Y, 1);
	p2DDoc->ksLineSeg(X2, Y, X2, 0, 1);
	p2DDoc->ksLineSeg(X2, 0, X1, 0, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); //������ ��� ���� ��������((((

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

	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "�����";
	path += name + ".m3d";

	p3DDoc->fileName = _bstr_t(CString(name.c_str()));
	p3DDoc->SaveAs(_bstr_t(CString(path.c_str())));
}
