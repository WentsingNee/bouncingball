#include <nana/gui.hpp>
#include <complex>
#include <thread>
#include <atomic>
#include <random>
#include <iostream>

#include "draw_ellipse.hpp"

nana::form form{nana::API::make_center(1500, 900)};
nana::point cursor_point;
std::minstd_rand eg(std::time(nullptr));

template <typename Tp, typename Up>
nana::basic_point<Tp>
cast_to_nana_point(const std::complex<Up>& pos)
{
	return nana::basic_point<Tp>(pos.real(), pos.imag());
}

struct Ball
{
		typedef std::complex<float> Vec;

		Vec position;
		Vec speed;
		int radius;
		nana::color color;

		Ball(Vec position, int radius) : position(position), speed(), radius(radius)
		{
			std::uniform_int_distribution<int> dis(0, 255);
			color = nana::color(dis(eg), dis(eg), dis(eg));
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
			if (form.size().width - this->position.real() < this->radius) {
				if (speed.real() > 0) {
					speed = {-k * speed.real(), speed.imag()};
				}
			} else if (this->position.real() < this->radius) {
				if (speed.real() < 0) {
					speed = {-k * speed.real(), speed.imag()};
				}
			}
			if (form.size().height - this->position.imag() < this->radius) {
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

		void draw(nana::form& form, nana::paint::graphics& graph)
		{
			ellipse(graph, cast_to_nana_point<int>(this->position), radius, radius, color);
		}
};


std::vector<Ball> balls = [] {
	std::vector<Ball> balls;
	std::uniform_int_distribution<int> dis_x(0, form.size().width);
	std::uniform_int_distribution<int> dis_y(0, form.size().height);
	int i = 0;
	while (i < 40) {
		Ball ball(Ball::Vec(dis_x(eg), dis_y(eg)), 30);

		if ((form.size().width - ball.position.real() < ball.radius) || (ball.position.real() < ball.radius)) {
			continue;
		}
		if ((form.size().height - ball.position.imag() < ball.radius) || (ball.position.imag() < ball.radius)) {
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


void draw(nana::form& form, nana::paint::graphics& graph)
{
	for (auto& e : balls) {
		e.move();
		e.draw(form, graph);
	}
}


int main()
{
	form.bgcolor(nana::color(255, 255, 255));
	form.events().click([](const nana::arg_click& arg) {
		for (auto& e : balls) {
			e.click();
		}
	});
	form.events().mouse_move([](const nana::arg_mouse& arg) {
		cursor_point = arg.pos;
	});
	form.show();
	nana::drawing dw{form};

	dw.draw([&](nana::paint::graphics& graph) {
		draw(form, graph);
	});

	std::atomic<bool> loop = true;
	form.events().destroy([&loop] {
		loop = false;
	});
	std::thread draw_loop([&loop, &dw] {
		using namespace std::chrono_literals;
		while (loop) {
			dw.update();
			std::this_thread::sleep_for(10ms);
		}
	});

	nana::exec();
	draw_loop.join();
	return 0;
}
