#include <iostream>
#include <vector>
import Keyboard;
import CommandHandler;
import Console;

auto cmd = CommandHandler();
bool sprint = false;

void OnRun()
{
	if (Keyboard::GetState(Key::W) == KeyState::Released)
	{
		if (sprint) Keyboard::Press(Key::Shift);
		Keyboard::Press(Key::W);
		Console::WriteLine("Press");
	}
	else
	{
		if (sprint) Keyboard::Release(Key::Shift);
		Keyboard::Release(Key::W);
		Console::WriteLine("Release");
	}
}

int main()
{
	Console::WriteLine("Starting...");

	cmd.Handle("help", [](const std::vector<Argument>& args)
		{
			Console::WriteLine("sprint (enable | disable)");
		});

	cmd.Handle("sprint", [](const std::vector<Argument>& args)
		{
			if (args.size() < 1)
			{
				Console::WriteLine("Sprint command requires at least 1 argument. Arguments are (enable | disable)");
				return;
			}
			auto& first = args[0];
			if (strcmp(first.chars, "enable") == 0)
			{
				sprint = true;
				Console::WriteLine("Sprint is now enabled...");
			}
			else if (strcmp(first.chars, "disable") == 0)
			{
				sprint = false;
				Console::WriteLine("Sprint is now disabled...");
			}
			else {
				Console::Write("Unknown argument.");
			}
		});

	Hotkey key = {
		0,
		true,
		true,
		Key::A,
		OnRun,
	};
	Keyboard::RegisterHotkey(key);

	Console::ListenForInput([](const char* inp)
		{
			cmd.Parse(inp);
		});

	Console::WriteLine("Started!\nType 'help' for commands.\n");
	while (true)
	{
		Keyboard::PollHotkey();
	}
}