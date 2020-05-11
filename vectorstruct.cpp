#include "vectorstruct.h"

MyVector::MyVector(){
	double x = (rand()%20 - 10)/static_cast<double>(rand()%20 + 1);
	double y = (rand()%20 - 10)/static_cast<double>(rand()%20 + 1);
	double z = (rand()%20 - 10)/static_cast<double>(rand()%20 + 1);
	setCoordinates(x, y, z);
}
void MyVector::setX(double x){
	if(m_x == x)
		return;

	m_x = x;
}

void MyVector::setCoordinates(double x, double y, double z){
	setX(x);
	setY(y);
	setZ(z);
}

void MyVector::setY(double y){
	if(m_y == y)
		return;

	m_y = y;
}

void MyVector::setZ(double z){
	if(m_z == z)
		return;

	m_z = z;
}

double MyVector::getX(){
	return m_x;
}

double MyVector::getY(){
	return m_y;
}

double MyVector::getZ(){
	return m_z;
}

const MyVector operator+ (const MyVector& myVec1, const MyVector& myVec2){
	MyVector myVec3;
	myVec3.m_x = myVec1.m_x + myVec2.m_x;
	myVec3.m_y = myVec1.m_y + myVec2.m_y;
	myVec3.m_z = myVec1.m_z + myVec2.m_z;
	return myVec3;
}

std::ostream& operator<< (std::ostream& out, const MyVector& myVec){
	out << "(" << myVec.m_x << ", " << myVec.m_y << ", " << myVec.m_z << ")";
	return out;
}