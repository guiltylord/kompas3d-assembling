#include "pch.h"
#include "Assembler.h"

#include<cmath>
#include "C:\Program Files\ASCON\KOMPAS-3D v22 Study\SDK\Include\ksConstants.h"
#include "C:\Program Files\ASCON\KOMPAS-3D v22 Study\SDK\Include\ksConstants3D.h"

#import "C:\Program Files\ASCON\KOMPAS-3D v22 Study\SDK\lib\kAPI5.tlb"

# define M_PI           3.14159265358979323846  /* pi */

using namespace Kompas6API5;

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
		seal.bottom_radius = 23;
		seal.bottom_angle_big = 65 * M_PI / 180;
		seal.bottom_angle_small = 55 * M_PI / 180;
	}

	return seal;
}

void Assembler::CreateSeal()
{
	KompasObjectPtr kompas;

	kompas.CreateInstance(L"Kompas.Application.5");
	kompas->Visible = true;

	ksDocument3DPtr doc;
	doc = kompas->Document3D();
	doc->Create(false, true);
	doc = kompas->ActiveDocument3D();
	ksPartPtr part = doc->GetPart(pTop_Part);

	//эскиз дно
	ksEntityPtr sketch1 = part->NewEntity(o3d_sketch);
	ksSketchDefinitionPtr sketchDef = sketch1->GetDefinition();
	sketchDef->SetPlane(part->GetDefaultEntity(o3d_planeXOY));
	sketch1->Create();
	ksDocument2DPtr doc2D = sketchDef->BeginEdit();

	auto Seal = GetSeal(1);
	auto t = std::cos(Seal.bottom_angle_big);
	auto X = Seal.bottom_radius * t;
	auto Y = Seal.bottom_radius * std::sin(Seal.bottom_angle_big);
	doc2D->ksLineSeg(0, 0, X, Y, 1);

	sketchDef->EndEdit();

	//операци€ сохранени€ детали
	CString name = L"√нездо сальника";
	doc->fileName = _bstr_t(name);
	doc->SaveAs(L"C:\\Users\\desxz\\source\\repos\\Assembling\\Assembling");
}
