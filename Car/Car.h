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
	Car(double consumption, int capacity, int max_speed = 250);
	~Car();
	void CarRun();
	void CarStop();
	void get_in();
	void get_out();
	double RefuelCar(double amount);
	void control();
	void speed_up(int delay, std::unique_lock<std::mutex>& Ulock);
	void speed_down(int delay, std::unique_lock<std::mutex>& Ulock, double time = 0);
	void panel();
	void CarInfo();
};

