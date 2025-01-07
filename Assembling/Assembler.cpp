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
	switch (type) {
	case 1:
		//l = 18
		m_Seal.BaseRad = 46 / 2;
		m_Seal.BaseAngleBig = 65 * M_PI / 180;
		m_Seal.BaseAngleSmall = 55 * M_PI / 180;
		m_Seal.BaseDepth = 3;
		
		m_Seal.HexRad = 21.9 / 2;
		m_Seal.HexDepth = 15;

		m_Seal.AxHoleRad = 6;
		m_Seal.AxHoleThruRad = 4;

		m_Seal.X_Pin = 34/2;

		m_Seal.ThreadDR = 15;
		m_Seal.ThreadP = 1;
		break;
	case 2:
		//l = 26
		m_Seal.BaseRad = 60 / 2;
		m_Seal.BaseAngleBig = 65 * M_PI / 180;
		m_Seal.BaseAngleSmall = 55 * M_PI / 180;
		m_Seal.BaseDepth = 5;
		
		m_Seal.HexRad = 25.4 / 2;
		m_Seal.HexDepth = 19;

		m_Seal.AxHoleRad = 8;
		m_Seal.AxHoleThruRad = 5;

		m_Seal.X_Pin = 46/2;

		m_Seal.ThreadDR = 18;
		m_Seal.ThreadP = 1.5;
		break;
	case 3:
		//l = 35
		m_Seal.BaseRad = 78 / 2;
		m_Seal.BaseAngleBig = 65 * M_PI / 180;
		m_Seal.BaseAngleSmall = 55 * M_PI / 180;
		m_Seal.BaseDepth = 7;
		
		m_Seal.HexRad = 36.9 / 2;
		m_Seal.HexDepth = 25;

		m_Seal.AxHoleRad = 11;
		m_Seal.AxHoleThruRad = 6;

		m_Seal.X_Pin = 62/2;

		m_Seal.ThreadDR = 26;
		m_Seal.ThreadP = 1.5;
		break;
	}

	m_Seal.PinRadBig = 8 / 2;
	m_Seal.PinDepth = 2;
	m_Seal.PinRadSmall = 4.5 / 2;

	return m_Seal;
}

ScrewData Assembler::GetScrew(int type)
{
	switch (type) {
	case 1:
		m_Screw.LegThick = 2.5;

		m_Screw.FullHeight = 17;
		m_Screw.HexDepth = 3;
		//LegHeight=FullHeight-HexDepth

		m_Screw.HexRad = 21.9 / 2;
		m_Screw.AxHoleRad = 5;

		m_Screw.GasketHeight = 1;
		m_Screw.GasketWidth = 1;

		m_Screw.ThreadDR = 13;
		m_Screw.ThreadP = 1;
		break;
	case 2:
		m_Screw.FullHeight = 23;
		m_Screw.LegThick = 2;

		m_Screw.HexDepth = 5;
		m_Screw.HexRad = 25.4 / 2;

		m_Screw.AxHoleRad = 7;

		m_Screw.GasketHeight = 2;
		m_Screw.GasketWidth = 1;

		m_Screw.ThreadDR = 17;
		m_Screw.ThreadP = 1;
		break;
	case 3:
		m_Screw.FullHeight = 31;
		m_Screw.LegThick = 4;

		m_Screw.HexDepth = 7;
		m_Screw.HexRad = 36.9 / 2;

		m_Screw.AxHoleRad = 9;

		m_Screw.GasketHeight = 2;
		m_Screw.GasketWidth = 2;

		m_Screw.ThreadDR = 23;
		m_Screw.ThreadP = 1;
		break;
	}
	return m_Screw;
}

PuckData Assembler::GetPuck(int type)
{
	auto puck = PuckData();
	switch (type) {
	case 1:
		puck.RadIn = 3.2;
		puck.RadOut = 6;
		puck.Height = 1;
		break;
	case 2:
		puck.RadIn = 4.5;
		puck.RadOut = 8.05;
		puck.Height = 1;
		break;
	case 3:
		puck.RadIn = 5.5;
		puck.RadOut = 11.05;
		puck.Height = 1;
		break;
	}
	return puck;
}

void Assembler::FillAssembler(const int execution)
{
	m_currExec = execution;
	m_Seal = GetSeal(execution);
	m_Screw = GetScrew(execution);
	m_Puck = GetPuck(execution);
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

	//����� Base
	ksEntityPtr pBaseSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pBaseSketchDef = pBaseSketch->GetDefinition();
	pBaseSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pBaseSketch->Create();
	p2DDoc = pBaseSketchDef->BeginEdit();

	auto cosBig = cos(m_Seal.BaseAngleBig);
	auto sinBig = sin(m_Seal.BaseAngleBig);
	auto cosSmall = cos(m_Seal.BaseAngleSmall);
	auto sinSmall = sin(m_Seal.BaseAngleSmall);

	auto X1 = m_Seal.BaseRad * cosBig;
	auto Y1 = m_Seal.BaseRad * sinBig;
	auto X2 = m_Seal.BaseRad * cosSmall;
	auto Y2 = m_Seal.BaseRad * sinSmall;
	auto X3 = m_Seal.BaseRad * cos(m_Seal.BaseAngleSmall+120*M_PI/180);
	auto Y3 = m_Seal.BaseRad * sin(m_Seal.BaseAngleSmall+120*M_PI/180);

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
	pBaseBossExtrusionDef->SetSideParam(true, 0, m_Seal.BaseDepth, 0, false);
	pBaseExtrude->Create();

	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	auto counterEdges = 0;
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pBaseExtrude) {
			if (def->IsPlanar()) {
				ksEdgeCollectionPtr col = def->EdgeCollection();
				int amountEdges = col->GetCount();
				for (int k = 0; k < amountEdges; k++) {
					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);

					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);
					if (z1 == float(m_Seal.HexDepth) && amountEdges == 6) {
						counterEdges++;
					}
				}
				if (counterEdges == 6) {
					face->Putname("Face4Assembly1");
					face->Update();
					counterEdges = 0;
					break;
				}
			}
		}
	}

	//��������� ���������
	ksEntityPtr pBasePlane = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pBaseOffsetPlane = pBasePlane->GetDefinition();
	pBaseOffsetPlane->direction = true;
	pBaseOffsetPlane->offset = m_Seal.BaseDepth;
	pBaseOffsetPlane->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pBasePlane->Create();

	//����� Hex
	ksEntityPtr pHexSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pHexSketchDef = pHexSketch->GetDefinition();
	pHexSketchDef->SetPlane(pBasePlane);
	pHexSketch->Create();

	auto X_hex2 = m_Seal.HexRad * cos(30*M_PI/180);
	auto Y_hex2 = m_Seal.HexRad / 2;

	p2DDoc = pHexSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, m_Seal.HexRad, X_hex2, Y_hex2, 1);
	p2DDoc->ksLineSeg(X_hex2, Y_hex2, X_hex2, -Y_hex2, 1);
	p2DDoc->ksLineSeg(X_hex2, -Y_hex2, 0, -m_Seal.HexRad, 1);
	p2DDoc->ksLineSeg(0, -m_Seal.HexRad, -X_hex2, -Y_hex2, 1);
	p2DDoc->ksLineSeg(-X_hex2, -Y_hex2, -X_hex2, Y_hex2, 1);
	p2DDoc->ksLineSeg(-X_hex2, Y_hex2, 0, m_Seal.HexRad, 1);

	pHexSketchDef->EndEdit();

	//����� ��������� ����������� Groove
	ksEntityPtr pGrooveSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pGrooveSketchDef = pGrooveSketch->GetDefinition();
	pGrooveSketchDef->SetPlane(pBasePlane);
	pGrooveSketch->Create();

	p2DDoc = pGrooveSketchDef->BeginEdit();
	p2DDoc->ksCircle(-m_Seal.X_Pin, 0, m_Seal.PinRadBig, 1);

	pGrooveSketchDef->EndEdit();

	//����� ��������� ��������� Pin
	ksEntityPtr pPinSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pPinSketchDef = pPinSketch->GetDefinition();
	pPinSketchDef->SetPlane(pBasePlane);
	pPinSketch->Create();

	p2DDoc = pPinSketchDef->BeginEdit();
	p2DDoc->ksCircle(-m_Seal.X_Pin, 0, m_Seal.PinRadSmall, 1);

	pPinSketchDef->EndEdit();

	//������������ Hex
	ksEntityPtr pHexExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHexBossExtrusionDef = pHexExtrude->GetDefinition();
	pHexBossExtrusionDef->directionType = dtNormal;
	pHexBossExtrusionDef->SetSketch(pHexSketch);
	pHexBossExtrusionDef->SetSideParam(true, 0, m_Seal.HexDepth, 0, false);
	pHexExtrude->Create();

	//��������� Groove
	ksEntityPtr pGrooveCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pGrooveCutExtrudeDef = pGrooveCutExtrude->GetDefinition();
	pGrooveCutExtrudeDef->directionType = dtNormal;
	pGrooveCutExtrudeDef->SetSketch(pGrooveSketchDef);
	pGrooveCutExtrudeDef->SetSideParam(true, etBlind, m_Seal.PinDepth, 0, false);
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
	pTopPlaneDef->offset = m_Seal.BaseDepth + m_Seal.HexDepth;
	pTopPlaneDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pTopPlane->Create();

	//����� ������� ��������� 
	ksEntityPtr pAxialHoleSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pAxialHoleSketchDef = pAxialHoleSketch->GetDefinition();
	pAxialHoleSketchDef->SetPlane(pTopPlane);
	pAxialHoleSketch->Create();
	p2DDoc = pAxialHoleSketchDef->BeginEdit();
	p2DDoc->ksCircle(0, 0, m_Seal.AxHoleRad, 1);
	pAxialHoleSketchDef->EndEdit();

	//��������� ������� ���������
	ksEntityPtr pAxialHoleCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pAxialHoleCutExtrudeDef = pAxialHoleCutExtrude->GetDefinition();
	pAxialHoleCutExtrudeDef->directionType = dtNormal;
	pAxialHoleCutExtrudeDef->SetSketch(pAxialHoleSketchDef);
	pAxialHoleCutExtrudeDef->SetSideParam(true, etBlind, m_Seal.HexDepth, 0, false);
	pAxialHoleCutExtrude->Create();

	//����� ������� ��������� ������
	ksEntityPtr pAxialHoleSketch2 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pAxialHoleSketchDef2 = pAxialHoleSketch2->GetDefinition();
	pAxialHoleSketchDef2->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pAxialHoleSketch2->Create();
	p2DDoc = pAxialHoleSketchDef2->BeginEdit();
	p2DDoc->ksCircle(0, 0, m_Seal.AxHoleThruRad, 1);
	pAxialHoleSketchDef2->EndEdit();

	//��������� ������� ���������
	ksEntityPtr pAxialHoleCutExtrude2 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pAxialHoleCutExtrudeDef2 = pAxialHoleCutExtrude2->GetDefinition();
	pAxialHoleCutExtrudeDef2->directionType = dtReverse;
	pAxialHoleCutExtrudeDef2->SetSketch(pAxialHoleSketchDef2);
	pAxialHoleCutExtrudeDef2->SetSideParam(true, etBlind, m_Seal.BaseDepth, 0, false);
	pAxialHoleCutExtrude2->Create();

	flFaces = pPart->EntityCollection(o3d_face);
	counterEdges = 0;
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pAxialHoleCutExtrude) {
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				if (r == m_Seal.AxHoleRad) {
					face->Putname("Cylinder4Assembly1");
					face->Update();
				}
			}
		}
		if (def->GetOwnerEntity() == pHexExtrude) {
			if (def->IsPlanar()) {
				ksEdgeCollectionPtr col = def->EdgeCollection(); 
				int amountEdges = col->GetCount();
				for (int k = 0; k < amountEdges; k++) {
					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);

					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);

					if (z1 == float(m_Seal.BaseDepth + m_Seal.HexDepth) && amountEdges == 7) {
						counterEdges++;
					}
				}
				if (counterEdges == 7) {
					face->Putname("Face4Assembly0");
					face->Update();
					counterEdges = 0;
				}
			}
		}
	}

	//������
	ksEntityPtr pTHread = pPart->NewEntity(o3d_thread);
	ksThreadDefinitionPtr pTHreadDef = pTHread->GetDefinition();
	pTHreadDef->Putlength(m_Seal.HexDepth - m_Puck.Height );//( = 14) in 1 exec
	ksEntityPtr BossFace4Assemly0 = flFaces->GetByName("Face4Assembly0", true, true);
	ksEntityPtr Cylinder4Assembly1 = flFaces->GetByName("Cylinder4Assembly1", true, true);
	pTHreadDef->SetFaceBegin(BossFace4Assemly0);
	pTHreadDef->dr = m_Seal.ThreadDR;// 15;
	pTHreadDef->p = m_Seal.ThreadP; //1
	pTHreadDef->SetBaseObject(Cylinder4Assembly1);
	pTHread->Create();

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
	
	//�������� ���
	auto X_hex = m_Screw.HexRad * cos(30 * M_PI / 180);
	auto Y_hex = m_Screw.HexRad / 2;

	p2DDoc = pHexSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, m_Screw.HexRad, X_hex, Y_hex, 1);
	p2DDoc->ksLineSeg(X_hex, Y_hex, X_hex, -Y_hex, 1);
	p2DDoc->ksLineSeg(X_hex, -Y_hex, 0, -m_Screw.HexRad, 1);
	p2DDoc->ksLineSeg(0, -m_Screw.HexRad, -X_hex, -Y_hex, 1);
	p2DDoc->ksLineSeg(-X_hex, -Y_hex, -X_hex, Y_hex, 1);
	p2DDoc->ksLineSeg(-X_hex, Y_hex, 0, m_Screw.HexRad, 1);

	pHexSketchDef->EndEdit();

	//������������ Hexagon
	ksEntityPtr pHexExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHexExtrusionDef = pHexExtrude->GetDefinition();
	pHexExtrusionDef->directionType = dtNormal;
	pHexExtrusionDef->SetSketch(pHexSketch);
	pHexExtrusionDef->SetSideParam(true, 0, m_Screw.HexDepth, 0, false);
	pHexExtrude->Create();
	
	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	auto counterEdges = 0;
	auto isFirst = true;
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pHexExtrude) {
			if (def->IsPlanar()) {
				ksEdgeCollectionPtr col = def->EdgeCollection();
				auto amountEdges = col->GetCount();
				for (int k = 0; k < amountEdges; k++) {
					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);
					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);
					if (round(y1) == m_Screw.HexDepth && amountEdges == 6) {
						counterEdges++;
					}
					if (amountEdges == 4 && isFirst == true) {
						face->Putname("Face4Assembly21");
						face->Update();
						isFirst = false;
					}
				}
				if (counterEdges == 6) {
					face->Putname("Face4Assembly2");
					face->Update();
					counterEdges = 0;
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

	auto Y1_ax = m_Screw.HexDepth;
	auto Y2_ax = m_Screw.FullHeight;
	auto X_out = m_Screw.AxHoleRad + m_Screw.LegThick;
	auto Y_out = m_Screw.HexDepth + m_Screw.GasketHeight;
	auto X_in = m_Screw.AxHoleRad + m_Screw.GasketWidth;

	p2DDoc = pLegSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Y1_ax, 0, Y2_ax, 1);
	//p2DDoc->ksLineSeg(0, Y2_ax, X_out-1, Y2_ax, 1); //-1 ��� �������
	//p2DDoc->ksLineSeg(X_out - 1, Y2_ax, X_out, Y2_ax-1, 1);
	p2DDoc->ksLineSeg(0, Y2_ax, X_out, Y2_ax, 1);
	p2DDoc->ksLineSeg(X_out, Y2_ax, X_out, Y_out, 1);
	p2DDoc->ksLineSeg(X_out, Y_out, X_in, Y_out, 1);
	p2DDoc->ksLineSeg(X_in, Y_out, X_in, Y1_ax, 1);
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
	auto X_ax = m_Screw.AxHoleRad;
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

	//������
	ksEntityPtr pTHread = pPart->NewEntity(o3d_thread);
	ksThreadDefinitionPtr pTHreadDef = pTHread->GetDefinition();
	pTHreadDef->PutallLength(TRUE);
	pTHreadDef->dr = m_Screw.ThreadDR;
	pTHreadDef->p = m_Screw.ThreadP;

	flFaces = pPart->EntityCollection(o3d_face);
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pRotate) {
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				if (r == m_Screw.AxHoleRad + m_Screw.LegThick) {
					face->Putname("Cylinder4Assembly2");
					face->Update();

					pTHreadDef->SetBaseObject(face);
					pTHread->Create();
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

	//�����
	ksEntityPtr pSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pSketchDef = pSketch->GetDefinition();
	pSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pSketch->Create();

	auto X1 = m_Puck.RadIn;
	auto X2 = m_Puck.RadOut;
	auto Y = m_Puck.Height;

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

	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pRot) {
			if (def->IsPlanar()) {
				face->Putname("Face4Assembly3");
				face->Update();
				continue;
			}
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				if (r == m_Puck.RadIn) {
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

void Assembler::MakeAssemble()
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

	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "������";
	path += name + ".a3d";

	p3DDoc->fileName = _bstr_t(CString(name.c_str()));
	p3DDoc->SaveAs(_bstr_t(CString(path.c_str())));
}

void Assembler::GodAssemble(float RBase, float RHole, float L)
{
	if (RHole * sqrt(3) / 2 >= sin(35 / M_PI * 180) * RBase) {
		CString str;
		str.Format(L"R2 cant be more than %f", sin(35 * M_PI / 180) * RBase);
		AfxMessageBox(str);
		return;
	}

	m_Seal.BaseRad = RBase;
	m_Seal.AxHoleThruRad = RHole / 2;

	m_Seal.HexDepth = L;
	m_Screw.FullHeight = L - 1 + m_Screw.HexDepth;

	m_Seal.ThreadDR = 2*(RHole + m_Screw.LegThick - m_Screw.GasketWidth);

	m_Seal.X_Pin = RBase * 0.75;
	m_Seal.PinRadBig = RBase * 0.15;
	m_Seal.PinRadSmall = m_Seal.PinRadBig * 0.44;
	m_Seal.AxHoleRad = RHole;
	m_Seal.HexRad = (RHole / sqrt(3) * 2) * 1.5;

	m_Screw.AxHoleRad = RHole - m_Screw.GasketWidth;
	m_Screw.ThreadDR = 2*RHole + m_Screw.GasketWidth;
	//m_Screw.ThreadDR = 2 * (m_Screw.AxHoleRad + m_Screw.GasketWidth);
	m_Screw.HexRad = m_Seal.HexRad;
	m_Puck.RadOut = RHole + 0.1;
	m_Puck.RadIn = m_Seal.AxHoleThruRad * 0.9;

	CreateSeal();
	CreateScrew();
	CreatePuck();
	MakeAssemble();
}

void Assembler::CloseAll()
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

	p3DDoc->Create(false, true);
	p3DDoc = pKompasApp5->ActiveDocument3D();
	ksPartPtr pPart = p3DDoc->GetPart(pTop_Part);
	ksDocument2DPtr p2DDoc;
	
	//https://help.ascon.ru/KOMPAS_SDK/22/ru-RU/iapplication_documents.html
}
