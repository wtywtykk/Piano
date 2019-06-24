typedef void(*MenuCallback)(u32 Tag);

struct MenuStruct
{
	const char* Name;
	MenuCallback Callback;
	u32 Tag;
};

void EnterMenu(const struct MenuStruct* Struct);
