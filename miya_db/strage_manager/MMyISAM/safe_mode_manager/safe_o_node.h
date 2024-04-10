#ifndef E81917A3_924A_4690_938C_E7F1FB47E25E
#define E81917A3_924A_4690_938C_E7F1FB47E25E



// #include "../../components/index_manager/btree.h"
#include "../components/index_manager/o_node.h"
#include "../components/index_manager/index_manager.h"
#include "./safe_btree.h"

#include <array>
#include <typeinfo>
#include <unordered_map>




namespace miya_db
{


struct ONodeConversionTable;
struct SafeOCItemSet;
struct SafeOItemSet;




// SafONodeではDataOptrの格納先ファイルの切り替えがdataOptr単位で発生する為,本体の所在を保持しておく必要がある
// first: dataOptr , int: dataOptrLocation



// 分割用の仮想アイテムセット(ノードスプリットが発生する時以外は特に使用しない)
struct SafeViewItemSet : ViewItemSet
{
	std::array< DataOptrEx , DEFAULT_DATA_OPTR_COUNT + 1> _dataOPtr;

	void importItemSet( std::shared_ptr<SafeOCItemSet> citemSet );
	void moveInsertDataOptr( unsigned short index , DataOptrEx target );
};



struct SafeOCItemSet : public OCItemSet // Safe ONode Const ItemSet
{
public:
  std::shared_ptr<ONodeConversionTable> _conversionTable;

public:
	SafeOCItemSet( std::shared_ptr<ONodeItemSet> base , std::shared_ptr<ONodeConversionTable> conversionTable );

	void conversionTable( std::shared_ptr<ONodeConversionTable> target );
	// dataOptr返却時にConversionTable/DataOptrLocationを取得し,データの所在を付与する
	const DataOptrEx dataOptr( unsigned short index );
	std::array< DataOptrEx, DEFAULT_DATA_OPTR_COUNT > *exportDataOptrArray(); // DataOptrExで書き出す

	void hello(){ std::cout << "Hello :: SafeOCItemSet" << "\n";};
};




struct SafeOItemSet : public OItemSet // Safe ONode ItemSet
{
public:
  std::shared_ptr<ONodeConversionTable> _conversionTable;
public:
	SafeOItemSet( std::shared_ptr<ONodeItemSet> base , std::shared_ptr<ONodeConversionTable> conversionTable );

	void conversionTable( std::shared_ptr<ONodeConversionTable> target );
	// データポインタ保存・変更時にConversionTable/DataOptrLocationも変更する
	void dataOptr( unsigned short index , DataOptrEx target );
	void moveInsertDataOptr( unsigned short index , DataOptrEx targetDataOptr , std::shared_ptr<SafeOCItemSet> citemSet );

	void hello(){ std::cout << "Hello :: SafeOItemSet" << "\n";};
};





// SafeONodeはONodeと異なるMemoryManagerを持っている為,SafeONodeから生成されたSafeONodeはSafeファイル自動で機に生成される
class SafeONode : public ONode
{
private:
	std::shared_ptr<ONodeConversionTable> _conversionTable;
protected:
	int findIndex( std::shared_ptr<unsigned char> targetKey ) override;

public:
	SafeONode( std::shared_ptr<ONodeConversionTable> conversionTable , std::shared_ptr<OverlayMemoryManager> oMemoryManager = nullptr ,std::shared_ptr<optr> baseOptr = nullptr );

	std::shared_ptr<SafeONode> parent(); // 再定義
	std::shared_ptr<SafeONode> child( unsigned short index ); // 再定義

	std::shared_ptr<SafeOCItemSet> citemSet();
	std::shared_ptr<SafeOItemSet> itemSet();
	std::shared_ptr<ONodeConversionTable> conversionTable();
	void conversionTable( std::shared_ptr<ONodeConversionTable> target );

	std::shared_ptr<DataOptrEx> subtreeFind( std::shared_ptr<unsigned char> key );
	std::shared_ptr<SafeONode> subtreeMax();

	struct Hash;

	bool operator ==( SafeONode& so ) const;
	bool operator !=( SafeONode& so ) const;

	std::shared_ptr<SafeONode> recursiveAdd( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<DataOptrEx> targetDataOptr ,std::shared_ptr<SafeONode> targetONode = nullptr );
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








struct MappingContext
{
	optr _optr; // 変換先optr
	std::array< int /*index*/ , DEFAULT_DATA_OPTR_COUNT > _dataOptrLocation; // これを使ってDataOptrExを作成する
	// ONode/SafeONodeはメモリを使用しない為参照が終了すると関連情報以外は保存されない

	MappingContext( const optr target ) : _optr(target) {
		for( int i=0; i<_dataOptrLocation.size(); i++ ) _dataOptrLocation[i] = DATA_OPTR_LOCATED_AT_NORMAL;
	};

};


struct ONodeConversionTableEntryDetail
{
	std::shared_ptr<SafeONode> convertedONode; // 変換結果
	bool isExists; // 変換エントリ(情報)が存在したか否か
	std::shared_ptr<optr> key;
	std::shared_ptr<MappingContext> value;
};



struct ONodeConversionTable : public std::enable_shared_from_this<ONodeConversionTable> // ONode to SafeONode
{
// private:
public:
	std::unordered_map< const optr , MappingContext , optr::Hash > _entryMap;

	std::shared_ptr<OverlayMemoryManager> _safeOMemoryManager;
	std::shared_ptr<OverlayMemoryManager> _normalOMemoryManager;

public:
	ONodeConversionTable( const std::shared_ptr<OverlayMemoryManager> safeOMemoryManager );
	void init();

	std::pair< std::shared_ptr<SafeONode>, bool > ref( std::shared_ptr<optr> target );
	ONodeConversionTableEntryDetail refEx( std::shared_ptr<optr> target);
	void regist( std::shared_ptr<optr> key , std::shared_ptr<optr> value );
	void update( std::shared_ptr<optr> key , std::shared_ptr<MappingContext> target ); // そのままtargetに置き換わるので注意

	void safeOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	const std::shared_ptr<OverlayMemoryManager> safeOMemoryManager();

	void normalOMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	const std::shared_ptr<OverlayMemoryManager> normalOMemoryManager();

	const std::unordered_map< const optr , MappingContext , optr::Hash > entryMap();
	void printEntryMap();
};









};



#endif // E81917A3_924A_4690_938C_E7F1FB47E25E
