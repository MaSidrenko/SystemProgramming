#include<iostream>
#include<thread>
#include<future>

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

#define MIN_ENGINE_CONSUMPTION		 3
#define MAX_ENGINE_CONSUMPTION		25
class Engine
{
private:
	const double CONSUMPTION;
	const double DEFAULT_CONSUMPTION_PER_SECOND;
	double consumption_per_second;
	bool is_started;
public:
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	Engine(double consumption) :
		CONSUMPTION
		(
			consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
			consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
			consumption
		),
		DEFAULT_CONSUMPTION_PER_SECOND(CONSUMPTION * 3e-5),
		consumption_per_second(DEFAULT_CONSUMPTION_PER_SECOND)
	{
		std::cout << "Engine: " << this << " is ready" << std::endl;
	}
	~Engine()
	{
		std::cout << "Engine: " << this << " is over" << std::endl;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void info()const
	{
		std::cout << "Consumption: " << CONSUMPTION << " liters/100km\n";
		std::cout << "Default Consumption: " << DEFAULT_CONSUMPTION_PER_SECOND << " liters/s\n";
		std::cout << "Consumption: " << consumption_per_second << " listers/s\n";
	}
};

class Car
{
private:
	Tank& fuel_tank;
	Engine& engine;
public:
	Car(Tank& fuel_tank, Engine& engine) : fuel_tank(fuel_tank), engine(engine)
	{
		std::cout << "Car: " << this << std::endl;
		std::cout << "Car is ready!" << std::endl;
	}
	~Car()
	{
		std::cout << "Car: " << this << " is over!" << std::endl;
	}
	void CarRun()
	{
		engine.start();
	}
	void CarStop()
	{
		engine.stop();
	}
	void CarInfo()
	{
		fuel_tank.info();
	}
};


//#define TANK_CHEK
//#define ENGINE_CHEK

void main()
{
	setlocale(LC_ALL, "");
	double fuell;
	Tank tank(80);
#ifdef TANK_CHEK
	do
	{
		std::cout << "На сколько заправляемся?: "; std::cin >> fuell;
		std::cout << std::endl;
		tank.fill(fuell);
		tank.info();

	} while (true);
#endif // TANK_CHEK
#ifdef ENGINE_CHEK
	Engine engine(10);
	engine.info();
#endif // ENGINE_CHEK


}
