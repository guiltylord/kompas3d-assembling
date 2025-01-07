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
	float PinRadSmall;
	float PinRadBig;
	float PinDepth;

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
	
	SealData m_Seal;
	ScrewData m_Screw;
	PuckData m_Puck;

	SealData GetSeal(int type);
	ScrewData GetScrew(int type);
	PuckData GetPuck(int type);

	void FillAssembler(const int execution);

	void CreateSeal();
	void CreateScrew();
	void CreatePuck();
	void MakeAssemble();
	void GodAssemble(float RBase, float RHole, float L);

	int m_currExec;

	void CloseAll();
};

