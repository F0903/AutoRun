#include <thread>
#include <iostream>
import Keypresser;

using namespace std::chrono_literals;

int main()
{
	bool shouldRun = false;
	auto kbd = Keyboard();
	kbd.RegisterHotkey({ 0, true, false, true, Key::A });
	while (true)
	{
		if (kbd.UpdateHotkeys())
		{
			if (shouldRun = !shouldRun) std::cout << "Starting keypresses..." << std::endl;
			else std::cout << "Stopped" << std::endl;
		}
		std::this_thread::sleep_for(100ms);
		if (!shouldRun)
			continue;
		kbd.Press(Key::W);
	}
}