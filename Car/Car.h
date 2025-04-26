#pragma once
#include<iostream>
#include<thread>
#include<future>
#include<conio.h>
#include"Tank.h"
#include"Engine.h"
#include <mutex>
#include <stdexcept>
#include <algorithm>
//#define NOMINMAX
#include<Windows.h>

#define Escape					 27
#define Enter					 13

#define MAX_SPEED_LOWER_LIMIT		130
#define MAX_SPEED_HIGHER_LIMIT		410

#define MIN_DELAY					  50
#define MAX_DELAY					1000


class Car
{
private:
	Tank fuel_tank;
	Engine engine;
	const int MAX_SPEED;
	int speed;
	bool driver_inside;
	double base_acceleration;
	double base_friction;
	std::future<void> panel_task;
	std::atomic<bool> panel_running;
	mutable std::mutex console_mutex; //mutable позволяет использовать lock()/unlock() в const-методах
	//mutal Exlusion "Взаимное исключение". Объект-замок, используется, что бы не лезть в один и тот же участок кода или данные
	//Нужен, если допустим два потока пишут в одну и туже переменную, из-за этого они(потоки) могут "переписать" друг друга. 
	//Мьютекс позволяет: Первому потоку "Запереться", остальные ждут пока он закончит свою работу, потом первый поток "Снимает замок" и
	//следующий поток может "войти"
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
		driver_inside(false),
		base_acceleration(2.5),
		base_friction(1)
	{
		std::cout << "Car: " << this << std::endl;
		std::cout << "Car is ready! Press 'Enter' to get it!" << std::endl;
	}
	~Car()
	{
		std::cout << "Car: " << this << " is over!" << std::endl;
	}
	void CarRun()
	{
		engine.EngineStart([this](double amount) -> bool		//лямбда-выражение
			{
				double fuel_level = fuel_tank.get_fuel_level();
				fuel_tank.give_fuel(amount);					//тело лямбды
				return fuel_tank.get_fuel_level() > 0;
			});
	}
	void CarStop()
	{
		engine.EngineStop();
	}
	void get_in()
	{
		driver_inside = true;
		panel_running = true;
		panel_task = std::async(std::launch::async, &Car::panel, this);
	}
	void get_out()
	{
		panel_running = false;
		if (panel_task.valid()) panel_task.wait();
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
					//std::lock_guard<std::mutex> - это класс-помощник из стандартной библиотеки C++,
					//который работает автоматически захватывает и освобождает мьютекс по принципу
					//RAII (RAII - Resiurce Acquistion Is Initialization (Получения ресурса инициализации)
					//как только объект создаётся, он захватывает ресурс и как только объект уничтожается (выходит из блока, из стека, из unique_ptr и etc.) - 
					//ресурс автоматически освобождается) для mutex в C++ это std::lock_guard/std::unique_lock
					std::cout << "На сколько заправляемся?\n";

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
	void speed_up(int delay, std::unique_lock<std::mutex>& Ulock)
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

		//int delta;
		if (_kbhit())
		{
			char key = _getch();
			if (key == 'w' || key == 'W')
			{
				speed += acceleration;
				if (speed > MAX_SPEED) speed = MAX_SPEED;

				//delta = (std::max)(5, delay / 10);
				//delay -= delta;
				//if (delay < min_delay) delay = min_delay;
				Ulock.lock();
				speed_up(delay, Ulock);
				return;
			}
		}
		else
		{
			speed -= deceleration;
			if (speed < 0) speed = 0;


			//delta = (std::max)(5, delay / 20);
			//delay -= delta;
			//if (delay < min_delay) delay = min_delay;
			Ulock.lock();
			speed_up(delay, Ulock);
		}
	}
	void speed_down(int delay, std::unique_lock<std::mutex>& Ulock)
	{
		double deceleration = 9.81 * 0.71; //9.81 ускорение свободного падения, 0.71 коэфициент сопротевления движения(в данном случае мы считаем, что едим только по сухому асфальту)
		double initialSpeed = speed;	   //Начальная скорость
		const double lambda = 0.3;		   //коэфициент экспоненциального убывания 1/с
		const double deltaTime = 0.01;	   //"шаг" времени ,с
		double time = 0;
		if (deceleration < base_friction)
		{
			deceleration = base_friction;
		}
		Ulock.unlock();
		const int min_delay = 50;
		const int max_delay = 1000;

		std::this_thread::sleep_for(std::chrono::microseconds(delay));
		while (speed > 0)
		{
			double acceleration = deceleration * std::exp(-lambda * time);
			double fuel_consumed = acceleration * engine.get_consumption_per_second();
			fuel_tank.give_fuel(fuel_consumed);
			
			//int delta;
			if (_kbhit())
			{
				char key = _getch();
				if (key == 's' || key == 'S')
				{
					speed -= acceleration * deltaTime;
					if (speed < 0) speed = 0;

					time += deltaTime;

					//delta = (std::max)(5, delay / 20);
					//delay -= delta;
					//if (delay < min_delay) delay = min_delay;
					Ulock.lock();
					speed_down(delay, Ulock);
				}
			}
			else
			{
				speed -= deceleration;
				if (speed < 0) speed = 0;

				time += deltaTime;
				//delta = (std::max)(5, delay / 20);
				//delay -= delta;
				//if (delay < min_delay) delay = min_delay;
				Ulock.lock();
				speed_down(delay, Ulock);
			}
		}

	}
	void panel()
	{
		while (panel_running)
		{
			{
				std::lock_guard<std::mutex> lock(console_mutex);
				system("CLS");
				if (fuel_tank.get_fuel_level() == 0)
					std::cout << "Уровень топлива равен нулю! Для заправки нажмите 'f'" << std::endl;
				if (!engine.isStarted())
					std::cout << "Вы внутри машины для её включения нажимите 'e'" << std::endl;
				std::cout << "Fuel level: " << fuel_tank.get_fuel_level() << " liters" << std::endl;
				std::cout << "Engine is " << (engine.isStarted() ? "started" : "stoped") << std::endl;
				std::cout << "Speed: " << speed << " km/h\n";
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

