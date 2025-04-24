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
	double get_fuel_level()const
	{
		return fuel_level;
	}
	Tank(int capacity) :CAPACITY
	(
		capacity < MIN_TANK_CAPACITY ? MIN_TANK_CAPACITY :
		capacity > MAX_TANK_CAPACITY ? MAX_TANK_CAPACITY :
		capacity
	),
		fuel_level(0)
	{
		std::cout << "Tank: " << this << std::endl;
		if (capacity < MIN_TANK_CAPACITY)std::cout << "Min capacity was applied" << std::endl;
		if (capacity > MAX_TANK_CAPACITY)std::cout << "Max capacity was applied" << std::endl;
		std::cout << "Tank is ready" << std::endl;
	}
	~Tank()
	{
		std::cout << "Tank: " << this << " is over" << std::endl;
	}

	double fill(double amount)
	{
		if (amount < 0)
			return fuel_level;
		fuel_level += amount;
		if (fuel_level >= CAPACITY)
		{
			fuel_level = CAPACITY;
			std::cout << "Full Tank!" << std::endl;
		}
		return fuel_level;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level == 0)fuel_level = 0;
		return fuel_level;
	}
	void info()const
	{
		std::cout << "Capacity:\t" << CAPACITY << " listers" << std::endl;
		std::cout << "Fuel level:\t" << fuel_level << " listers" << std::endl;
	}
};

