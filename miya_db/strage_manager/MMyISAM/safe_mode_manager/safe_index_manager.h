#ifndef BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A
#define BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A



#include <iostream>
#include <memory>
#include <string>


#include "../components/index_manager/index_manager.h"
#include "./safe_btree.h"


namespace miya_db
{


constexpr unsigned short SAFE_MODE_COLLICION_OFFSET = (O_NODE_ITEMSET_SIZE / 2);

class SafeOBtree;
class ONode;
class OBtree;
struct ONodeConversionTable;




class SafeIndexManager : public IndexManager
{
private:
	std::shared_ptr<SafeOBtree> _masterBtree;
	std::shared_ptr<ONodeConversionTable> _conversionTable; 

	std::shared_ptr<unsigned char> _migrationDBState; // SafeModeに移行した際のNormalDBの状態を保持する変数
public:
	 // コンストラクタ実行時に必ずNormalIndexManagerのマッピング位置の倍数にならない様にダミー領域をはじめに確保する
	SafeIndexManager( std::string indexFilePath ,const std::shared_ptr<OBtree> normalBtree , std::shared_ptr<unsigned char> migrationDBState );
	~SafeIndexManager(){};

	const std::shared_ptr<SafeOBtree> masterBtree();
	std::shared_ptr<ONodeConversionTable> conversionTable();

	void migrationDBState( std::shared_ptr<unsigned char> target ); 
	std::shared_ptr<unsigned char> migrationDBState();

	void add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr ) override;
	void remove( std::shared_ptr<unsigned char> key ) override;
	std::pair<std::shared_ptr<optr>, int > find( std::shared_ptr<unsigned char> key ) override;
	// [(first): データ本体先頭ポインター], [(second): データ本体が格納されているデータファイルインデックス]

	std::shared_ptr<ONode> mergeSafeBtree(); // セーフモードでの変更を通常OBtreeに反映する ≒ commit
											 // 内部にもつConversionTableを利用してマージを行うため,NormalIndexManagerは不要
	void printIndexTree();
};




/*
 SafeModeの振る舞い

 - SafeModeはレジストリIDを指定してモードチェンジ(SafeMode_(registryIndex))することができる
 - SafeMode中に,NormalDBに何らかの変更が加えられた場合は,以降のSafeModeのクエリは失敗するように設計する( 毎回SafeModeのStatusとNormalModeのStatusを比較するのはナンセンス )
	-> SafeModeが幾つか発行されていても,NormalDBへの変更は受け付けるようにする

 - SafeMode中のクエリが成功する条件は,SafeMode起動時に取得しているNormalModeの内部状態とクエリ発行時のNormalModeの内部状態が同じ時

 全てのSafeModeがcloseされる条件 : NormalModeにAdd, Remove, merge の更新があった


 (未セットアップのレジストリIDを指定してクエリが発行された場合)
*/





};


#endif //  BC852F99_DFC6_48DF_90C7_31B9A4C8EC4A
