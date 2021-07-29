module;
#include <Windows.h>
#include <map>
export module Key;

export enum class Key
{
	None = -1,
	W,
	A,
	S,
	D,
	Space
};

export enum class KeyState
{
	Released,
	Pressed
};

export struct Hotkey
{
	unsigned int id;
	bool holdAlt;
	bool holdCtrl;
	bool holdShift;
	Key key;
	void(*onHotkey)();
};

export struct KeyInfo
{
	KeyInfo() = default;

	KeyInfo(WORD code) : code(code), scancode(MapVirtualKey(code, MAPVK_VK_TO_VSC))
	{}

	WORD code;
	UINT scancode;
};

static inline std::map<Key, KeyInfo> keys = {
	{ Key::W, KeyInfo(0x57) },
	{ Key::A, KeyInfo(0x41) },
	{ Key::S, KeyInfo(0x53) },
	{ Key::D, KeyInfo(0x44) },
	{ Key::Space, KeyInfo(VK_SPACE) }
};

export const KeyInfo& GetKeyInfo(Key key)
{
	return keys[key];
}