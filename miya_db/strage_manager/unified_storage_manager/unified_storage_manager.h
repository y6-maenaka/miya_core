#ifndef F617C7C9_CF1F_4EF4_B567_E26A5A4D3C0E
#define F617C7C9_CF1F_4EF4_B567_E26A5A4D3C0E


#include <memory>


namespace miya_db
{


struct QueryContext;


class UnifiedStorageManager //　どのストレージエンジンを使ってもインターフェースは統一する
{

public:
	// virtual void update() = 0;
	virtual bool get( std::shared_ptr<QueryContext> qctx ) = 0;
	virtual bool add( std::shared_ptr<QueryContext> qctx ) = 0;
	virtual bool remove( std::shared_ptr<QueryContext> qctx ) = 0;
	virtual bool exists( std::shared_ptr<QueryContext> qctx ) = 0;

	virtual void hello() = 0;
};

};



#endif // F617C7C9_CF1F_4EF4_B567_E26A5A4D3C0E

