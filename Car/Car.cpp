#include "Car.h"


Car::Car(double consumption, int capacity, int max_speed) :
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
	base_acceleration(2.5),
	base_friction(1)
{
	std::cout << "Car: " << this << std::endl;
	std::cout << "Car is ready! Press 'Enter' to get it!" << std::endl;
}
Car::~Car()
{
	std::cout << "Car: " << this << " is over!" << std::endl;
}
void Car::CarRun()
{
	engine.EngineStart([this](double amount) -> bool		//������-���������
		{
			double fuel_level = fuel_tank.get_fuel_level();
			fuel_tank.give_fuel(amount);					//���� ������
			return fuel_tank.get_fuel_level() > 0;
		});
}
void Car::CarStop()
{
	engine.EngineStop();
}
void Car::get_in()
{
	driver_inside = true;
	panel_running = true;
	panel_task = std::async(std::launch::async, &Car::panel, this);
}
void Car::get_out()
{
	panel_running = false;
	if (panel_task.valid()) panel_task.wait();
	driver_inside = false;
}
double Car::RefuelCar(double amount)
{
	return fuel_tank.fill(amount);
}
void Car::control()
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
		case 'e': case 'E':
		{
			if (driver_inside)
			{
				CarRun();
			}
		}
		break;
		case 'f': case 'F':
		{
			double refuel;
			{
				std::lock_guard<std::mutex> lock(console_mutex);
				//std::lock_guard<std::mutex> - ��� �����-�������� �� ����������� ���������� C++,
				//������� �������� ������������� ����������� � ����������� ������� �� ��������
				//RAII (RAII - Resiurce Acquistion Is Initialization (��������� ������� �������������)
				//��� ������ ������ ��������, �� ����������� ������ � ��� ������ ������ ������������ (������� �� �����, �� �����, �� unique_ptr � etc.) - 
				//������ ������������� �������������) ��� mutex � C++ ��� std::lock_guard/std::unique_lock
				std::cout << "�� ������� ������������?\n";

				std::cin >> refuel;
				RefuelCar(refuel);
			}
		}
		break;
		case 'w':case'W':
		{
			if (engine.isStarted() && fuel_tank.get_fuel_level() > 0 && driver_inside)
			{
				{
					std::unique_lock<std::mutex> lock(console_mutex);

					speed_up(1000, lock);
				}
			}

		}
		break;
		case 's':case'S':
			if (engine.isStarted() && driver_inside && fuel_tank.get_fuel_level() > 0)
			{
				{
					std::unique_lock<std::mutex> lock(console_mutex);

					speed_down(1000, lock);
				}
			}
		}
	} while (key != Escape);
}
void Car::speed_up(int delay, std::unique_lock<std::mutex>& Ulock)
{


	double acceleration = base_acceleration * (1 - (double)(speed / MAX_SPEED));
	double deceleration = base_friction * double(1 - double(speed / MAX_SPEED));
	if (deceleration < base_friction)
	{
		deceleration = base_friction;
	}

	Ulock.unlock();

	const int min_delay = 50;
	const int max_delay = 1000;
	std::this_thread::sleep_for(std::chrono::microseconds(delay));

	double fuel_consumed = acceleration * engine.get_consumption_per_second();
	fuel_tank.give_fuel(fuel_consumed);

	if (_kbhit())
	{
		char key = _getch();
		if (key == 'w' || key == 'W')
		{
			speed += acceleration;
			if (speed > MAX_SPEED) speed = MAX_SPEED;

			Ulock.lock();
			speed_up(delay, Ulock);
			return;
		}
	}
	else
	{
		speed -= deceleration;
		if (speed < 0) speed = 0;

		Ulock.lock();
		speed_up(delay, Ulock);
	}
}
void Car::speed_down(int delay, std::unique_lock<std::mutex>& Ulock, double time)
{
	double deceleration = 9.81 * 0.71; //9.81 ��������� ���������� �������, 0.71 ���������� ������������� ��������(� ������ ������ �� �������, ��� ���� ������ �� ������ ��������)
	const double lambda = 0.3;		   //���������� ����������������� �������� 1/�
	const double deltaTime = 0.01;	   //"���" ������� ,�


	if (deceleration < base_friction)
	{
		deceleration = base_friction;
	}
	Ulock.unlock();

	std::this_thread::sleep_for(std::chrono::microseconds(delay));
	while (speed > 0)
	{
		if (time > 20.0)time = 20.0;


		double acceleration = deceleration * std::exp(-lambda * time);
		acceleration = (std::max)(acceleration, 0.1);

		double fuel_consumed = acceleration * engine.get_consumption_per_second();
		fuel_tank.give_fuel(fuel_consumed);

		if (_kbhit())
		{
			char key = _getch();
			if (key == 's' || key == 'S')
			{
				speed -= acceleration * deltaTime;
			}
		}
		else
		{
			speed -= deceleration * deltaTime;

		}
		if (speed < 0) speed = 0;
		time += deltaTime;
		std::this_thread::sleep_for(std::chrono::microseconds(delay));
	}

}
void Car::panel()
{
	while (panel_running)
	{
		{
			std::lock_guard<std::mutex> lock(console_mutex);
			system("CLS");
			if (fuel_tank.get_fuel_level() == 0)
				std::cout << "������� ������� ����� ����! ��� �������� ������� 'f'" << std::endl;
			if (!engine.isStarted())
				std::cout << "�� ������ ������ ��� � ��������� �������� 'e'" << std::endl;
			std::cout << "Fuel level: " << fuel_tank.get_fuel_level() << " liters" << std::endl;
			std::cout << "Engine is " << (engine.isStarted() ? "started" : "stoped") << std::endl;
			std::cout << "Speed: " << speed << " km/h\n";
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}
void Car::CarInfo()
{
	fuel_tank.info();
	engine.EngineInfo();
	std::cout << "Speed now: " << speed << " km/h" << std::endl;
	std::cout << "Max speed: " << MAX_SPEED << " km/h" << std::endl;
}