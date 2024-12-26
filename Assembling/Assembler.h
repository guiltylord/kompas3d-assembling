#pragma once

struct SealData {
	float hexagon_radius;
	float hexagon_angle_big;
	float hexagon_angle_small;

};


class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData GetSeal(int type);


	void CreateSeal();

};

