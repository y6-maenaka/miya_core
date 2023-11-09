#ifndef E81917A3_924A_4690_938C_E7F1FB47E25E
#define E81917A3_924A_4690_938C_E7F1FB47E25E



// #include "../../components/index_manager/btree.h"
#include "../../components/index_manager/o_node.h"
#include "../safe_mode_manager.h"



namespace miya_db
{


struct ONodeConversionTable;







/*
 通常モードで使用するONodeとセーフモードで使用するSafeONodeとの変換テーブル
 ONodeはoptrで管理されており,SafeONodeは通常のポインタで管理する　その対応テーブル
 */
struct ONodeConversionTable // ONode to SafeONode
{
private:
	std::unordered_map< std::shared_ptr<SafeONode> , std::shared_ptr<SafeONode> > _entryMap;
	std::shared_ptr<OverlayMemoryManager> _safeOMemoryManager;

public:
	ONodeConversionTable( const std::shared_ptr<OverlayMemoryManager> safeOMemoryManager );
	void init();

	std::shared_ptr<SafeONode> ref( std::shared_ptr<SafeONode> target );
	void regist( std::shared_ptr<SafeONode> key , std::shared_ptr<SafeONode> value );

	void safeOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	const std::shared_ptr<OverlayMemoryManager> safeOMemoryManager();

};





// SafeONodeはONodeと異なるMemoryManagerを持っている為,SafeONodeから生成されたSafeONodeはSafeファイル自動で機に生成される
class SafeONode : public ONode
{

private:
	//static const std::shared_ptr<ONodeConversionTable> _conversionTable;

public:
	static ONodeConversionTable _conversionTable;

	SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	std::shared_ptr<SafeONode> parent(); // 再定義
	std::shared_ptr<SafeONode> child( unsigned short index ); // 再定義

	std::shared_ptr<ONodeItemSet> itemSet() override;
 
	struct Hash;

	bool operator ==( SafeONode& so ) const;
	bool operator !=( SafeONode& so ) const;

	void hello() override { std::cout << "Hello SafeONode" << "\n";};

	std::shared_ptr<SafeONode> shared_from_this()  
	{
   	 return SafeONode::shared_from_this();
	}

};



struct SafeONode::Hash
{
	std::size_t operator()(SafeONode &so ) const;
};






};



#endif // E81917A3_924A_4690_938C_E7F1FB47E25E



