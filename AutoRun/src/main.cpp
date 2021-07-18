#include <iostream>
import Keyboard;

auto kbd = Keyboard();
bool running = false;

void OnCombo()
{
	if (!running)
	{
		kbd.Press(Key::W);
		std::cout << "Press" << std::endl;
	}
	else
	{
		kbd.Press(Key::W, true);
		std::cout << "Release" << std::endl;
	}
	running = !running;
}

void OnShort()
{
	kbd.Press(Key::W, true);
	running = false;
	std::cout << "Release (shorted)" << std::endl;
}

int main()
{
	Hotkey key = {
		1,
		true,
		false,
		true,
		Key::A,
		Key::W,
		OnCombo,
		OnShort
	};
	kbd.RegisterHotkey(key);
	while (true)
	{
		kbd.PollHotkey();
	}
}