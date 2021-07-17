module;
#include <Windows.h>
#include <map>
export module Keypresser;

export enum class Key
{
	W,
	A,
	S,
	D,
	Space
};

struct KeyInfo
{
	WORD code;
};

struct Hotkey
{
	int id;
	bool holdAlt;
	bool holdCtrl;
	bool holdShift;
	Key key;
};

export class Keyboard
{
	static inline std::map<Key, KeyInfo> keys = {
		{Key::W, {0x57}},
		{Key::A, {0x41}},
		{Key::S, {0x53}},
		{Key::D, {0x44}},
		{Key::Space, {VK_SPACE}}
	};

	public:
	void Press(Key key)
	{
		const auto& keyInfo = keys[key];
		INPUT input = { INPUT_KEYBOARD, { keyInfo.code, 0, 0, 0, GetMessageExtraInfo() } };
		if (SendInput(1, &input, sizeof(INPUT)) != 1)
			throw;
	}

	void RegisterHotkey(Hotkey hotkey)
	{
		UINT mod = MOD_NOREPEAT;
		if (hotkey.holdAlt) mod |= MOD_ALT;
		if (hotkey.holdCtrl) mod |= MOD_CONTROL;
		if (hotkey.holdShift) mod |= MOD_SHIFT;

		KeyInfo key = keys[hotkey.key];

		if (RegisterHotKey(NULL, hotkey.id, mod, key.code) == 0)
			throw;
	}

	void UnregisterHotkey(int hotkeyId)
	{
		UnregisterHotKey(NULL, hotkeyId);
	}

	bool UpdateHotkeys()
	{
		bool result = false;
		MSG msg = { 0 };
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message != WM_HOTKEY)
				continue;
			result = true;
			break;
		}
		return result;
	}
};