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


	if(saturation == 0)
	{
		vector<float> retern;
		retern.push_back(value);
		retern.push_back(value);
		retern.push_back(value);
		return retern;
	}

	int i;
	float f,p,q,t,r1,g1,b1;
	float h = hue/60.0f;

	r1=g1=b1=0.0f;

	i = (int) h;
	f = h - i;
	p = value * (1 - saturation);
	q = value * (1 - saturation * f);
	t = value * (1 - saturation * ( 1 - f));


	switch( i ) {
		case 0:
			r1 = value;
			g1 = t;
			b1 = p;
			break;
		case 1:
			r1 = q;
			g1 = value;
			b1 = p;
			break;
		case 2:
			r1 = p;
			g1 = value;
			b1 = t;
			break;
		case 3:
			r1 = p;
			g1 = q;
			b1 = value;
			break;
		case 4:
			r1 = t;
			g1 = p;
			b1 = value;
			break;
		default:		// case 5:
			r1 = value;
			g1 = p;
			b1 = q;
			break;
	}


	vector<float> retern;
	retern.push_back(r1);
	retern.push_back(g1);
	retern.push_back(b1);

	return retern;
}