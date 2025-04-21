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
		std::cout << "Tank:" << this << std::endl;
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

class Engine
{
private:
	bool isRunning;
	Tank& fuel_tank;
	std::future<void> task;
public:
	Engine(Tank& fuel_tank) : fuel_tank(fuel_tank), isRunning(false)
	{
		std::cout << "Engine: " << this << std::endl;
		std::cout << "Engine is ready!" << std::endl;
	}
	~Engine()
	{
		EngineStop();
		if (task.valid())
		{
			task.wait();
		}
		std::cout << "Engine:" << this << " is over" << std::endl;

	}
	void EngineStart()
	{
		isRunning = true;
		//std::thread(&Engine::Consume, this).detach();
		task = std::async(std::launch::async, &Engine::Consume, this, 0.0003);
	}
	void EngineStop()
	{
		isRunning = false;
	}
	void Consume(const double CONSUME_FUEL = 0.0003)
	{
		while (isRunning)
		{
			double fuel_now = fuel_tank.give_fuel(CONSUME_FUEL);
			if (fuel_now <= 0)
			{
				std::cout << "Топлива нет. Мы встали!" << std::endl;
				isRunning = false;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
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
		engine.EngineStart();
	}
	void CarStop()
	{
		engine.EngineStop();
	}
	void CarInfo()
	{
		fuel_tank.info();
	}
};




void main()
{
	setlocale(LC_ALL, "");
	double fuell;
	char isRun;
	bool CarRun;
	Tank tank(80);
	Engine engine(tank);
	Car car(tank, engine);
	do
	{
		std::cout << "На сколько заправляемся?: "; std::cin >> fuell;
		std::cout << std::endl;
		tank.fill(fuell);
		tank.info();
		std::cout << "Заводим машину Y/N?: "; std::cin >> isRun;
		std::cout << std::endl;
		if (isRun == 'Y')
		{
			if (fuell > 0)
			{
				car.CarRun();
				CarRun = true;
				while (CarRun)
				{
					//std::cout << 
					car.CarInfo();

					if (tank.get_fuel_level() <= 0)
					{
						CarRun = false;
					}
				}
			}
			else
			{
				std::cout << "Не хватет топлива" << std::endl;
				car.CarStop();
			}
		}
		else if (isRun == 'N')
		{
			car.CarStop();
		}
		else
		{
			std::cout << "Unknown symbol" << std::endl;
		}
	} while (true);
}
