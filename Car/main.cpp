#include<iostream>
#include"Car.h"

void main()
{
	setlocale(LC_ALL, "");

	Car bmw(10, 80, 270);
	bmw.control();
	//bmw.CarInfo();

}
