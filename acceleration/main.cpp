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

		static constexpr int MAX_RADIUS = 35;

		Vec position;
		Vec speed;
		int radius;
		nana::color color;

		Ball(Vec pos) : position(pos)
		{
			{
				std::uniform_real_distribution<float> dis(-15, 15);
				speed = {dis(eg), dis(eg)};
			}
			{
				std::uniform_int_distribution<int> dis(12, MAX_RADIUS);
				radius = dis(eg);
			}
			{
				std::uniform_int_distribution<int> dis(0, 255);
				color = nana::color(dis(eg), dis(eg), dis(eg));
			}
		}

		void collide_with_wall()
		{
			float k = 0.95;
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

		void move()
		{
			Vec distance = Vec(cursor_point.x, cursor_point.y) - position;
			Vec acc = distance; {
				if (std::abs(distance) > 0.1) {
					acc /= std::abs(distance);
					float m = std::sin((1.0 - 0.85 * radius / (float) MAX_RADIUS) * M_PI_2);
					acc *= m;
				}
			};

			speed += acc;
			position += speed;
			this->collide_with_wall();
		}

		void draw(nana::form& form, nana::paint::graphics & graph)
		{
			ellipse(graph, cast_to_nana_point<int>(position), radius, radius, color);
		}
};

std::vector<Ball> balls = {
		Ball{{100, 100}}
};

void draw(nana::form& form, nana::paint::graphics & graph)
{
	for (auto & e : balls) {
		e.move();
		e.draw(form, graph);
	}
}


int main()
{
	form.bgcolor(nana::color(255, 255, 255));
	form.events().click([](const nana::arg_click& arg) {
		const nana::point point = arg.mouse_args->pos;
		balls.push_back(Ball{{point.x, point.y}});
	});
	form.events().mouse_move([](const nana::arg_mouse& arg) {
		cursor_point = arg.pos;
	});
	form.show();
	nana::drawing dw{form};

	dw.draw([&](nana::paint::graphics & graph){
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
