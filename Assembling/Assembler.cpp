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

		seal.AxHoleRad = 16 / 2;
		seal.AxHoleThruRad = 8 / 2;

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
		// Получаем CLSID для Компас
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// Проверяем есть ли запущенный экземпляр Компас
		//если есть получаем IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// Приходится запускать Компас самим так как работающего нет
			// Также получаем IUnknown для только что запущенного приложения Компас
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// Получаем интерфейс приложения Компас
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

	//эскиз дно Hexagon Bottom
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

	//выдавливание Base
	ksEntityPtr pBaseExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pBaseBossExtrusionDef = pBaseExtrude->GetDefinition();
	pBaseBossExtrusionDef->directionType = dtNormal;
	pBaseBossExtrusionDef->SetSketch(pBaseSketch);
	pBaseBossExtrusionDef->SetSideParam(true, 0, Seal.BaseDepth, 0, false);
	pBaseExtrude->Create();

	//смещенная плоскость
	ksEntityPtr pBasePlane = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pBaseOffsetPlane = pBasePlane->GetDefinition();
	pBaseOffsetPlane->direction = true;
	pBaseOffsetPlane->offset = Seal.BaseDepth;
	pBaseOffsetPlane->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pBasePlane->Create();

	//эскиз Hex
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

	//эскиз отверстия несквозного Groove
	ksEntityPtr pGrooveSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pGrooveSketchDef = pGrooveSketch->GetDefinition();
	pGrooveSketchDef->SetPlane(pBasePlane);
	pGrooveSketch->Create();

	p2DDoc = pGrooveSketchDef->BeginEdit();
	p2DDoc->ksCircle(-Seal.X_Pin, 0, Seal.GrooveRad, 1);

	pGrooveSketchDef->EndEdit();

	//эскиз отверстие сквозного Pin
	ksEntityPtr pPinSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pPinSketchDef = pPinSketch->GetDefinition();
	pPinSketchDef->SetPlane(pBasePlane);
	pPinSketch->Create();

	p2DDoc = pPinSketchDef->BeginEdit();
	p2DDoc->ksCircle(-Seal.X_Pin, 0, Seal.PinRad, 1);

	pPinSketchDef->EndEdit();

	//выдавливание Hex
	ksEntityPtr pHexExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pHexBossExtrusionDef = pHexExtrude->GetDefinition();
	pHexBossExtrusionDef->directionType = dtNormal;
	pHexBossExtrusionDef->SetSketch(pHexSketch);
	pHexBossExtrusionDef->SetSideParam(true, 0, Seal.HexDepth, 0, false);
	pHexExtrude->Create();

	//вырезание Groove
	ksEntityPtr pGrooveCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pGrooveCutExtrudeDef = pGrooveCutExtrude->GetDefinition();
	pGrooveCutExtrudeDef->directionType = dtNormal;
	pGrooveCutExtrudeDef->SetSketch(pGrooveSketchDef);
	pGrooveCutExtrudeDef->SetSideParam(true, etBlind, Seal.GrooveDepth, 0, false);
	pGrooveCutExtrude->Create();

	//вырезание Pin
	ksEntityPtr pPinCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pPinCutExtrudeDef = pPinCutExtrude->GetDefinition();
	pPinCutExtrudeDef->directionType = dtNormal;
	pPinCutExtrudeDef->SetSketch(pPinSketchDef);
	pPinCutExtrudeDef->SetSideParam(true, etThroughAll, 0, 0, false);
	pPinCutExtrude->Create();

	//массив Groove+Pin
	ksEntityPtr pCircCopy = pPart->NewEntity(o3d_circularCopy); 
	ksCircularCopyDefinitionPtr pCircCopyDef = pCircCopy->GetDefinition();

	pCircCopyDef->Putcount2(3);
	pCircCopyDef->SetAxis(pPart->GetDefaultEntity(o3d_axisOZ)); 

	ksEntityCollectionPtr pEntColl = pCircCopyDef->GetOperationArray();
	pEntColl->Clear();
	pEntColl->Add(pGrooveCutExtrude);
	pEntColl->Add(pPinCutExtrude);

	pCircCopy->Create();

	//смещенная плоскость для осевого отверстия
	ksEntityPtr pTopPlane = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pTopPlaneDef = pTopPlane->GetDefinition();
	pTopPlaneDef->direction = true;
	pTopPlaneDef->offset = Seal.BaseDepth + Seal.HexDepth;
	pTopPlaneDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pTopPlane->Create();

	//эскиз осевого отверстия 
	ksEntityPtr pAxialHoleSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pAxialHoleSketchDef = pAxialHoleSketch->GetDefinition();
	pAxialHoleSketchDef->SetPlane(pTopPlane);
	pAxialHoleSketch->Create();
	p2DDoc = pAxialHoleSketchDef->BeginEdit();
	p2DDoc->ksCircle(0, 0, Seal.AxHoleRad, 1);
	pAxialHoleSketchDef->EndEdit();

	//вырезание осевого отверстия
	ksEntityPtr pAxialHoleCutExtrude = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pAxialHoleCutExtrudeDef = pAxialHoleCutExtrude->GetDefinition();
	pAxialHoleCutExtrudeDef->directionType = dtNormal;
	pAxialHoleCutExtrudeDef->SetSketch(pAxialHoleSketchDef);
	pAxialHoleCutExtrudeDef->SetSideParam(true, etBlind, Seal.HexDepth, 0, false);
	pAxialHoleCutExtrude->Create();

	//эскиз осевого отверстия малого
	ksEntityPtr pAxialHoleSketch2 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pAxialHoleSketchDef2 = pAxialHoleSketch2->GetDefinition();
	pAxialHoleSketchDef2->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pAxialHoleSketch2->Create();
	p2DDoc = pAxialHoleSketchDef2->BeginEdit();
	p2DDoc->ksCircle(0, 0, Seal.AxHoleThruRad, 1);
	pAxialHoleSketchDef2->EndEdit();

	//вырезание осевого отверстия
	ksEntityPtr pAxialHoleCutExtrude2 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr pAxialHoleCutExtrudeDef2 = pAxialHoleCutExtrude2->GetDefinition();
	pAxialHoleCutExtrudeDef2->directionType = dtReverse;
	pAxialHoleCutExtrudeDef2->SetSketch(pAxialHoleSketchDef2);
	pAxialHoleCutExtrudeDef2->SetSideParam(true, etBlind, Seal.BaseDepth, 0, false);
	pAxialHoleCutExtrude2->Create();

	//p3DDoc->hideAllPlanes=true;	
	//p3DDoc->hideInComponentsMode; //doesnt work
	

	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	auto t = 0;
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pAxialHoleCutExtrude2) {
			if (def->IsCylinder()) {
				double h, r;
				def->GetCylinderParam(&h, &r);
				if (r == Seal.AxHoleRad / 2) {
					face->Putname("Cylinder4Assembly");
					face->Update();
				}
			}


		}
		if (def->GetOwnerEntity() == pHexExtrude) {
			if (def->IsPlanar()) {
				ksEdgeCollectionPtr col = def->EdgeCollection();
				for (int k = 0; k < col->GetCount(); k++) {

					ksEdgeDefinitionPtr d = col->GetByIndex(k);
					ksVertexDefinitionPtr p1 = d->GetVertex(true);
					ksVertexDefinitionPtr p2 = d->GetVertex(false);


					double x1, y1, z1;
					p1->GetPoint(&x1, &y1, &z1);

					double x2, y2, z2;
					p2->GetPoint(&x2, &y2, &z2);

					if (z1==(Seal.HexDepth+Seal.BaseDepth) && x1 == 0 && y1 == 10.95f) {
						t++;
						face->Putname("Face4Assembly0");
						face->Update();
						//break;
					}
				}
			}
		}
	}












	//операция сохранения детали
	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "Гнездо сальника";
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
		// Получаем CLSID для Компас
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// Проверяем есть ли запущенный экземпляр Компас
		//если есть получаем IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// Приходится запускать Компас самим так как работающего нет
			// Также получаем IUnknown для только что запущенного приложения Компас
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// Получаем интерфейс приложения Компас
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

	//эскиз дно Hexagon
	ksEntityPtr pHexSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pHexSketchDef = pHexSketch->GetDefinition();
	pHexSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOZ));
	pHexSketch->Create();
	
	ScrewData Screw = GetScrew(1);
	//описание вар
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

	//выдавливание Hexagon
	ksEntityPtr pBaseExtrude = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr pBaseBossExtrusionDef = pBaseExtrude->GetDefinition();
	pBaseBossExtrusionDef->directionType = dtNormal;
	pBaseBossExtrusionDef->SetSketch(pHexSketch);
	pBaseBossExtrusionDef->SetSideParam(true, 0, Screw.HexDepth, 0, false);
	pBaseExtrude->Create();

	//в эттом эскизе потом сделать смену на втторрое исполнение. добавиь флаг так, чтобы выемки под сальникк не было и фаски тоже
	//эскиз Leg
	ksEntityPtr pLegSketch = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pLegSketchDef = pLegSketch->GetDefinition();
	pLegSketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pLegSketch->Create();

	auto X_ax = Screw.AxHoleRad;
	auto Y1_ax = Screw.HexDepth;
	auto Y2_ax = Screw.LegHeight;
	auto X_out = X_ax + Screw.LegThick;
	auto Y1_out = Y1_ax + Screw.GasketHeight;
	auto X_in = X_ax + Screw.GasketWidth;

	p2DDoc = pLegSketchDef->BeginEdit();
	p2DDoc->ksLineSeg(0, Y1_ax, 0, Y2_ax, 1);
	p2DDoc->ksLineSeg(0, Y2_ax, X_out-1, Y2_ax, 1); //-1 для фасочки
	p2DDoc->ksLineSeg(X_out - 1, Y2_ax, X_out, Y2_ax-1, 1);
	p2DDoc->ksLineSeg(X_out, Y2_ax-1, X_out, Y1_out, 1);
	p2DDoc->ksLineSeg(X_out, Y1_out, X_in, Y1_out, 1);
	p2DDoc->ksLineSeg(X_in, Y1_out, X_in, Y1_ax, 1);
	p2DDoc->ksLineSeg(X_in, Y1_ax, 0, Y1_ax, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3);
	
	pLegSketchDef->EndEdit();

	//вращение Leg
	ksEntityPtr pRotate = pPart->NewEntity(o3d_bossRotated);
	ksBossRotatedDefinitionPtr pRotDef = pRotate->GetDefinition();
	pRotDef->SetSketch(pLegSketch);
	pRotDef->SetSideParam(FALSE, 360);
	//ksEntityCollectionPtr fl = pRotDef->GetSketch(); //коллекция для фасок
	pRotate->Create();

	//эскиз дырки Leg
	ksEntityPtr pLegSketch1 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr pLegSketchDef1 = pLegSketch1->GetDefinition();
	pLegSketchDef1->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pLegSketch1->Create();
	p2DDoc = pLegSketchDef1->BeginEdit();
	p2DDoc->ksLineSeg(0, 0, 0, Y2_ax, 1);
	p2DDoc->ksLineSeg(0, Y2_ax, X_ax, Y2_ax, 1);
	p2DDoc->ksLineSeg(X_ax, Y2_ax, X_ax, 1, 1);
	p2DDoc->ksLineSeg(X_ax, 1, X_ax+1, 0, 1); //+1 для фасочки
	p2DDoc->ksLineSeg(X_ax+1, 0, 0,0, 1);

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); 

	pLegSketchDef1->EndEdit();

	//вращение дырки 
	ksEntityPtr pRotate2 = pPart->NewEntity(o3d_cutRotated);
	ksCutRotatedDefinitionPtr pRotDef2 = pRotate2->GetDefinition();
	pRotDef2->SetSketch(pLegSketch1);
	pRotDef2->SetSideParam(FALSE, 360);
	pRotate2->Create();

	//фасочки
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
	string name = "Гайка нажимная";
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
		// Получаем CLSID для Компас
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// Проверяем есть ли запущенный экземпляр Компас
		//если есть получаем IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// Приходится запускать Компас самим так как работающего нет
			// Также получаем IUnknown для только что запущенного приложения Компас
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// Получаем интерфейс приложения Компас
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

	//эскиз
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

	p2DDoc->ksLineSeg(0, 10, 0, 0, 3); 
	pSketchDef->EndEdit();

	//вращение
	ksEntityPtr pRot = pPart->NewEntity(o3d_bossRotated);
	ksBossRotatedDefinitionPtr pRotDef = pRot->GetDefinition();
	pRotDef->SetSketch(pSketch);
	pRotDef->SetSideParam(FALSE, 360);
	pRot->Create();

	pPart->SetAdvancedColor(RGB(200, 150, 30), 1, 1, 1, 1, 1, 0.5);
	pPart->SetMaterial(L"СКФ-26 ГОСТ 18376-79", 1.83); //фторкаучук
	pPart->Update();







	ksEntityCollectionPtr flFaces = pPart->EntityCollection(o3d_face);
	for (int i = 0; i < flFaces->GetCount(); i++) {
		ksEntityPtr face = flFaces->GetByIndex(i);
		ksFaceDefinitionPtr def = face->GetDefinition();
		if (def->GetOwnerEntity() == pRot) {
			if (def->IsPlanar()) {
				face->Putname("Face4Assembly1");
				face->Update();
				//break;
			}
		}
	}



















	string path = "C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\";
	string name = "Шайба";
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
		// Получаем CLSID для Компас
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Kompas.Application.5", &InvAppClsid);
		if (FAILED(hRes)) {
			pKompasApp5 = nullptr;
			return;
		}

		// Проверяем есть ли запущенный экземпляр Компас
		//если есть получаем IUnknown
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pKompasAppUnk);
		if (FAILED(hRes)) {
			// Приходится запускать Компас самим так как работающего нет
			// Также получаем IUnknown для только что запущенного приложения Компас
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pKompasAppUnk);
			if (FAILED(hRes)) {
				pKompasApp5 = nullptr;
				return;
			}
		}

		// Получаем интерфейс приложения Компас
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

	ksPartPtr pBoss, pGear1, pGear2;
	p3DDoc->SetPartFromFile("C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\Гнездо сальника.m3d", pPart, true);
	p3DDoc->SetPartFromFile("C:\\Users\\desxz\\source\\repos\\Assembling\\Details\\Шайба.m3d", pPart, true);
	//p3DDoc->SetPartFromFile("D:\\KompasAssembly\\Gear.m3d", pPart, true);

	pBoss = p3DDoc->GetPart(0);
	pGear1 = p3DDoc->GetPart(1);


	ksEntityCollectionPtr col = pBoss->EntityCollection(o3d_face);
	ksEntityCollectionPtr col2 = pGear1->EntityCollection(o3d_face);

	ksEntityPtr BossFace4Assemly0 = col->GetByName("Face4Assembly0", true, true);
	ksEntityPtr BossFace4Assemly1 = col2->GetByName("Face4Assembly1", true, true);



	p3DDoc->AddMateConstraint(mc_Coincidence, BossFace4Assemly0, BossFace4Assemly1, -1, 1, 0);

	p3DDoc->RebuildDocument();
}
