#include <iostream>
import Keyboard;

int main()
{
	bool running = false;
	auto kbd = Keyboard();
	Hotkey key = {
		1,
		true,
		false,
		true,
		Key::A,
		Key::W,
		[&]()
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
		},
		[&]()
		{
			kbd.Press(Key::W, true);
			running = false;
			std::cout << "Release (shorted)" << std::endl;
		}
	};
	kbd.RegisterHotkey(key);
	while (true)
	{
		kbd.PollHotkey();
	}
}