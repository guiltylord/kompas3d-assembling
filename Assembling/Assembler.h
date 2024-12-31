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
};

struct ScrewData {
	float LegHeight;
	float LegThick;

	float HexRad;
	float HexDepth;

	float AxHoleRad;

	float GasketHeight;
	float GasketWidth;
};

struct PuckData {
	float Rad;
	float Height;
	float Width;
};

class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData GetSeal(int type);
	ScrewData GetScrew(int type);
	PuckData GetPuck(int type);

	void CreateSeal();
	void CreateScrew();
	void CreatePuck();

};

