#include <iostream>
import Keyboard;

auto kbd = Keyboard();

void OnHotkey(unsigned int hotkeyId)
{
	if (hotkeyId != 0)
		return;

	if (kbd.GetState(Key::W) == KeyState::Released)
	{
		kbd.Press(Key::W);
		kbd.GetState(Key::W);
		std::cout << "Press" << std::endl;
	}
	else
	{
		kbd.Release(Key::W);
		kbd.GetState(Key::W);
		std::cout << "Release" << std::endl;
	}
}

int main()
{
	std::cout << "Setting up hotkeys..." << std::endl;
	Hotkey key = {
		0,
		true,
		false,
		true,
		Key::A,
		OnHotkey,
	};
	kbd.RegisterHotkey(key);
	std::cout << "Done!" << std::endl;
	while (true)
	{
		kbd.PollHotkey();
	}
}