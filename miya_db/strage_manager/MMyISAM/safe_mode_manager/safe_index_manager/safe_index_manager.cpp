#include "safe_index_manager.h"

#include "./safe_btree.h"


namespace miya_db
{

std::shared_ptr<SafeONode> ONodeConversionTable::ref( std::shared_ptr<SafeONode> target )
{
    auto itr = _entryMap.find( target );
    if( itr == _entryMap.end() ) return target; // 変換テーブルに要素が存在しなかった場合は,targetを返却する

    return itr->second; // 変換テーブルに要素が存在した場合
}


void ONodeConversionTable::regist( std::shared_ptr<SafeONode> key , std::shared_ptr<SafeONode> value )
{
    // コピーを作成する

    _entryMap[key] = value;


    // ここでONodeとマッピングされるSafeONodeはメンバの optrとoMemoryManager がsafeModeのものになっている
}





};