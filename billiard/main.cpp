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

		void collide(Ball& ano)
		{
			if (std::abs(this->position - ano.position) < this->radius + ano.radius) {
				std::swap(this->speed, ano.speed);
			}
		}

		void click()
		{
			auto distance = position - Vec(cursor_point.x, cursor_point.y);
			if (std::abs(distance) < this->radius) {
				Vec new_speed = distance;
				distance *= 0.9;
				speed += new_speed;
			}
		}

		void collide_with_wall()
		{
			float k = 0.85;
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

		void move();
};


std::vector<Ball> balls;

void Ball::move()
{
	for (auto& ball : balls) {
		if (&ball == this) {
			continue;
		}
		this->collide(ball);
	}

	if (std::abs(this->speed) < 0.2) {
		this->speed = {0, 0};
		return;
	}

	Vec acc = speed * -0.008f;
	this->speed += acc;
	this->position += speed;

	this->collide_with_wall();

}


void draw(nana::paint::graphics& graph)
{
	for (const auto& e : balls) {
		e.draw(graph);
	}
}


int main()
{
	form = std::make_shared<nana::form>(nana::API::make_center(1500, 700));
	form->bgcolor(nana::color(255, 255, 255));
	form->events().click([](const nana::arg_click& arg) {
		for (auto& e : balls) {
			e.click();
		}
	});
	form->events().mouse_move([](const nana::arg_mouse& arg) {
		cursor_point = arg.pos;
	});

	balls = [] {
		std::vector<Ball> balls;
		std::uniform_int_distribution<int> dis_x(0, form->size().width);
		std::uniform_int_distribution<int> dis_y(0, form->size().height);
		std::uniform_int_distribution<int> dis_color(0, 255);
		int i = 0;
		while (i < 40) {
			nana::color color = nana::color(dis_color(eg), dis_color(eg), dis_color(eg));
			Ball ball(Ball::Vec(dis_x(eg), dis_y(eg)), 30, color);

			if ((form->size().width - ball.position.real() < ball.radius) || (ball.position.real() < ball.radius)) {
				continue;
			}
			if ((form->size().height - ball.position.imag() < ball.radius) || (ball.position.imag() < ball.radius)) {
				continue;
			}

			bool flag = true;
			for (const auto& ano : balls) {
				if (std::abs(ball.position - ano.position) < ball.radius + ano.radius) {
					flag = false;
					break;
				}
			}
			if (flag) {
				balls.push_back(ball);
			}

			++i;
		}
		return balls;
	}();

	form->show();

	nana::drawing dw{*form};

	dw.draw([](nana::paint::graphics& graph) {
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
	std::thread move_loop([&loop] {
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
