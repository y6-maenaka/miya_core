#ifndef B3C13BE4_E4A4_496E_971D_400E0D09945A
#define B3C13BE4_E4A4_496E_971D_400E0D09945A


namespace chain
{

/*
  ブロックのある地点からのブロックヘッダを提供するheadersメッセージを要求するメッセージ
*/


struct MiyaCoreMSG_GETHEADERS
{
private:
  struct __attribute__((packed))
  {
      unsigned char _version[4];
      uint32_t _hashCount;
      unsigned char _blockHeaderHash[32];
      unsigned char _stopHash[32];
  } _body;

public:
  static constexpr char command[12] = "getheaders";

	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
	bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );

  std::vector<std::uint8_t> export_to_binary() const;
};


}


#endif // B3C13BE4_E4A4_496E_971D_400E0D09945A
