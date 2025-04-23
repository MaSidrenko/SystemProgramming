#include<iostream>
#include<thread>
#include<future>
#include<conio.h>
#include<Windows.h>

#define Escape					 27
#define Enter					 13

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

#define MIN_ENGINE_CONSUMPTION			 3
#define MAX_ENGINE_CONSUMPTION			25

class Engine
{
private:
	bool isRunning;
	const double CONSUMPITON;
	const double DEFAULT_CONSUMPITON_PER_SECOND;
	double consumpiton_per_second;
	std::future<void> task;
public:
	double get_consumption_per_second()const
	{
		return consumpiton_per_second;
	}
	Engine(double consumption) :
		CONSUMPITON
		(
			consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
			consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
			consumption
		),
		DEFAULT_CONSUMPITON_PER_SECOND(CONSUMPITON * 3e-5),
		consumpiton_per_second(DEFAULT_CONSUMPITON_PER_SECOND),
		isRunning(false)
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
		std::cout << "Engine: " << this << " is over" << std::endl;

	}
	void EngineStart()
	{
		isRunning = true;
		task = std::async(std::launch::async, &Engine::Consume, this, 0.0003);
	}
	void EngineStop()
	{
		isRunning = false;
	}
	bool isStarted()
	{
		return isRunning;
	}
	void Consume(double CONSUME_FUEL = 0.0003)
	{

		while (isRunning)
		{
			double fuel_now = CONSUME_FUEL;
			if (fuel_now <= 0)
			{
				std::cout << std::endl << "Топлива нет. Мы встали!" << std::endl;
				//isRunning = false;
				EngineStop();
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	void EngineInfo()const
	{
		std::cout << "Consumption: " << CONSUMPITON << " liters/100km\n";
		std::cout << "Default Consumption: " << DEFAULT_CONSUMPITON_PER_SECOND << " liters/s\n";
		std::cout << "Consumption: " << consumpiton_per_second << " listers/s\n";
	}
};

#define MAX_SPEED_LOWER_LIMIT		130
#define MAX_SPEED_HIGHER_LIMIT		410

class Car
{
private:
	Tank fuel_tank;
	Engine engine;
	const int MAX_SPEED;
	int speed;
	bool driver_inside;
public:
	Car(double consumption, int capacity, int max_speed = 250) :
		MAX_SPEED
		(
			max_speed < MAX_SPEED_LOWER_LIMIT ? MAX_SPEED_LOWER_LIMIT :
			max_speed > MAX_SPEED_HIGHER_LIMIT ? MAX_SPEED_HIGHER_LIMIT :
			max_speed
		),
		engine(consumption),
		fuel_tank(capacity),
		speed(0),
		driver_inside(false)
	{
		std::cout << "Car: " << this << std::endl;
		std::cout << "Car is ready! Press 'Enter' to enter her" << std::endl;
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
	void get_in()
	{
		driver_inside = true;
		panel();
	}
	void get_out()
	{
		driver_inside = false;
	}
	double RefuelCar(double amount)
	{
		return fuel_tank.fill(amount);
	}
	void control()
	{
		char key = 0;
		do
		{
			key = _getch();
			switch (key)
			{
			case Enter:
			{

				driver_inside ? get_out() : get_in();
			}
			break;
			case 101:
			{
				if (driver_inside)
				{
					CarRun();
					panel();
				}
			}
			break;
			case 102:
			{
				double refuel;
				std::cout << "На сколько заправляемся?\n";
				std::cin >> refuel;
				RefuelCar(refuel);
				panel();
			}
			break;
			}
		} while (key != Escape);
	}
	void panel()
	{
		while (driver_inside)
		{
			system("CLS");
			if (fuel_tank.get_fuel_level() == 0)
			{
				std::cout << "Уровень топлива равен нулю! Для заправки нажмите 'f'" << std::endl;
			}
			if (!engine.isStarted())
				std::cout << "Вы внутри машины для её включения нажимите 'e'" << std::endl;
			std::cout << "Fuel level: " << fuel_tank.get_fuel_level() << " liters" << std::endl;
			std::cout << "Engine is " << (engine.isStarted() ? "started" : "stoped") << std::endl;
			std::cout << "Speed: " << speed << " km/h\n";
			if (engine.isStarted())
			{
				engine.Consume();
			}
			control();
			Sleep(100);

		}
	}
	void CarInfo()
	{
		fuel_tank.info();
		engine.EngineInfo();
		std::cout << "Speed now: " << speed << " km/h" << std::endl;
		std::cout << "Max speed: " << MAX_SPEED << " km/h" << std::endl;
	}
};




void main()
{
	setlocale(LC_ALL, "");
	/*double fuell;
	char isRun;
	bool CarRun;
	Tank tank(80);
	Engine engine(10);
	engine.EngineInfo();*/
	//Engine engine(tank);
	//Car car(tank, engine);
	/*do
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
					system("CLS");
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
	} while (true);*/

	Car bmw(10, 80, 270);
	bmw.control();

}
