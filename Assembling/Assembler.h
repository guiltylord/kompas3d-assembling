#pragma once

struct SealData {
	float HBDepth;
	float HBRad;
	float HBAngleBig;
	float HBAngleSmall;
	
	float HTRad;
	float HTDepth;
	
	float X_Pin;
	float PinRad;
	float GrooveRad;

	float AxHoleRad;
	float AxHoleThruRad;
};

struct ScrewData {
	float LegHeight;
	float LegThick;

	float HexDepth;
	float HexRad;

	float AxHoleRad;

	float GasketHeight;
	float GasketWidth;
};

class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData GetSeal(int type);
	ScrewData GetScrew(int type);

	void CreateSeal();
	void CreateScrew();

};

