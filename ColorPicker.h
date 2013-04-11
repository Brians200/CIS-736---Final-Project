#pragma once
#include <vector>
using namespace std;
class ColorPicker
{
public:
	ColorPicker(void);
	~ColorPicker(void);

	vector<float> getColor(float hue, float saturation, float value);
};

