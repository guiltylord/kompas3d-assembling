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
ksDocument3DPtr pDoc;

Assembler::Assembler()
{
}

Assembler::~Assembler()
{
}

SealData Assembler::GetSeal(int type)
{
	SealData seal = SealData();
	switch (type) {
	case 1:
		seal.hexagon_bottom_radius = 23;
		seal.hexagon_bottom_angle_big = 65 * M_PI / 180;
		seal.hexagon_bottom_angle_small = 55 * M_PI / 180;
		seal.hexagon_bottom_depth = 3;
		
		seal.hexagon_top_radius = 21.9 / 2;
		seal.hexagon_top_depth = 15;

		seal.hole_X = 34/2;
		seal.hole_radius_big = 8 / 2;
		seal.hole_radius_small = 4.5 / 2;

		seal.hole_axial_big = 16 / 2;
		seal.hole_axial_small = 8 / 2;
	}

	return seal;
}

void Assembler::CreateSeal()
{
	//CCmdTarget::BeginWaitCursor();
	//kompas.CreateInstance(L"Kompas.Application.5");

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

	pDoc = pKompasApp5->Document3D();
	pDoc->Create(false, true);
	pDoc = pKompasApp5->ActiveDocument3D();
	ksPartPtr pPart = pDoc->GetPart(pTop_Part);

	//эскиз дно
	ksEntityPtr sketchHex = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDef = sketchHex->GetDefinition();
	sketchDef->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	sketchHex->Create();
	ksDocument2DPtr doc2D = sketchDef->BeginEdit();

	auto Seal = GetSeal(1);

	auto cosBig = cos(Seal.hexagon_bottom_angle_big);
	auto sinBig = sin(Seal.hexagon_bottom_angle_big);
	auto cosSmall = cos(Seal.hexagon_bottom_angle_small);
	auto sinSmall = sin(Seal.hexagon_bottom_angle_small);

	auto X1 = Seal.hexagon_bottom_radius * cosBig;
	auto Y1 = Seal.hexagon_bottom_radius * sinBig;
	auto X2 = Seal.hexagon_bottom_radius * cosSmall;
	auto Y2 = Seal.hexagon_bottom_radius * sinSmall;
	auto X3 = Seal.hexagon_bottom_radius * cos(Seal.hexagon_bottom_angle_small+120*M_PI/180);
	auto Y3 = Seal.hexagon_bottom_radius * sin(Seal.hexagon_bottom_angle_small+120*M_PI/180);

	doc2D->ksLineSeg(X1, Y1, X2, Y2, 1);
	doc2D->ksLineSeg(X2, Y2, X2, -Y2, 1);
	doc2D->ksLineSeg(X2, -Y2, X1, -Y1, 1);
	doc2D->ksLineSeg(X1, -Y1, X3, -Y3, 1);
	doc2D->ksLineSeg(X3, -Y3, X3, Y3, 1);
	doc2D->ksLineSeg(X3, Y3, X1, Y1, 1);

	sketchDef->EndEdit();

	//выдавливание hexagon
	ksEntityPtr pExtrude1 = pPart->NewEntity(o3d_bossExtrusion);
	ksBossExtrusionDefinitionPtr PEP = pExtrude1->GetDefinition();
	PEP->directionType = dtNormal;
	PEP->SetSketch(sketchHex);
	PEP->SetSideParam(true, 0, Seal.hexagon_bottom_depth, 0, false);
	pExtrude1->Create();

	//смещенная плоскость
	ksEntityPtr pPlane1 = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pSketch3 = pPlane1->GetDefinition();
	pSketch3->direction = true;
	pSketch3->offset = 3;
	pSketch3->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pPlane1->Create();

	//эскиз hexagon2
	ksEntityPtr sketchHex2 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDef1 = sketchHex2->GetDefinition();
	sketchDef1->SetPlane(pPlane1);
	sketchHex2->Create();

	auto X_hex2 = Seal.hexagon_top_radius * cos(30*M_PI/180);
	auto Y_hex2 = Seal.hexagon_top_radius / 2;

	doc2D = sketchDef1->BeginEdit();
	doc2D->ksLineSeg(0, Seal.hexagon_top_radius, X_hex2, Y_hex2, 1);
	doc2D->ksLineSeg(X_hex2, Y_hex2, X_hex2, -Y_hex2, 1);
	doc2D->ksLineSeg(X_hex2, -Y_hex2, 0, -Seal.hexagon_top_radius, 1);
	doc2D->ksLineSeg(0, -Seal.hexagon_top_radius, -X_hex2, -Y_hex2, 1);
	doc2D->ksLineSeg(-X_hex2, -Y_hex2, -X_hex2, Y_hex2, 1);
	doc2D->ksLineSeg(-X_hex2, Y_hex2, 0, Seal.hexagon_top_radius, 1);

	sketchDef1->EndEdit();

	//эскиз отверстие 1
	ksEntityPtr sketchHole = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDefHole = sketchHole->GetDefinition();
	sketchDefHole->SetPlane(pPlane1);
	sketchHole->Create();

	doc2D = sketchDefHole->BeginEdit();
	doc2D->ksCircle(-Seal.hole_X, 0, Seal.hole_radius_big, 1);

	sketchDefHole->EndEdit();

	//эскиз отверстие 2
	ksEntityPtr sketchHole2 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDefHole2 = sketchHole2->GetDefinition();
	sketchDefHole2->SetPlane(pPlane1);
	sketchHole2->Create();

	doc2D = sketchDefHole2->BeginEdit();
	doc2D->ksCircle(-Seal.hole_X, 0, Seal.hole_radius_small, 1);

	sketchDefHole2->EndEdit();

	//выдавливание hexagon2
	ksEntityPtr pExtrude2 = pPart->NewEntity(o3d_bossExtrusion);
	PEP = pExtrude2->GetDefinition();
	PEP->directionType = dtNormal;
	PEP->SetSketch(sketchHex2);
	PEP->SetSideParam(true, 0, Seal.hexagon_top_depth, 0, false);
	pExtrude2->Create();

	//вырезание hole big
	ksEntityPtr pCutExtrude1 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr BED = pCutExtrude1->GetDefinition();
	BED->directionType = dtNormal;
	BED->SetSketch(sketchDefHole);
	BED->SetSideParam(true, etBlind, 2, 0, false);
	pCutExtrude1->Create();

	//вырезание hole small
	ksEntityPtr pCutExtrude2 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr BED2 = pCutExtrude2->GetDefinition();
	BED2->directionType = dtNormal;
	BED2->SetSketch(sketchDefHole2);
	BED2->SetSideParam(true, etThroughAll, 0, 0, false);
	pCutExtrude2->Create();

	//массив hole
	ksEntityPtr pCircCopy2 = pPart->NewEntity(o3d_circularCopy); // Получаем интерфейс объекта операции
	ksCircularCopyDefinitionPtr pCircDef = pCircCopy2->GetDefinition(); // Получаем интерфейс параметров операции

	pCircDef->Putcount2(3); // Количество копий
	pCircDef->SetAxis(pPart->GetDefaultEntity(o3d_axisOZ)); // Установка оси

	ksEntityCollectionPtr fl1 = pCircDef->GetOperationArray();
	fl1->Clear();
	fl1->Add(pCutExtrude1);
	fl1->Add(pCutExtrude2);

	pCircCopy2->Create();

	//смещенная плоскость для осевого отверстия
	ksEntityPtr pPlane24 = pPart->NewEntity(o3d_planeOffset);
	ksPlaneOffsetDefinitionPtr pSketch12 = pPlane24->GetDefinition();
	pSketch12->direction = true;
	auto t = Seal.hexagon_bottom_depth + Seal.hexagon_top_depth;
	pSketch12->offset = t;
	pSketch12->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	pPlane24->Create();

	//эскиз осевого отверстия 
	ksEntityPtr sketchAxialHole = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDefAxialHole = sketchAxialHole->GetDefinition();
	sketchDefAxialHole->SetPlane(pPlane24);
	sketchAxialHole->Create();
	doc2D = sketchDefAxialHole->BeginEdit();
	doc2D->ksCircle(0, 0, Seal.hole_axial_big, 1);
	sketchDefAxialHole->EndEdit();

	//вырезание осевого отверстия
	ksEntityPtr pCutExtrude3 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr BED3 = pCutExtrude3->GetDefinition();
	BED3->directionType = dtNormal;
	BED3->SetSketch(sketchDefAxialHole);
	BED3->SetSideParam(true, etBlind, Seal.hexagon_top_depth, 0, false);
	pCutExtrude3->Create();

	//эскиз осевого отверстия малого
	ksEntityPtr sketchAxialHole2 = pPart->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDefAxialHole2 = sketchAxialHole2->GetDefinition();
	sketchDefAxialHole2->SetPlane(pPart->GetDefaultEntity(o3d_planeXOY));
	sketchAxialHole2->Create();
	doc2D = sketchDefAxialHole2->BeginEdit();
	doc2D->ksCircle(0, 0, Seal.hole_axial_small, 1);
	sketchDefAxialHole2->EndEdit();

	//вырезание осевого отверстия
	ksEntityPtr pCutExtrude4 = pPart->NewEntity(o3d_cutExtrusion);
	ksCutExtrusionDefinitionPtr BED4 = pCutExtrude4->GetDefinition();
	BED4->directionType = dtReverse;
	BED4->SetSketch(sketchDefAxialHole2);
	BED4->SetSideParam(true, etBlind, Seal.hexagon_bottom_depth, 0, false);
	pCutExtrude4->Create();

	//операция сохранения детали
	CString name = L"Гнездо сальника.m3d";
	pDoc->fileName = _bstr_t(name);
	pDoc->SaveAs(L"C:\\Users\\desxz\\source\\repos\\Assembling\\Гнездо.m3d");
}
