#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>

class MyVector{
	double m_x, m_y, m_z;

public:
	MyVector();
	void setCoordinates(double x, double y, double z);
	void setX(double x);
	void setY(double y);
	void setZ(double z);
	double getX();
	double getY();
	double getZ();

friend const MyVector operator+ (const MyVector& myVec1, const MyVector& myVec2);

friend std::ostream& operator<< (std::ostream& out, const MyVector& myVec);
};
