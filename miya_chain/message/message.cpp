#include "message.h"



namespace miya_chain
{






size_t MiyaChainMessage::payloadLength()
{
	return static_cast<size_t>(_header._payloadLength);
}


const char *MiyaChainMessage::command()
{
	return _header._command;
}


MiyaChainCommand MiyaChainMessage::payload()
{
	return _payload;
}


void MiyaChainMessage::payload( MiyaChainCommand targetPayload , const char* comand  )
{
	memcpy( _header._command , comand , sizeof(_header._command) );
	_payload = targetPayload;
}

void MiyaChainMessage::payloadLength( size_t target )
{
	_header._payloadLength = static_cast<uint32_t>(target); // エンディアンん変換はしない
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





size_t MiyaChainMessage::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
	std::shared_ptr<unsigned char> rawPayload; size_t rawPayloadLength;
	rawPayloadLength = exportRawCommand( _header._command , _payload ,&rawPayload );
	this->payloadLength( rawPayloadLength );

	size_t retLength = rawPayloadLength + sizeof(_header);
	*retRaw = std::shared_ptr<unsigned char>( new unsigned char[retLength] );

	size_t formatPtr = 0;
	memcpy( (*retRaw).get() , &_header , sizeof(_header) ); formatPtr += sizeof(_header);
	memcpy( (*retRaw).get() + formatPtr , rawPayload.get() , rawPayloadLength ); formatPtr += rawPayloadLength;

	return formatPtr;
}




size_t MiyaChainMessage::exportRawCommand( const char* command , MiyaChainCommand commandBody ,std::shared_ptr<unsigned char> *retRaw )
{

	if( memcmp( command , MiyaChainMSG_INV::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // INV
	{
		return 0;
	}
	else if( memcmp( command , MiyaChainMSG_BLOCK::command , MIYA_CHAIN_MSG_COMMAND_LENGTH )  == 0 ) // BLOCK
	{
		return 0;
	}
	else if( memcmp( command, MiyaChainMSG_GETBLOCKS::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // GETBLOCKS
	{
		std::cout << "Passed Here" << "\n";
		return (std::get<MiyaChainMSG_GETBLOCKS>(commandBody)).exportRaw( retRaw );
	}
	else if( memcmp( command, MiyaChainMSG_GETDATA::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // GETDATA
	{
		return 0;
	} 
	else if( memcmp( command, MiyaChainMSG_GETHEADERS::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // GETHEADERS
	{
		return 0;
	}
	else if( memcmp( command , MiyaChainMSG_HEADERS::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // HEADERS
	{
		return 0;
	}
	else if( memcmp( command, MiyaChainMSG_MEMPOOL::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // MEMPOOL
	{
		return 0;
	}
	else if( memcmp( command, MiyaChainMSG_BLOCK::command , MIYA_CHAIN_MSG_COMMAND_LENGTH )  == 0 ) // BLOCK
	{
		return 0;
	}
	else if( memcmp( command, MiyaChainMSG_NOTFOUND::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 ) // NOTFOUND
	{
		return 0;
	}
	else{
		std::cout << "(コマンド書き出しエラー) 一致するコマンドがありません"  << "\n";
		*retRaw = nullptr;
		return 0;
	}


}


int MiyaChainMessage::commandIndex( const char* command )
{
	if( memcmp( command , MiyaChainMSG_INV::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 )
		return 0;
	else if( memcmp( command , MiyaChainMSG_BLOCK::command , MIYA_CHAIN_MSG_COMMAND_LENGTH )  == 0 )
		return 1;
	else if( memcmp( command, MiyaChainMSG_GETBLOCKS::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 )
		return 2;
	else if( memcmp( command, MiyaChainMSG_GETDATA::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 )
		return 3;
	else if( memcmp( command, MiyaChainMSG_GETHEADERS::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 )
		return 4;
	else if( memcmp( command , MiyaChainMSG_HEADERS::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 )
		return 5;
	else if( memcmp( command, MiyaChainMSG_MEMPOOL::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0 )
		return 6;
	else if( memcmp( command, MiyaChainMSG_BLOCK::command , MIYA_CHAIN_MSG_COMMAND_LENGTH )  == 0 )
		return 7;
	else if( memcmp( command, MiyaChainMSG_NOTFOUND::command , MIYA_CHAIN_MSG_COMMAND_LENGTH ) == 0  )
		return 8;

	else
		return -1;

}



};
