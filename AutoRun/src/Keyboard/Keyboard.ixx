module;
#include <Windows.h>
#include <map>
export module Keyboard;
import Key;

export class Keyboard
{
	static inline std::map<unsigned int, Hotkey> hotkeys;

	bool ignoreNext = false;

	private:
	void IgnoreNext()
	{
		ignoreNext = true;
	}

	public:
	KeyState GetState(Key key)
	{
		const auto& info = GetKeyInfo(key);
		const auto state = GetKeyState(info.code);
		const bool highBit = (state & 0x8000);
		//const bool lowBit = (state & 0x0001); // Used for toggle keys like caps lock.
		return highBit ? KeyState::Pressed : KeyState::Released;
	}

	void Tap(Key key)
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

	void Press(Key key)
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

	void Release(Key key)
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

	void RegisterHotkey(Hotkey& hotkey)
	{
		if (!hotkey.onHotkey)
			throw "Hotkey handler is required.";

		UINT mod = MOD_NOREPEAT;
		if (hotkey.holdAlt) mod |= MOD_ALT;
		if (hotkey.holdCtrl) mod |= MOD_CONTROL;
		if (hotkey.holdShift) mod |= MOD_SHIFT;

		const auto& info = GetKeyInfo(hotkey.key);

		if (RegisterHotKey(NULL, hotkey.id, mod, info.code) == 0)
			throw;

		hotkeys[hotkey.id] = hotkey;
	}

	void UnregisterHotkey(const Hotkey& hotkey)
	{
		UnregisterHotkey(hotkey.id);
	}

	void UnregisterHotkey(int hotkeyId)
	{
		UnregisterHotKey(NULL, hotkeyId);
	}

	void PollHotkey()
	{
		MSG msg = { 0 };
		BOOL res;
		while ((res = GetMessage(&msg, NULL, 0, 0)) != 0)
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
			if (hotkey.onHotkey) hotkey.onHotkey(id);
			break;
		}
	}
};