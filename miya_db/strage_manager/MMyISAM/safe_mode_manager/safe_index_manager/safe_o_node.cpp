#include "safe_o_node.h"

#include "./safe_index_manager.h"


namespace miya_db
{

ONodeConversionTable::ONodeConversionTable( std::shared_ptr<OverlayMemoryManager> safeOMemoryManager )
{
    _safeOMemoryManager = safeOMemoryManager;
}


void ONodeConversionTable::init()
{
    _entryMap.clear();
}


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

void ONodeConversionTable::safeOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager )
{
    _safeOMemoryManager = oMemoryManager;
}


const std::shared_ptr<OverlayMemoryManager> ONodeConversionTable::safeOMemoryManager()
{
    return _safeOMemoryManager;
}



















SafeONode::SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager ) : ONode( nullptr ) // nullptrを渡して新規に割り当てさせないようにする
{ 
    // 新規作成

    // セーフモードファイルのメモリマネージャーをセット
    overlayMemoryManager( _conversionTable.safeOMemoryManager() );

    std::shared_ptr<optr> baseOptr = oMemoryManager->allocate( O_NODE_ITEMSET_SIZE ); // 新規作成の場合
    ONode::itemSet( std::make_shared<ONodeItemSet>(baseOptr) );
    itemSet()->clear(); // ゼロ埋め


    std::cout << "\x1b[35m" << "SafeONodeが初期化されました" << "\x1b[39m" << "\n";
}


std::shared_ptr<SafeONode> SafeONode::parent()
{
   // 変換テーブルを一旦参照する 変換テーブルに該当するエントリがなければ,自身がそのまま帰ってくる
   std::shared_ptr<SafeONode> convertedONode = _conversionTable.ref( shared_from_this() );

    std::cout << "\x1b[35m" << "This is SafeONode::parent" << "\x1b[39m" << "\n";

   ONode *oNode = (dynamic_cast<ONode*>(convertedONode.get()))->parent().get();
   SafeONode *safeONode = static_cast<SafeONode*>(oNode);

   return std::make_shared<SafeONode>(*safeONode);
}



std::shared_ptr<SafeONode> SafeONode::child( unsigned short index )
{
    // 変換テーブルを一旦参照する 変換テーブルに該当するエントリがなければ,自身がそのまま帰ってくる
    std::shared_ptr<SafeONode> convertedONode = _conversionTable.ref( shared_from_this() );

    std::cout << "\x1b[35m" << "This is SafeONode::child" << "\x1b[39m" << "\n";
    return static_pointer_cast<SafeONode>(convertedONode->child(index));

    /*
    ONode *oNode = (dynamic_cast<ONode*>(convertedONode.get()))->child(index).get();
    SafeONode *safeONode = static_cast<SafeONode*>(oNode);
    return std::make_shared<SafeONode>(*safeONode);
    */
}


OItemSet* SafeONode::itemSet() 
{
    std::cout << "\x1b[35m" << "SafeONode::itemSet()" << "\x1b[39m" << "\n";
    return ONode::itemSet();
}




bool SafeONode::operator ==( SafeONode& so ) 
{
    return (memcmp( citemSet()->Optr()->addr() , so.citemSet()->Optr()->addr() , NODE_OPTR_SIZE ) == 0 );
}


bool SafeONode::operator !=( SafeONode& so )
{
    return !(this->operator==(so));
}

std::size_t SafeONode::Hash::operator()( SafeONode &so ) const 
{
    std::string bytes(reinterpret_cast<const char*>(so.citemSet()->Optr()->addr(), NODE_OPTR_SIZE));
    return std::hash<std::string>()(bytes);
}





};
