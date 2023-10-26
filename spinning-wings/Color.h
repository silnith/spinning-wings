#pragma once

class Color
{
public:
	Color(void);

	Color(float r, float g, float b);

	float getRed(void) const;
	float getGreen(void) const;
	float getBlue(void) const;

	static Color const BLACK;
	static Color const WHITE;

private:
	float red;
	float green;
	float blue;
};
