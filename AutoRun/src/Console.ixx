module;
#include <iostream>
#include <vector>
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

	private:
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
	}

	public:
	static void Write(const char* text) noexcept
	{
		if (blocked)
		{
			Queue({ false, text });
			return;
		}
		std::cout << text;
	}

	static void WriteLine(const char* text) noexcept
	{
		if (blocked)
		{
			Queue({ true, text });
			return;
		}
		std::cout << text << '\n';
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