module;
#include <map>
#include <vector>
#include <functional>
export module CommandHandler;

export inline bool StrEquals(const char* a, const char* b) noexcept
{
	while (*a)
	{
		if (*a != *b) return false;
		++a;
		++b;
	}
	return true;
}

export struct Argument
{
	private:
	void Free()
	{
		delete[] chars;
	}

	public:
	char* chars;
	int length;

	friend class CommandHandler;
};

export class CommandHandler
{
	std::map<const char*, std::function<void(const std::vector<Argument>&)>> handlers;

	public:
	void Parse(const char* input)
	{
		std::vector<Argument> words;
		int pos = 0;
		int wordCount = 0;
		while (true)
		{
			const bool end = *input == 0;
			if (*input == ' ' || end)
			{
				int offset = 0;
				for (auto& word : words)
					offset += word.length + 1;

				constexpr int bufSize = 256;
				words.push_back({ new char[bufSize] { 0 }, 0 });
				auto& word = words[wordCount];

				int i = 0;
				while (true)
				{
					if (i == bufSize - 1)
						break;
					auto ch = *((input - pos + offset) + i);
					if (ch == 0 || ch == ' ')
						break;
					words[wordCount].chars[i] = ch;
					++i;
				}
				if (i < 1)
				{
					words.pop_back();
					break;
				}
				word.length = i;
				++wordCount;
			}
			++pos;
			++input;
			if (end) break;
		}
		for (const auto& handler : handlers)
		{
			if (words.empty()) break;
			auto& word = words[0];
			const char* handlerName = handler.first;
			if (StrEquals(word.chars, handlerName))
			{
				words.erase(words.begin());
				handler.second(words);
			}
		}
		for (auto& word : words)
			word.Free();
	}

	void Handle(const char* name, std::function<void(const std::vector<Argument>&)> handler)
	{
		handlers[name] = handler;
	}
};