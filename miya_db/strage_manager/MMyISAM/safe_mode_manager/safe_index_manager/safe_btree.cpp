#include "safe_btree.h"

#include "./safe_index_manager.h"


namespace miya_db
{


SafeONode::SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager ) : ONode( nullptr ) // nullptrを渡して新規に割り当てさせないようにする
{ 
    /*
    // 新規作成す

    // セーフモードファイルのメモリマネージャーをセット
    overlayMemoryManager( _conversionTable->safeOMemoryManager() );

    std::shared_ptr<optr> baseOptr = oMemoryManager->allocate( O_NODE_ITEMSET_SIZE ); // 新規作成の場合
	_itemSet = std::make_shared<ONodeItemSet>( baseOptr );
	_itemSet->clear(); // ゼロ埋めする
    */
}


std::shared_ptr<SafeONode> SafeONode::parent()
{
    /*
    // 変換テーブルを一旦参照する 変換テーブルに該当するエントリがなければ,自身がそのまま帰ってくる
   std::shared_ptr<SafeONode> convertedONode = _conversionTable->ref( shared_from_this() );

   ONode *oNode = (dynamic_cast<ONode*>(convertedONode.get()))->parent().get();
   SafeONode *safeONode = static_cast<SafeONode*>(oNode);

   return std::make_shared<SafeONode>(*safeONode);
    */
}



std::shared_ptr<SafeONode> SafeONode::child( unsigned short index )
{
    /*
    // 変換テーブルを一旦参照する 変換テーブルに該当するエントリがなければ,自身がそのまま帰ってくる
    std::shared_ptr<SafeONode> convertedONode = _conversionTable->ref( shared_from_this() );

    ONode *oNode = (dynamic_cast<ONode*>(convertedONode.get()))->child(index).get();
    SafeONode *safeONode = static_cast<SafeONode*>(oNode);

    return std::make_shared<SafeONode>(*safeONode);
    */
}






bool SafeONode::operator ==( SafeONode& so ) const 
{
    /*
    return (memcmp( _itemSet->Optr()->addr() , so.itemSet()->Optr()->addr() , NODE_OPTR_SIZE ) == 0 );
    */
}


bool SafeONode::operator !=( SafeONode& so ) const 
{
    /*
    return !(this->operator==(so));
    */
}

std::size_t SafeONode::Hash::operator()( SafeONode &so ) const 
{
    /*
    std::string bytes(reinterpret_cast<const char*>(so.itemSet()->Optr()->addr(), NODE_OPTR_SIZE));
    return std::hash<std::string>()(bytes);
    */
}



};