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
  static constexpr char command[12] = "headers";

	void count( size_t target );

	std::vector< std::shared_ptr<struct BlockHeader> > headersVector();

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  std::vector<std::uint8_t> export_to_binary() const;
};


}; // namespace chain


#endif // C7FDBB05_3027_4D94_B5F4_4B17B9A11D23
