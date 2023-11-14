#ifndef E81917A3_924A_4690_938C_E7F1FB47E25E
#define E81917A3_924A_4690_938C_E7F1FB47E25E



// #include "../../components/index_manager/btree.h"
#include "../components/index_manager/o_node.h"



namespace miya_db
{


struct ONodeConversionTable;

constexpr int DATA_OPTR_LOCATED_AT_NORMAL = 0;
constexpr int DATA_OPTR_LOCATED_AT_SAFE = 1;


struct SafeOItemSet : public OItemSet
{
public:
	void dataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr ) override;
};


// SafeONodeはONodeと異なるMemoryManagerを持っている為,SafeONodeから生成されたSafeONodeはSafeファイル自動で機に生成される
class SafeONode : public ONode
{
private:
	//static const std::shared_ptr<ONodeConversionTable> _conversionTable;

public:
	static ONodeConversionTable _conversionTable;

	SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager = nullptr ); // 新規作成コンストラクタ
	SafeONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<optr> baseOptr ); //

	std::shared_ptr<SafeONode> parent(); // 再定義
	std::shared_ptr<SafeONode> child( unsigned short index ); // 再定義

	std::shared_ptr<OCItemSet> citemSet() override;
	std::shared_ptr<OItemSet> itemSet() override;
	std::shared_ptr<optr> subtreeFind( std::shared_ptr<unsigned char> key );
	std::shared_ptr<SafeONode> subtreeMax();

	struct Hash;

	bool operator ==( SafeONode& so ) const;
	bool operator !=( SafeONode& so ) const;

	std::shared_ptr<SafeONode> recursiveAdd( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> targetDataOptr ,std::shared_ptr<SafeONode> targetONode = nullptr );
	std::shared_ptr<SafeONode> remove( std::shared_ptr<unsigned char> targetKey );
	std::shared_ptr<SafeONode> underflow( std::shared_ptr<SafeONode> sourceONode );
	std::shared_ptr<SafeONode> merge( unsigned short index );
	std::shared_ptr<SafeONode> merge( std::shared_ptr<SafeONode> sourceONode );
	std::shared_ptr<SafeONode> recursiveMerge( unsigned short index ) ;
	std::shared_ptr<SafeONode> recursiveMerge( std::shared_ptr<SafeONode> sourceONode ) ;

	void hello() override { std::cout << "Hello SafeONode" << "\n";};


	std::shared_ptr<SafeONode> shared_from_this()
	{
		return std::dynamic_pointer_cast<SafeONode>(ONode::shared_from_this());
	}
};





inline bool SafeONode::operator ==( SafeONode& so ) const
{
	bool ret =  (memcmp( _itemSet->citemSet()->Optr()->addr() , so.ONode::citemSet()->Optr()->addr() , NODE_OPTR_SIZE ) == 0 );
	return ret;
}


inline bool SafeONode::operator !=( SafeONode& so ) const
{
    return !(this->operator==(so));
}

struct SafeONode::Hash
{
	std::size_t operator()( std::shared_ptr<SafeONode> so ) const;
};


inline std::size_t SafeONode::Hash::operator()( std::shared_ptr<SafeONode> so )  const
{
	std::string bytes;
	std::copy( so->_itemSet->citemSet()->Optr()->addr(), so->_itemSet->citemSet()->Optr()->addr() + 5, std::back_inserter(bytes));
	return std::hash<std::string>()(bytes);
}




struct ONodeConversionTableEntryDetail
{
	std::shared_ptr<SafeONode> convertedONode; // 変換結果
	bool isExists; // 変換エントリ(情報)が存在したか否か
	std::pair< std::shared_ptr<optr>, std::shared_ptr<optr> > entry; // エントリ
};

struct MappingContext
{
	const optr _optr;
	std::array< int , DEFAULT_DATA_OPTR_COUNT > _dataOptrLocation;

	MappingContext( const optr target ) : _optr(target) {
		for( auto itr : _dataOptrLocation ) itr = 0;
	};

	std::shared_ptr<optr> Optr() const {
		return std::make_shared<optr>( _optr );
	}
};


struct ONodeConversionTable // ONode to SafeONode
{
private:
	std::unordered_map< const optr , const MappingContext , optr::Hash > _entryMap;

	std::shared_ptr<OverlayMemoryManager> _safeOMemoryManager;
	std::shared_ptr<OverlayMemoryManager> _normalOMemoryManager;

public:
	ONodeConversionTable( const std::shared_ptr<OverlayMemoryManager> safeOMemoryManager );
	void init();

	std::pair< std::shared_ptr<SafeONode>, bool > ref( std::shared_ptr<optr> target );
	ONodeConversionTableEntryDetail refEx( std::shared_ptr<optr> target);
	void regist( std::shared_ptr<optr> key , std::shared_ptr<optr> value );

	void safeOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	const std::shared_ptr<OverlayMemoryManager> safeOMemoryManager();

	void normalOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	const std::shared_ptr<OverlayMemoryManager> normalOMemoryManager();

	void printEntryMap();
};









};



#endif // E81917A3_924A_4690_938C_E7F1FB47E25E
