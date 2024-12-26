#pragma once

struct SealData {
	float hexagon_bottom_depth;
	float hexagon_bottom_radius;
	float hexagon_bottom_angle_big;
	float hexagon_bottom_angle_small;
	
	float hexagon_top_radius;
	float hexagon_top_depth;
	
	float hole_X;
	float hole_radius_big;
	float hole_radius_small;

	float hole_axial_big;
	float hole_axial_small;
};


class Assembler
{
public:
	Assembler();
	~Assembler();
	
	SealData GetSeal(int type);


	void CreateSeal();

};

