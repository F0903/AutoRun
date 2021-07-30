module;
#include <iostream>
#include <vector>
#include <stack>
#include <thread>
#include <conio.h>
export module Console;

struct ConsoleAction
{
	bool newline = false;
	const char* text;
};

export class Console
{
	static inline bool blocked = false;
	static inline std::vector<ConsoleAction> blockedQueue;
	static inline bool listening = false;
	static inline std::thread listener;

	private:
	static void InternalWrite(const char* text) noexcept
	{
		std::cout << text;
	}

	static void InternalWrite(const char ch) noexcept
	{
		std::cout << ch;
	}

	static void InternalWriteLine(const char* text) noexcept
	{
		std::cout << text << '\n';
	}

	static void Queue(ConsoleAction action) noexcept
	{
		blockedQueue.push_back(action);
	}

	static void WriteQueue() noexcept
	{
		for (auto& i : blockedQueue)
		{
			if (i.newline)
			{
				WriteLine(i.text);
			}
			else
			{
				Write(i.text);
			}
		}
		blockedQueue.clear();
	}

	static inline const bool IsLetter(const char ch) noexcept
	{
		return (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122);
	}

	public:
	static void ListenForInput(void(*onInput)(const char*)) noexcept
	{
		if (listening) return;
		listening = true;
		listener = std::thread([onInput]()
		{
			bool lastState = false;
			std::stack<char> input;
			while (listening)
			{
				if (!lastState && !input.empty())
				{
					Console::Block();
					lastState = true;
				}

				char ch = _getch();
				if (ch == '\b')
				{
					if (input.empty()) continue;
					InternalWrite("\b \b");
					input.pop();

					if (input.empty())
					{
						Console::Unblock();
						lastState = false;
					}
					continue;
				}

				if (ch == '\r')
				{
					input.push(0);
					const auto size = input.size();
					char* buf = new char[size];
					for (int i = size - 1; i >= 0; i--)
					{
						buf[i] = input.top();
						input.pop();
					}
					onInput(buf);
					for (size_t i = 0; i < size; i++)
					{
						InternalWrite("\b \b");
					}
					delete[] buf;

					Console::Unblock();
					lastState = false;
					continue;
				}

				if (ch != ' ' && !IsLetter(ch))
					continue;

				InternalWrite(ch);
				input.push(ch);
			}
		});
	}

	static void StopListening() noexcept
	{
		listening = false;
	}

	static void Write(const char* text) noexcept
	{
		if (blocked)
		{
			Queue({ false, text });
			return;
		}
		InternalWrite(text);
	}

	static void WriteLine(const char* text) noexcept
	{
		if (blocked)
		{
			Queue({ true, text });
			return;
		}
		InternalWriteLine(text);
	}

	static void Block() noexcept
	{
		blocked = true;
	}

	static void Unblock() noexcept
	{
		blocked = false;
		WriteQueue();
	}
};