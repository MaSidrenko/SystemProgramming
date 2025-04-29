#pragma once
#include<iostream>
#include<thread>
#include<future>

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
	double get_consumption_per_second()const;
	Engine(double consumption);
	~Engine();
	void EngineStart(std::function<bool(double)> fuel_callback);
	void EngineStop();
	bool isStarted();
	void Consume(std::function<bool(double)> fuel_callback);
	void EngineInfo()const;
};

