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
		seal.hexagon_radius = 23;
		seal.hexagon_angle_big = 65 * M_PI / 180;
		seal.hexagon_angle_small = 55 * M_PI / 180;
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
	ksPartPtr part = pDoc->GetPart(pTop_Part);

	//эскиз дно
	ksEntityPtr sketch1 = part->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDef = sketch1->GetDefinition();
	sketchDef->SetPlane(part->GetDefaultEntity(o3d_planeXOY));
	sketch1->Create();
	ksDocument2DPtr doc2D = sketchDef->BeginEdit();

	auto Seal = GetSeal(1);

	auto cosBig = cos(Seal.hexagon_angle_big);
	auto sinBig = sin(Seal.hexagon_angle_big);
	auto cosSmall = cos(Seal.hexagon_angle_small);
	auto sinSmall = sin(Seal.hexagon_angle_small);

	auto X1 = Seal.hexagon_radius * cosBig;
	auto Y1 = Seal.hexagon_radius * sinBig;
	auto X2 = Seal.hexagon_radius * cosSmall;
	auto Y2 = Seal.hexagon_radius * sinSmall;
	auto X3 = Seal.hexagon_radius * cos(Seal.hexagon_angle_small+120*M_PI/180);
	auto Y3 = Seal.hexagon_radius * sin(Seal.hexagon_angle_small+120*M_PI/180);

	doc2D->ksLineSeg(X1, Y1, X2, Y2, 1);
	doc2D->ksLineSeg(X2, Y2, X2, -Y2, 1);
	doc2D->ksLineSeg(X2, -Y2, X1, -Y1, 1);
	doc2D->ksLineSeg(X1, -Y1, X3, -Y3, 1);
	doc2D->ksLineSeg(X3, -Y3, X3, Y3, 1);
	doc2D->ksLineSeg(X3, Y3, X1, Y1, 1);

	sketchDef->EndEdit();

	//операция сохранения детали
	CString name = L"Гнездо сальника";
	pDoc->fileName = _bstr_t(name);
	pDoc->SaveAs(L"C:\\Users\\desxz\\source\\repos\\Assembling\\Assembling");
}
