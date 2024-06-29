#ifndef C7FDBB05_3027_4D94_B5F4_4B17B9A11D23
#define C7FDBB05_3027_4D94_B5F4_4B17B9A11D23

#include <vector>


namespace chain
{


struct BlockHeader;

/*
    getheadersメッセージを受信したノードが,1つ以上のブロックヘッダを返答する際に使用する
		invにつめて返信することはしない
*/


struct MiyaCoreMSG_HEADERS
{
private:
	struct __attribute__((packed))
	 {
		uint32_t _count; // 格納したヘッダーの数
		std::vector< std::shared_ptr<struct BlockHeader> > _headers;
	} _body;

public:
  using ref = std::shared_ptr<MiyaCoreMSG_HEADERS>;
  static constexpr char command[12] = "headers";

	void count( size_t target );

	std::vector< std::shared_ptr<struct BlockHeader> > headersVector();

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  std::vector<std::uint8_t> export_to_binary() const;
};


void MiyaCoreMSG_HEADERS::count( size_t target )
{
	_body._count = static_cast<uint32_t>( target );
}

std::vector< std::shared_ptr<struct BlockHeader> > MiyaCoreMSG_HEADERS::headersVector()
{
	return _body._headers;
}

size_t MiyaCoreMSG_HEADERS::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	size_t rawTxsLength = 0;
	std::vector< std::pair<std::shared_ptr<unsigned char> , size_t> > rawTxVector;
	for( auto itr : _body._headers )
	{
		std::shared_ptr<unsigned char> rawTx; size_t rawTxLength = 0;
		rawTxLength = itr->exportRaw( &rawTx );
		rawTxVector.push_back( std::make_pair(rawTx , rawTxLength) );
		rawTxsLength += rawTxLength;
	}

	size_t retLength = sizeof(_body._count) + rawTxsLength;
	(*retRaw) = std::shared_ptr<unsigned char>( new unsigned char[retLength] );

	// 書き出し
	size_t formatPtr = 0;
	memcpy( (*retRaw).get() + formatPtr , &(_body._count) , sizeof(_body._count) );
	for( auto itr : rawTxVector ){
		memcpy( (*retRaw).get() + formatPtr , (itr.first).get() , itr.second );
		formatPtr += itr.second;
	}

	return formatPtr;
}


}; // namespace chain


#endif // C7FDBB05_3027_4D94_B5F4_4B17B9A11D23
