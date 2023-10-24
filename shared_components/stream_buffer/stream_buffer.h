#ifndef F99BC652_963D_45EF_8377_CE50E2B01AC2
#define F99BC652_963D_45EF_8377_CE50E2B01AC2


#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <any>
#include <memory>
#include <chrono>

#include <arpa/inet.h>





namespace ekp2p
{
	class KNodeAddr;
	struct EKP2PMessageHeader;
};


constexpr unsigned int DEFAULT_STREAM_BUFFER_CAPACITY = 20;


constexpr unsigned char SB_FLAG_MODULE_EXIT = 0x01;




// ekp2pに最適化されたストリームバッファセグメント
struct SBSegment
{
	struct
	{
		uint8_t _flag;
		uint8_t _type;
		uint8_t _protocol;
		uint16_t _bodySize;
		uint16_t _reserved;

		unsigned short _forwardingSBCID; // brokerで次のモジュールへの転送先を示す

	} _controlBlock;


	struct EKP2PBlock
	{
		unsigned int _rpcType;
		bool _isProcessed = false;
		int _sendFlag = 0; // 0 : sendBack(sourceKNodeAddrに送信)
		std::shared_ptr<ekp2p::KNodeAddr> _sourceKNodeAddr;
		std::vector< std::shared_ptr<ekp2p::KNodeAddr> > _relayKNodeAddrVector;

		struct sockaddr_in _rawClientAddr;
	} _ekp2pBlock;


	struct
	{
		std::shared_ptr<unsigned char> _body;
	} _sbBlock;

public:
	struct Options
	{
		//std::shared_ptr<std::any> option1;
		std::any option1;
	} options;


	SBSegment(){};
	SBSegment( void* body , unsigned short bodyLength );


	/* Getter群 */
	std::shared_ptr<ekp2p::KNodeAddr> sourceKNodeAddr();
	std::shared_ptr<unsigned char> body();
	unsigned short bodyLength()	;
	std::vector< std::shared_ptr<ekp2p::KNodeAddr> > relayKNodeAddrVector();
	unsigned short rpcType();
	unsigned short protocol();
	bool ekp2pIsProcessed();
	int sendFlag();
	unsigned short forwardingSBCID();
	unsigned char flag();
	struct sockaddr_in rawClientAddr();

	/* Setter群 */
	void body( void* body , unsigned short bodyLength );
	void body( std::shared_ptr<unsigned char> body , unsigned short bodyLength );
	void sourceKNodeAddr( std::shared_ptr<ekp2p::KNodeAddr> target );
	void relayKNodeAddrVector( std::vector< std::shared_ptr<ekp2p::KNodeAddr> > target );
	void relayKNodeAddrVector( std::shared_ptr<ekp2p::KNodeAddr> target );
	void rpcType( unsigned short target );
	void protocol( unsigned short target );
	void ekp2pIsProcessed( bool target );
	void sendFlag( int target );
	void forwardingSBCID( unsigned short target );
	void flag( unsigned char target );
	void rawClientAddr( struct sockaddr_in target );


	// Import関係
	void importFromEKP2PHeader( std::shared_ptr<ekp2p::EKP2PMessageHeader> fromHeader );
};







class StreamBuffer
{

//private:
public:
	std::mutex _mtx;

	struct
	{
		std::condition_variable _popCV;
		std::vector< std::condition_variable > _contributers;
	}_popContributer ;

	struct
	{
		std::condition_variable _pushCV;
		std::vector< std::condition_variable > _contributers;
	} _pushContributer;


	struct
	{
		unsigned int _capacity;
		std::vector< std::unique_ptr<SBSegment> > _queue;
	} _sb;


public:
	StreamBuffer();

	void enqueue( std::unique_ptr<SBSegment> target , size_t timeout = 0 );
	std::unique_ptr<SBSegment> dequeue( size_t timeout = 0 );
};



#endif // F99BC652_963D_45EF_8377_CE50E2B01AC2
