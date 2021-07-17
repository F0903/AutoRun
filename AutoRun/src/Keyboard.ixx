module;
#include <Windows.h>
#include <map>
#include <functional>
export module Keyboard;

export enum class Key
{
	W,
	A,
	S,
	D,
	Space
};

export struct Hotkey
{
	int baseId;
	bool holdAlt;
	bool holdCtrl;
	bool holdShift;
	Key key;
	Key shortKey; // Key for shorting out.
	std::function<void()> onCombo;
	std::function<void()> onShort;
};

struct KeyInfo
{
	WORD code;
	UINT scancode;
};

export class Keyboard
{
	static inline std::map<Key, KeyInfo> keys = {
		{Key::W, { 0x57, MapVirtualKey(0x57, MAPVK_VK_TO_VSC)}},
		{Key::A, { 0x41, MapVirtualKey(0x41, MAPVK_VK_TO_VSC)}},
		{Key::S, { 0x53, MapVirtualKey(0x53, MAPVK_VK_TO_VSC)}},
		{Key::D, { 0x44, MapVirtualKey(0x44, MAPVK_VK_TO_VSC)}},
		{Key::Space, {VK_SPACE, MapVirtualKey(VK_SPACE, MAPVK_VK_TO_VSC) }}
	};

	static inline std::map<int, Hotkey> hotkeys;

	public:
	void Press(Key key, bool release = false)
	{
		const auto& keyInfo = keys[key];

		DWORD mode = KEYEVENTF_SCANCODE;
		if (release) mode |= KEYEVENTF_KEYUP;

		KEYBDINPUT kbd = { 0 };
		kbd.wScan = keyInfo.scancode;
		kbd.dwFlags = mode;

		INPUT input = { 0 };
		input.type = INPUT_KEYBOARD;
		input.ki = kbd;

		if (SendInput(1, &input, sizeof(INPUT)) != 1)
			throw;
	}

	void RegisterHotkey(Hotkey hotkey)
	{
		if (!hotkey.onCombo)
			throw "Combo handler is required.";

		UINT mod = MOD_NOREPEAT;
		if (hotkey.holdAlt) mod |= MOD_ALT;
		if (hotkey.holdCtrl) mod |= MOD_CONTROL;
		if (hotkey.holdShift) mod |= MOD_SHIFT;

		KeyInfo key = keys[hotkey.key];
		KeyInfo shortKey = keys[hotkey.shortKey];

		if (hotkey.baseId < 1) hotkey.baseId = 1;
		if (RegisterHotKey(NULL, hotkey.baseId, mod, key.code) == 0)
			throw;
		if (hotkey.onShort)
			if (RegisterHotKey(NULL, hotkey.baseId + 1, MOD_NOREPEAT, shortKey.code) == 0)
				throw;

		hotkeys[hotkey.baseId] = hotkey;
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

			const auto id = msg.wParam;
			if (id % 2 == 0)
			{
				const auto& hotkey = hotkeys[id - 1];
				if (hotkey.onShort) hotkey.onShort();
			}
			else
			{
				const auto& hotkey = hotkeys[id];
				if (hotkey.onCombo) hotkey.onCombo();
			}
			break;
		}
	}
};