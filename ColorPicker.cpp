#include "ColorPicker.h"


ColorPicker::ColorPicker(void)
{
}


ColorPicker::~ColorPicker(void)
{
}


/**
* hue - degrees (usually 0-360)
* saturation 0-1  0 is white, 1 is pure that color
* value 0 - 1  0 is black, 1 is pure that color
*/
vector<float> ColorPicker::getColor(float hue, float saturation, float value)
{
	while(hue>360.0f)
	{
		hue -=360.0f;
	}
	while(hue<0.0f)
	{
		hue +=360.0f;
	}

	float chroma = value * saturation;

	int hprime = (int)hue/60;

	float X = chroma * (hprime % 2 -1);

	float r1,g1,b1;
	r1=g1=b1=0;

	
	if(0<=hprime && hprime <1)
	{
		r1=chroma;
		g1=X;
	}
	else if(1<=hprime && hprime <2)
	{
		r1=X;
		g1=chroma;
	}
	else if(2<=hprime && hprime <3)
	{
		g1=chroma;
		b1=X;
	}
	else if(3<=hprime && hprime <4)
	{
		g1=X;
		b1=chroma;
	}
	else if(4<=hprime && hprime <5)
	{
		r1=X;
		b1=chroma;
	}
	else if(6<=hprime && hprime <6)
	{
		r1=chroma;
		b1=X;
	}

	float m = value - chroma;

	vector<float> retern;
	retern.push_back(r1+m);
	retern.push_back(g1+m);
	retern.push_back(b1+m);

	return retern;

}