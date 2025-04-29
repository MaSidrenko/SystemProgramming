#pragma once
#include<iostream>
#define MIN_TANK_CAPACITY		 20
#define MAX_TANK_CAPACITY		120


class Tank
{
private:
	double fuel_level;
public:
	const int CAPACITY;
	double get_fuel_level()const;
	Tank(int capacity);
	~Tank();
	double fill(double amount);
	double give_fuel(double amount);
	void info()const;
};

