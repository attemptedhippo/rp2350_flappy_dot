#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "pico/stdlib.h"
#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "drivers/button/button.hpp"
#include "pimoroni_common.hpp"

using namespace pimoroni;

const int width{320};
const int height{240};
const int32_t half_height{height / 2};
// const int32_t third_height{height / 3};

ST7789 st7789(width, height, ROTATE_0, {27, 28, 30, 31, 32, 26});
PicoGraphics_PenRGB565 graphics(st7789.width, st7789.height, nullptr);

const RGB565 clear_color{PicoGraphics::rgb_to_rgb565(0, 80, 110)};
const RGB565 ball_color{PicoGraphics::rgb_to_rgb565(150, 150, 0)};
const RGB565 pipe_color{PicoGraphics::rgb_to_rgb565(0, 80, 0)};
const RGB565 text_color{PicoGraphics::rgb_to_rgb565(200, 200, 200)};
const RGB565 panel_color{PicoGraphics::rgb_to_rgb565(50, 50, 50)};

int32_t highscore{0};
int32_t highscore_x{0};
char highscore_text[512];
bool lost_state{false};
Rect ui_panel{width / 2 - 60, height / 2 - 50, 120, 100};

const int32_t pipe_start{width/2};
const int32_t pipe_width{30};
const int32_t pipe_spacing{120};
const int32_t pipe_speed{1};

const float_t ball_gravity{0.05};
const float_t ball_max_speed{2.0};
float_t	      ball_velocity{ball_max_speed};

const uint SWITCH_A_PIN{16};
const uint SWITCH_B_PIN{15};
const uint SWITCH_C_PIN{14};
const uint SWITCH_X_PIN{17};
const uint SWITCH_Y_PIN{18};
const uint SWITCH_Z_PIN{19};
Button button_z(SWITCH_Z_PIN, Polarity::ACTIVE_LOW, 0, 0);

//Functions:
void gen_pipes(std::vector<Rect>& _pipes, const int32_t _x, const int _pairs);

void update_pipes(std::vector<Rect>& _pipes);

void update_ball(Point& ball);

bool check_collision(Point& _ball, std::vector<Rect>& _pipes);

void reset(Point& _ball, std::vector<Rect>& _pipes, int32_t& _highscore);

int main()
{
	stdio_init_all();
	std::vector<Rect> pipes;
	gen_pipes(pipes, pipe_start, 5);
	Point ball(50, height/2);
	uint8_t timer{0};

	srand(12345);			// TODO: change to use score after loss

	while (true)
	{
		graphics.set_pen(clear_color);
		graphics.clear();

		if(timer > 1)
		{
			timer = 0;
		}
		else
		{
			if(!lost_state)
			{
				update_pipes(pipes);
				highscore++;
			}
		}

		timer++;

		if(button_z.read())
		{
			if(!lost_state)
			{
				ball_velocity = (float)-2.0;
			}
			else
			{
				lost_state = false;
				reset(ball, pipes, highscore);
			}
		}
		
		if(!lost_state)
		{
			update_ball(ball);

			if(check_collision(ball, pipes))
			{
				lost_state = true;
			}
		}
		//Draw your stuff below:
		for(auto &i : pipes)
		{	
			i.inflate(2);
			graphics.set_pen(0);
			graphics.rectangle(i);

			i.deflate(2);
			graphics.set_pen(pipe_color);
			graphics.rectangle(i);
		}
		graphics.set_pen(ball_color);
		graphics.circle(ball, 10);

		if(lost_state)
		{
			graphics.set_pen(panel_color);
			graphics.rectangle(ui_panel);
		}
		sprintf(highscore_text, "%d", highscore);

		graphics.set_pen(text_color);
		highscore_x = graphics.measure_text(highscore_text);
		Point highscore_pos{(width / 2) - highscore_x / 2, lost_state * 100};
		graphics.text(highscore_text, highscore_pos, 100);
		if(lost_state)
		{
			graphics.text("You Lost! Z to reset", Point(width / 2 - 50, height / 2), 101);
		}

		st7789.update(&graphics);
	}
}


void gen_pipes(std::vector<Rect>& _pipes, const int32_t _x, const int _pairs)
{
	_pipes.reserve(_pairs);
	int32_t pipe_cur{_x};
	int32_t rand_offset{0};	// NOTE: between -99 and 99 or something like that

	for(int i{0}; i < _pairs; i++)
	{
		rand_offset = rand() % 200 - 100;

		_pipes.emplace_back(pipe_cur, 0, pipe_width, half_height + rand_offset - 50);
		_pipes.emplace_back(pipe_cur, half_height + rand_offset + 50, pipe_width, 70 - rand_offset);
		pipe_cur += pipe_spacing;
	}
}

void update_pipes(std::vector<Rect>& _pipes)
{
	int32_t tmp_max_x{-20};
	bool	gen_new_pair{false};
	int32_t _index{0};

	for(auto &i : _pipes)
	{
		if(i.x + pipe_width <= 0)
		{
			gen_new_pair = true;
			auto it = _pipes.begin() + _index;
			_pipes.erase(it);
			continue;
		}

		i.x -= pipe_speed;

		if(i.x >= tmp_max_x)
			tmp_max_x = i.x;

		_index++;
	}

	gen_pipes(_pipes, tmp_max_x + pipe_spacing, 1);
	// i.x = tmp_max_x + pipe_spacing;
}

void update_ball(Point& ball)
{
	ball_velocity += ball_gravity;

	if(ball_velocity > ball_max_speed)
		ball_velocity = ball_max_speed;

	ball.y += std::round(ball_velocity);

}

bool check_collision(Point& _ball, std::vector<Rect>& _pipes)
{
	if(_ball.y + 10 > height)
		return true;
	if(_ball.y - 10 < 0)
		return true;

	for(auto &i : _pipes)
	{
		if (i.x + 10 > 80 || i.x - 10 < 20)
			continue;
		if (i.contains(_ball))
		{
			return true;
		}
	}

	return false;
}

void reset(Point& _ball, std::vector<Rect>& _pipes, int32_t& _highscore)
{
	srand(_highscore);
	_pipes.clear();
	gen_pipes(_pipes, pipe_start, 5);
	_ball.x = 50;
	_ball.y = height / 2;
	_highscore = 0;
}
