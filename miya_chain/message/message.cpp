#include "message.h"



namespace miya_chain
{



size_t MiyaChainMessage::payloadLength()
{
	#ifdef __linux__
		return static_cast<size_t>( be32toh(_header._payloadLength) );
	#endif
	#ifdef __APPLE__
		return static_cast<size_t>( ntohl(_header._payloadLength) );
	#endif

}


const unsigned char *MiyaChainMessage::command()
{
	return _header._command;
}




void MiyaChainMessage::payload( MiyaChainCommand targetPayload , unsigned char* comand  )
{
	memcpy( _header._command , comand , sizeof(_header._command) );
	_payload = targetPayload;
}







bool MiyaChainMessage::importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength )
{
	if( sizeof(_header) > fromRawLength ) return false;

	// header部の取り込み
	memcpy( &_header , fromRaw.get() , sizeof(_header) );



	if( memcmp(_header._command , MiyaChainMSG_INV::command , sizeof(_header._command) ) == 0 )
	{
		std::cout << "(MiyaChainMSG) : inv" << "\n";
		struct MiyaChainMSG_INV payload;
	}

	else if( memcmp( _header._command , MiyaChainMSG_BLOCK::command , sizeof(_header._command) )  == 0 )
	{
		std::cout << "(MiyaChainMSG) : block" << "\n";
		struct MiyaChainMSG_BLOCK payload;
	}

	else if( memcmp( _header._command, MiyaChainMSG_GETBLOCKS::command , sizeof(_header._command) ) == 0 )
	{
		std::cout << "(MiyaChainMSG) : getblocks" << "\n";
		struct MiyaChainMSG_GETBLOCKS payload;
	}

	else if( memcmp( _header._command, MiyaChainMSG_GETDATA::command , sizeof(_header._command) ) == 0 )
	{
		std::cout << "(MiyaChainMSG) : getdata" << "\n";
		struct MiyaChainMSG_GETDATA payload;
	}

	else if( memcmp( _header._command, MiyaChainMSG_GETHEADERS::command , sizeof(_header._command) ) == 0 )
	{
		std::cout << "(MiyaChainMSG) : getheaders" << "\n";
		struct MiyaChainMSG_GETHEADERS payload;

	}
	
	else if( memcmp( _header._command , MiyaChainMSG_HEADERS::command , sizeof(_header._command) ) == 0 )
	{
		std::cout << "(MiyaChainMSG) : headers" << "\n";
		struct MiyaChainMSG_HEADERS payload;
		
	}

	else if( memcmp(_header._command, MiyaChainMSG_MEMPOOL::command , sizeof(_header._command) ) == 0 )
	{
		std::cout << "(MiyaChainMSG) : mempool" << "\n";
		struct MiyaChainMSG_MEMPOOL paload;
		
	}

	else if( memcmp( _header._command, MiyaChainMSG_BLOCK::command , sizeof(_header._command) )  == 0 )
	{
		std::cout << "(MiyaChainMSG) : block" << "\n";
		struct MiyaChainMSG_BLOCK payload;
	}

	else if( memcmp(_header._command, MiyaChainMSG_NOTFOUND::command , sizeof(_header._command)) == 0  )
	{
		std::cout << "(MiyaChainMSG) : notfound" << "\n";
		struct MiyaChainMSG_NOTFOUND payload;
	}

	else
	{
		std::cout << "(MiyaChainMSG) : undefined" << "\n";
	}

}







};
