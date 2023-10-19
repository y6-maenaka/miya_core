#ifndef CD3D3C5E_6902_4CE2_A46C_9142BF794829
#define CD3D3C5E_6902_4CE2_A46C_9142BF794829


#include <memory>
#include <string>
#include <variant>
#include <iostream>
#include <any>
#include <algorithm>




const std::string COMMAND_TO_SENDER = "ekp2p_send";
const std::string COMMAND_TO_RECEIVER = "ekp2p_receive";
const std::string COMMAND_TO_K_ROUTING_TABLE_UPDATOR = "ekp2p_k_routing_table_update";





struct SBSegment;
class StreamBufferContainer;


namespace tx
{
	struct P2PKH;
}




class ControlInterface
{


private:
	struct
	{
		std::shared_ptr<StreamBufferContainer> _toSenderSB;
		std::shared_ptr<StreamBufferContainer> _toReceiverSB;
		std::shared_ptr<StreamBufferContainer> _toKRoutingTableUpdatorSB;
	}	_ekp2pMainDaemons;


public:

	void start();

	void command_exe( std::string command , std::shared_ptr<unsigned char> rawConetnt );
	std::shared_ptr<tx::P2PKH> createTxFromJsonFile( const char *filePath );

	
	void toSenderSB( std::shared_ptr<StreamBufferContainer> target );
};




#endif // CD3D3C5E_6902_4CE2_A46C_9142BF794829



