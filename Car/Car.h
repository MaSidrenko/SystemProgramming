#pragma once
#include<iostream>
#include<thread>
#include<future>
#include<conio.h>
#include<Windows.h>
#include"Tank.h"
#include"Engine.h"
#include <mutex>

#define Escape					 27
#define Enter					 13

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
	std::future<void> panel_task;
	std::atomic<bool> panel_running;
	mutable std::mutex console_mutex; //mutable ��������� ������������ lock()/unlock() � const-�������
	//mutal Exlusion "�������� ����������". ������-�����, ������������, ��� �� �� ����� � ���� � ��� �� ������� ���� ��� ������
	//�����, ���� �������� ��� ������ ����� � ���� � ���� ����������, ��-�� ����� ���(������) ����� "����������" ���� �����. 
	//������� ���������: ������� ������ "����������", ��������� ���� ���� �� �������� ���� ������, ����� ������ ����� "������� �����" �
	//��������� ����� ����� "�����"
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
		std::cout << "Car is ready! Press 'Enter' to get it!" << std::endl;
	}
	~Car()
	{
		std::cout << "Car: " << this << " is over!" << std::endl;
	}
	void CarRun()
	{
		engine.EngineStart([this](double amount) -> bool		//������-���������
			{
				double fuel_level = fuel_tank.get_fuel_level();
				fuel_tank.give_fuel(amount);					//���� ������
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
				double km;
				{
					std::lock_guard<std::mutex> lock(console_mutex);
					if (driver_inside && fuel_tank.get_fuel_level() > 0 && engine.isStarted())
					{
						std::cout << "������� �� �������� �����?: "; std::cin >> km;
						std::cout << "� ����� ���������?: "; std::cin >> speed;
					}
				}
				break;
			}
		} while (key != Escape);
	}
	void panel()
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
	void CarInfo()
	{
		fuel_tank.info();
		engine.EngineInfo();
		std::cout << "Speed now: " << speed << " km/h" << std::endl;
		std::cout << "Max speed: " << MAX_SPEED << " km/h" << std::endl;
	}
};

