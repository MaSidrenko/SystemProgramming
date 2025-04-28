#include<iostream>
#include<thread>
#include<future>
#include<conio.h>
#include<Windows.h>
#include<chrono>

using namespace std::chrono_literals;


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
		if (fuel_level < 0)fuel_level = 0;
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
		consumption_per_second(DEFAULT_CONSUMPTION_PER_SECOND),
		is_started(false)
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

#define MAX_SPEED_LOWER_LIMIT		130
#define MAX_SPEED_HIGHER_LIMIT		410

class Car
{
private:
	Tank fuel_tank;
	Engine engine;
	const int MAX_SPEED;
	int speed;
	int acceleration;
	bool driver_inside;
	struct
	{
		std::thread panale_thread;
		std::thread engine_idle_thread;
		std::thread free_wheling_thread;
	}threads_container; //Эта струткутра не имеет имени, и реализует только один экземпляр
public:
	Car(double consumption, int capacity, int max_speed = 250, int acceleration = 10) :
		MAX_SPEED
		(
			max_speed < MAX_SPEED_LOWER_LIMIT ? MAX_SPEED_LOWER_LIMIT :
			max_speed > MAX_SPEED_HIGHER_LIMIT ? MAX_SPEED_HIGHER_LIMIT :
			max_speed
		),
		engine(consumption),
		fuel_tank(capacity),
		speed(0),
		driver_inside(false),
		acceleration(acceleration)
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
	void get_in()
	{
		driver_inside = true;
		//panel();
		threads_container.panale_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (threads_container.panale_thread.joinable())
			threads_container.panale_thread.join();
		system("CLS");
		std::cout << "You are out of the Car" << std::endl;
	}
	void start()
	{
		if (fuel_tank.get_fuel_level())
		{
			engine.start();
			threads_container.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (threads_container.engine_idle_thread.joinable())
			threads_container.engine_idle_thread.join();
	}
	void accelerate()
	{
		if (driver_inside && engine.started())
		{
			speed += acceleration;
			if (!threads_container.free_wheling_thread.joinable())
				threads_container.free_wheling_thread = std::thread(&Car::free_wheeling, this);
			if (speed > MAX_SPEED)
				speed = MAX_SPEED;
			std::this_thread::sleep_for(1s);
		}
	}
	void slow_donw()
	{
		if (driver_inside)
		{
			speed -= acceleration;
			if (speed < 0)
				speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void free_wheeling()
	{
		while (--speed > 0)
		{
			if (speed < 0) speed = 0;
			std::this_thread::sleep_for(1s);
		}
	}
	void control()
	{
		char key = 0;
		do
		{
			key = 0;
			if (_kbhit())
			{
				key = _getch();
			}
			switch (key)
			{
			case Enter:
				driver_inside ? get_out() : get_in();
				break;
			case 'F':case 'f':
				double fuel;
				std::cout << "Введите объём топлива: "; std::cin >> fuel;
				fuel_tank.fill(fuel);
				break;
			case'I':case'i':
				if (driver_inside)
				{
					!engine.started() ? start() : stop();
				}
				break;
			case'W':case'w':
				if (driver_inside)
					accelerate();
				break;
			case'S':case's':
				if (driver_inside)
					slow_donw();
				break;
			case Escape:
				stop();
				get_out();
			}
			if (fuel_tank.get_fuel_level() <= 0)stop();
			if (speed <= 0 && threads_container.free_wheling_thread.joinable())
				threads_container.free_wheling_thread.join();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started() && fuel_tank.give_fuel(engine.get_consumption_per_second()))
		{
			std::this_thread::sleep_for(1s);
		}
	}
	void panel()
	{
		while (driver_inside)
		{
			system("CLS");
			std::cout << "Fuel level: " << fuel_tank.get_fuel_level() << " listers" << std::endl;
			if (fuel_tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0xCF);
				std::cout << " LOW FUEL " << std::endl;
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			std::cout << "Enigine is " << (engine.started() ? "started" : "stoped") << std::endl;
			std::cout << "Speed: " << speed << " km/h\n";
			Sleep(100);
		}
	}
	void CarInfo()const
	{
		fuel_tank.info();
		engine.info();
		std::cout << "Speed now: " << speed << " km/h" << std::endl;
		std::cout << "Max speed: " << MAX_SPEED << " km/h" << std::endl;
	}
};


//#define TANK_CHEK
//#define ENGINE_CHEK

void main()
{
	setlocale(LC_ALL, "");
#ifdef TANK_CHEK
	double fuell;
	Tank tank(80);
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
	Car bmw(10, 80, 270);
	//bmw.CarInfo();
	bmw.control();

}
