#ifndef F617C7C9_CF1F_4EF4_B567_E26A5A4D3C0E
#define F617C7C9_CF1F_4EF4_B567_E26A5A4D3C0E



namespace miya_db
{


class UnifiedStorageManager // どのストレージマネージャーを使っても
{

public:
	virtual void update();
	virtual void get();
};

};



#endif // F617C7C9_CF1F_4EF4_B567_E26A5A4D3C0E

