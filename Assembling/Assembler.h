#pragma once

struct SealData {
	float bottom_radius;
	float bottom_angle_big;
	float bottom_angle_small;

};


class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData GetSeal(int type);


	void CreateSeal();

};

