#ifndef D1D22C44_94A1_4E7A_A15F_FD21FF12206C
#define D1D22C44_94A1_4E7A_A15F_FD21FF12206C



#include <memory>
#include <iostream>
#include <cstdint>
#include <ctime>




namespace miya_db
{


class optr;
class OverlayMemoryManager;
struct QueryContext;



/*
 概要:
 バリューはアップデートによって後にサイズが増大する可能性があるため,一操作のデータ単位を断片(flagment)として全てのフラグメントを収取したのもを最終的なvalueとする
*/


constexpr unsigned short VALUE_FLAGMENT_ADDR_SIZE = 5; // optrアドレスの長さと同じ
constexpr unsigned short DEFAULT_VALUE_FLAGMENT_HEADER_SIZE = sizeof(uint16_t) + VALUE_FLAGMENT_ADDR_SIZE + VALUE_FLAGMENT_ADDR_SIZE + sizeof(uint64_t);






struct ValueFragmentHeader
{

	struct
	{
		uint16_t _headerLength; // リスト構造なので意味がないかも
		// 断片リスト
		std::shared_ptr<optr> _prevFlagment; // cacheTableはセットされていないため注意
		std::shared_ptr<optr> _nextFlagment;

		uint64_t _valueLength;

		// タイムスタンプ
		uint64_t _timestamp;

	} _meta;

public:
	ValueFragmentHeader();

	void valueLength( size_t valueLength );
	size_t valueLength();

	size_t headerLength();

	void importBinary( std::shared_ptr<optr> startFlagmentOptr );
	size_t exportBianry( std::shared_ptr<unsigned char> *ret );

};










class ValueStoreManager
{


private:

	std::shared_ptr<OverlayMemoryManager> _dataOverlayMemoryManager; // データが保存されているファイルのマネージャー
	

public:
	ValueStoreManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	std::shared_ptr<optr> add( std::shared_ptr<QueryContext> qctx );
	std::shared_ptr<unsigned char> get();


	size_t get( std::shared_ptr<optr> targetOptr ,std::shared_ptr<unsigned char> *ret );
	

};



};




#endif // 


