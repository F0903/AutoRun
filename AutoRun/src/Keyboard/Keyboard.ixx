module;
#include <Windows.h>
#include <map>
export module Keyboard;
import Key;

export class Keyboard
{
	static inline std::map<unsigned int, Hotkey> hotkeys;

	static inline bool ignoreNext = false;
	static inline bool pollWaitMode = true;

	private:
	static void IgnoreNext() noexcept
	{
		ignoreNext = true;
	}

	public:
	static void SetPollingWait(bool wait) noexcept
	{
		pollWaitMode = wait;
	}

	static bool GetPollingWait() noexcept
	{
		return pollWaitMode;
	}

	static KeyState GetState(Key key) noexcept
	{
		const auto& info = GetKeyInfo(key);
		const auto state = GetKeyState(info.code);
		const bool highBit = (state & 0x8000);
		//const bool lowBit = (state & 0x0001); // Used for toggle keys like caps lock.
		return highBit ? KeyState::Pressed : KeyState::Released;
	}

	static void Tap(Key key)
	{
		const auto& keyInfo = GetKeyInfo(key);

		DWORD mode = KEYEVENTF_SCANCODE;

		KEYBDINPUT pressInput = { 0 };
		pressInput.wScan = keyInfo.scancode;
		pressInput.dwFlags = mode;

		INPUT press = { 0 };
		press.type = INPUT_KEYBOARD;
		press.ki = pressInput;

		KEYBDINPUT releaseInput = { 0 };
		releaseInput.wScan = keyInfo.scancode;
		releaseInput.dwFlags = mode | KEYEVENTF_KEYUP;

		INPUT release = { 0 };
		release.type = INPUT_KEYBOARD;
		release.ki = releaseInput;

		INPUT inputs[] = { press, release };

		constexpr unsigned int keyCount = 2;
		if (SendInput(keyCount, inputs, sizeof(INPUT)) != keyCount)
			throw;
	}

	static void Press(Key key)
	{
		const auto& info = GetKeyInfo(key);

		DWORD mode = KEYEVENTF_SCANCODE;

		KEYBDINPUT kbd = { 0 };
		kbd.wScan = info.scancode;
		kbd.dwFlags = mode;

		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki = kbd;

		constexpr unsigned int keyCount = 1;
		if (SendInput(keyCount, &input, sizeof(INPUT)) != keyCount)
			throw;
	}

	static void Release(Key key)
	{
		const auto& info = GetKeyInfo(key);

		DWORD mode = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

		KEYBDINPUT kbd = { 0 };
		kbd.wScan = info.scancode;
		kbd.dwFlags = mode;

		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki = kbd;

		constexpr unsigned int keyCount = 1;
		if (SendInput(keyCount, &input, sizeof(INPUT)) != keyCount)
			throw;
	}

	static void RegisterHotkey(Hotkey& hotkey)
	{
		if (!hotkey.onHotkey)
			throw "Hotkey handler is required.";

		UINT mod = MOD_NOREPEAT;
		if (hotkey.holdAlt) mod |= MOD_ALT;
		if (hotkey.holdCtrl) mod |= MOD_CONTROL;

		const auto& info = GetKeyInfo(hotkey.key);

		if (RegisterHotKey(NULL, hotkey.id, mod, info.code) == 0)
			throw;

		hotkeys[hotkey.id] = hotkey;
	}

	static void UnregisterHotkey(const Hotkey& hotkey) noexcept
	{
		UnregisterHotkey(hotkey.id);
	}

	static void UnregisterHotkey(const int hotkeyId) noexcept
	{
		UnregisterHotKey(NULL, hotkeyId);
		hotkeys.erase(hotkeys.find(hotkeyId));
	}

	static void PollHotkey()
	{
		MSG msg = { 0 };
		BOOL res;
		while ((res = pollWaitMode ? GetMessage(&msg, NULL, 0, 0) : PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) != 0)
		{
			if (res == -1) throw;
			if (msg.message != WM_HOTKEY)
				continue;
			if (ignoreNext)
			{
				ignoreNext = false;
				continue;
			}
			const unsigned int id = msg.wParam;
			const auto& hotkey = hotkeys[id];
			if (hotkey.onHotkey) hotkey.onHotkey();
			break;
		}
	}
};