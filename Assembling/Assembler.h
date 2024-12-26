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

	float AxHole;
	float AxHoleThru;
};


class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData GetSeal(int type);


	void CreateSeal();

};

