#ifndef EE675C7D_7BCE_4454_845B_FDFD59A725D4
#define EE675C7D_7BCE_4454_845B_FDFD59A725D4



#include "./btree.h"


namespace miya_db
{




struct OCItemSet;
struct OItemSet;



struct ViewItemSet // ノードの追加の時際,分割をサポートする仮想的なアイテムセット
									 // ※少々オーバヘッドが大きいため,分割の際だけに使用する
{
	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT+1> _key; // 分割の際に一体対象のキーも追加するためサイズはプラス１
	std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT+1> _childOptr;
	std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT+1> _dataOPtr;

	void importItemSet( std::shared_ptr<OCItemSet> citemSet );
	void moveInsertChildOptr( unsigned short index ,std::shared_ptr<optr> target );
	void moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> target );
};

// メモリ上へのインデックス情報配置は [親ノードへのポインタ](個数)_key_key_key_key_key(個数)_dataPtr_dataPtr_dataPtr_dataPtr_dataPtr(個数)_child_child_child_child_child















// optrをラップして扱いやすくするための
struct ONodeItemSet
{
private:
	//optr *_optr;
	std::shared_ptr<optr> _optr = nullptr; // このItemSetの起点となる

public:
	ONodeItemSet( std::shared_ptr<optr> __optr );

	std::shared_ptr<optr> Optr();
	std::shared_ptr<optr> parent();

	/* Getter */
	unsigned short childOptrCount();
	std::shared_ptr<optr> childOptr( unsigned short index );
	/* Setter */
	void childOptr( unsigned short index , std::shared_ptr<optr> targetONode );
	void childOptrCount( unsigned short num );
	void moveInsertChildOptr( unsigned short index , std::shared_ptr<optr> targetONode );
	void moveDeleteChildOptr( unsigned short index );

	/* Getter */
	unsigned short keyCount();
	std::shared_ptr<optr> key( unsigned short index );
	std::shared_ptr<unsigned char> rawKey( unsigned short index );
	/* Setter */
	void key( unsigned short index , std::shared_ptr<unsigned char> targetKey ); // setter
	void keyCount( unsigned short num );
	void sortKey();
	void moveInsertKey( unsigned short index , std::shared_ptr<unsigned char> targetKey );
	void moveDeleteKey( unsigned short index );

	/* Getter */
	unsigned short dataOptrCount();
	std::shared_ptr<optr> dataOptr( unsigned short index );
	/* Setter */
	void dataOptrCount( unsigned short num );
	void dataOptr( unsigned short index ,std::shared_ptr<optr> targetDataOptr );
	void moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr );
	void moveDeleteDataOptr( unsigned short index );

	/* Setter */
	void parent( std::shared_ptr<ONode> target );

	void remove( unsigned short index ); // 子ノードは残す
	void clear();

	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *exportKeyArray();
	std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT> *exportChildOptrArray();
	std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *exportDataOptrArray();
};




struct OItemSet
{
private:
	std::shared_ptr<ONodeItemSet> _base;

public:
	OItemSet( std::shared_ptr<ONodeItemSet> base );

	void childOptr( unsigned short index , std::shared_ptr<optr> targetONode );
	void childOptrCount( unsigned short num );
	void moveInsertChildOptr( unsigned short index , std::shared_ptr<optr> targetONode );
	void moveDeleteChildOptr( unsigned short index );

	void key( unsigned short index , std::shared_ptr<unsigned char> targetKey );
	void keyCount( unsigned short num );
	void sortKey();
	void moveInsertKey( unsigned short index , std::shared_ptr<unsigned char> targetKey );
	void moveDeleteKey( unsigned short index );

	void dataOptrCount( unsigned short num );
	void dataOptr( unsigned short index ,std::shared_ptr<optr> targetDataOptr );
	void moveInsertDataOptr( unsigned short index , std::shared_ptr<optr> targetDataOptr );
	void moveDeleteDataOptr( unsigned short index );

	void parent( std::shared_ptr<ONode> target );

	void remove( unsigned short index );
	void clear();
};




struct OCItemSet
{
private:
	std::shared_ptr<ONodeItemSet> _base;

public:
	OCItemSet( std::shared_ptr<ONodeItemSet> base );

	const std::shared_ptr<optr> Optr();
	const std::shared_ptr<optr> parent();

	unsigned short childOptrCount();
	const std::shared_ptr<optr> childOptr( unsigned short index );

	unsigned short keyCount();
	const std::shared_ptr<optr> key( unsigned short index );
	const std::shared_ptr<unsigned char> rawKey( unsigned short index );

	unsigned short dataOptrCount();
	const std::shared_ptr<optr> dataOptr( unsigned short index );

	std::array< std::shared_ptr<unsigned char> , DEFAULT_KEY_COUNT> *exportKeyArray();
	std::array< std::shared_ptr<optr> , DEFAULT_CHILD_COUNT> *exportChildOptrArray();
	std::array< std::shared_ptr<optr>, DEFAULT_DATA_OPTR_COUNT > *exportDataOptrArray();
};






struct ItemSet{
	private:
		std::shared_ptr<ONodeItemSet> _body;

		std::shared_ptr<OItemSet> __itemSet;
		std::shared_ptr<OCItemSet> __citemSet;
	public:
		ItemSet( std::shared_ptr<ONodeItemSet> target );

		std::shared_ptr<OItemSet> itemSet();
		std::shared_ptr<OCItemSet> citemSet();
};




class ONode : public std::enable_shared_from_this<ONode>
{
private:
	bool _isLeaf = true;

protected:
	std::shared_ptr<OverlayMemoryManager> _oMemoryManager; // これから新たなノードを生成する可能性がある
	int findIndex( std::shared_ptr<unsigned char> targetKey );
	std::shared_ptr<ONode> subtreeMax();

public:
	std::shared_ptr<ItemSet> _itemSet;


	// ノード新規作成(新規割り当て)
	ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	// ノードラップ(新規作成はしない)
	ONode( std::shared_ptr<OverlayMemoryManager> oMemoryManager , std::shared_ptr<optr> baseOptr );

	void overlayMemoryManager( std::shared_ptr<OverlayMemoryManager> oMemoryManager );
	std::shared_ptr<OverlayMemoryManager> overlayMemoryManager();

	/* Setter */
	void itemSet( std::shared_ptr<ONodeItemSet> target );

	/* Getter */
	virtual std::shared_ptr<OCItemSet> citemSet(); 	// セーフモード( Getterのみを参照できる )
	virtual std::shared_ptr<OItemSet> itemSet();  // これが呼び出されるとsafeファイルにコピーが作成される

	void parent( std::shared_ptr<ONode> target );
	std::shared_ptr<ONode> parent();
	std::shared_ptr<ONode> child( unsigned short index );

	std::shared_ptr<optr> subtreeFind( std::shared_ptr<unsigned char> targetKey );
	std::shared_ptr<ONode> subtreeONodeFind( std::shared_ptr<unsigned char> targetKey );

	void isLeaf( bool flag ){ _isLeaf = flag; };
	bool isLeaf(){ return _isLeaf; }; // getter

	virtual void hello(){ std::cout << "Hello ONode" << "\n";};

	// ルートノードが更新されるとONodeがリターンされる
	std::shared_ptr<ONode> recursiveAdd( std::shared_ptr<unsigned char> targetKey , std::shared_ptr<optr> targetDataOptr ,std::shared_ptr<ONode> targetONode = nullptr );
	std::shared_ptr<ONode> remove( std::shared_ptr<unsigned char> targetKey );
	std::shared_ptr<ONode> underflow( std::shared_ptr<ONode> sourceONode );
	std::shared_ptr<ONode> merge( unsigned short index ); // ルードノードの変更が発生した場合は,戻り値として帰ってくる
	std::shared_ptr<ONode> merge( std::shared_ptr<ONode> sourceONode ); // 補助
	std::shared_ptr<ONode> recursiveMerge( unsigned short index );
	std::shared_ptr<ONode> recursiveMerge( std::shared_ptr<ONode> sourceONode ); // 補助
};






























}



#endif // EE675C7D_7BCE_4454_845B_FDFD59A725D4
