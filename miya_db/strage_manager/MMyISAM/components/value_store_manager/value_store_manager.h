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
class CacheTable;
struct ONodeConversionTable;


/*
 概要:
 バリューはアップデートによって後にサイズが増大する可能性があるため,一操作のデータ単位を断片(flagment)として全てのフラグメントを収取したのもを最終的なvalueとする
*/


constexpr unsigned short VALUE_FLAGMENT_ADDR_SIZE = 5; // optrアドレスの長さと同じ
constexpr unsigned short DEFAULT_VALUE_FLAGMENT_HEADER_SIZE = sizeof(uint16_t) + VALUE_FLAGMENT_ADDR_SIZE + VALUE_FLAGMENT_ADDR_SIZE + sizeof(uint64_t) + sizeof(uint64_t);






struct ValueFragmentHeader
{
// 一旦publicで
	struct
	{
		uint16_t _headerLength; // リスト構造なので意味がないかも
		// 断片リスト
		std::shared_ptr<optr> _prevFlagment; // cacheTableはセットされていないため注意
		std::shared_ptr<optr> _nextFlagment;

		// ストアする本体のデータ長
		uint64_t _valueLength;

		// タイムスタンプ
		uint64_t _timestamp;

	} _meta;

	std::shared_ptr<CacheTable> _cacheTable;


public:
	ValueFragmentHeader( std::shared_ptr<CacheTable> cacheTable );

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
	// こっちは基本的に使わない
	// ValueStoreManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	ValueStoreManager( std::string valueFilePath );

	std::shared_ptr<optr> add( std::shared_ptr<unsigned char> data , size_t dataLength );
	std::shared_ptr<optr> add( std::shared_ptr<QueryContext> qctx );
	std::shared_ptr<unsigned char> get();
	size_t get( std::shared_ptr<optr> targetOptr ,std::shared_ptr<unsigned char> *ret );

	// Safeモードでセーフファイルに割り付けてあるファイルを本ファイルに移動する // ConvertsionTableにアクセスするためにIndex関連ファイルのインポートが必要なのは仕方ない
	void mergeDataOptr( std::shared_ptr<ONodeConversionTable> conversionTable ,ValueStoreManager* safeValueStoreManager );

	void clear(); // 管理ファイルを完全にリフレッシュする
	const std::shared_ptr<OverlayMemoryManager> overlayMemoryManager();
};



};




#endif //
