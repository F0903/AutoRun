#include <iostream>
#include <vector>
import Keyboard;
import CommandHandler;
import Console;

auto kbd = Keyboard();
auto cmd = CommandHandler();

void OnRun()
{
	if (kbd.GetState(Key::W) == KeyState::Released)
	{
		kbd.Press(Key::W);
		kbd.GetState(Key::W);
		Console::WriteLine("Press");
	}
	else
	{
		kbd.Release(Key::W);
		kbd.GetState(Key::W);
		Console::WriteLine("Release");
	}
}

int main()
{
	cmd.Handle("sprint", [](const std::vector<Argument>& args)
	{
		auto& first = args[0];
		if (StrEquals(first.chars, "enable"))
		{
			Console::WriteLine("Sprint is now enabled...");
		}
		if (StrEquals(first.chars, "disable"))
		{
			Console::WriteLine("Sprint is now disabled...");
		}
	});
	Console::WriteLine("Setting up hotkeys...");
	Hotkey key = {
		0,
		true,
		false,
		true,
		Key::A,
		OnRun,
	};
	kbd.RegisterHotkey(key);
	kbd.ListenForInput();
	Console::WriteLine("Done!");
	while (true)
	{
		kbd.PollHotkey();
	}
}