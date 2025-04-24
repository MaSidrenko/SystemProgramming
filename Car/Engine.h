#pragma once
#include<iostream>
#include<thread>
#include<future>
#include"Tank.h"


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
	void EngineStart(std::function<bool(double)> fuel_callback)
	{
		//std::function<> - универсальный "контейнер для вызова". Класс-шаблон из <functional>,который может хранить любой
		//вызываемый объект с заданной сигнатурой
		isRunning = true;
		task = std::async(std::launch::async, &Engine::Consume, this, fuel_callback);
	}
	void EngineStop()
	{
		isRunning = false;
	}
	bool isStarted()
	{
		return isRunning;
	}
	void Consume(std::function<bool(double)> fuel_callback)
	{

		while (isRunning)
		{
			if (!fuel_callback(consumpiton_per_second))
			{
				std::cout << std::endl << "Топлива нет. Мы встали!" << std::endl;
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

