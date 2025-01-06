#pragma once

struct SealData {
	float BaseDepth;
	float BaseRad;
	float BaseAngleBig;
	float BaseAngleSmall;
	
	float HexRad;
	float HexDepth;

	float AxHoleRad;
	float AxHoleThruRad;

	float X_Pin;
	float PinRad;
	float GrooveRad;
	float GrooveDepth;

	int ThreadDR;
	int ThreadP;
};

struct ScrewData {
	float FullHeight;
	float LegThick;

	float HexRad;
	float HexDepth;

	float AxHoleRad;

	float GasketHeight;
	float GasketWidth;

	int ThreadDR;
	int ThreadP;
};

struct PuckData {
	float RadIn;
	float Height;
	float RadOut;
};

class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData Seal;
	ScrewData Screw;
	PuckData Puck;

	SealData GetSeal(int type);
	ScrewData GetScrew(int type);
	PuckData GetPuck(int type);

	void FillAssembler(const int execution);

	void CreateSeal();
	void CreateScrew();
	void CreatePuck();
	void MakeAssemble();

	int currExec;

	void CloseAll();
};

