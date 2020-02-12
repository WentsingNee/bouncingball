#include <nana/gui.hpp>
#include <complex>
#include <thread>
#include <atomic>
#include <random>
#include <iostream>

#include "Circle.hpp"

std::shared_ptr<nana::form> form;
nana::point cursor_point;
std::minstd_rand eg(std::time(nullptr));

struct Ball: Circle
{
		Vec speed;

		Ball(const Vec& position, int radius, const nana::color& color) : Circle(position, radius, color)
		{
		}

		void collide_with_wall()
		{
			constexpr const float k = 0.75;
			if (form->size().width - this->position.real() < this->radius) {
				if (speed.real() > 0) {
					speed = {-k * speed.real(), speed.imag()};
				}
			} else if (this->position.real() < this->radius) {
				if (speed.real() < 0) {
					speed = {-k * speed.real(), speed.imag()};
				}
			}
			if (form->size().height - this->position.imag() < this->radius) {
				if (speed.imag() > 0) {
					speed = {speed.real(), -k * speed.imag()};
				}
			} else if (this->position.imag() < this->radius) {
				if (speed.imag() < 0) {
					speed = {speed.real(), -k * speed.imag()};
				}
			}
		}

		void move()
		{
			Vec distance = Vec(cursor_point.x, cursor_point.y) - position;
			Vec acc = distance;
			if (distance != Vec{0, 0}) {
				acc /= std::abs(distance);
				std::cout << acc << std::endl;
				float m = 0.2;
				acc *= m;
			}

			speed += acc;
			position += speed;
			this->collide_with_wall();
		}
};

std::vector<Ball> balls;

void draw(nana::paint::graphics & graph)
{
	for (const auto & e : balls) {
		e.draw(graph);
	}
}


int main()
{
	form = std::make_shared<nana::form>(nana::API::make_center(1500, 700));
	form->bgcolor(nana::color(255, 255, 255));
	form->events().click([](const nana::arg_click& arg) {
		const nana::point point = arg.mouse_args->pos;
		std::uniform_int_distribution<int> dis_radius(12, 25);
		std::uniform_int_distribution<int> dis_color(0, 255);
		nana::color color = nana::color(dis_color(eg), dis_color(eg), dis_color(eg));
		balls.emplace_back(Ball::Vec(point.x, point.y), dis_radius(eg), color);
		{
			std::uniform_real_distribution<float> dis_speed(-15, 15);
			balls.back().speed = {dis_speed(eg), dis_speed(eg)};
		}
	});
	form->events().mouse_move([](const nana::arg_mouse& arg) {
		cursor_point = arg.pos;
	});
	form->show();
	nana::drawing dw{*form};

	dw.draw([](nana::paint::graphics & graph){
		draw(graph);
	});

	std::atomic<bool> loop = true;
	form->events().destroy([&loop] {
		loop = false;
	});
	std::thread draw_loop([&loop, &dw] {
		using namespace std::chrono_literals;
		while (loop) {
			auto start = std::chrono::steady_clock::now();
			dw.update();
			std::this_thread::sleep_for(10ms);
			//std::this_thread::sleep_until(start + 20ms);
		}
	});
	std::thread move_loop([&loop, &dw] {
		using namespace std::chrono_literals;
		while (loop) {
			auto start = std::chrono::steady_clock::now();
			for (auto & e : balls) {
				e.move();
			}
			std::this_thread::sleep_for(10ms);
			//std::this_thread::sleep_until(start + 20ms);
		}
	});

	nana::exec();
	draw_loop.join();
	move_loop.join();
	return 0;
}
