#include "__test.h"

#include "./message.h"
#include "./command/command_set.h"



void MiyaChainMessageTest()
{

 
	std::cout << ">>>>>>> " << sizeof(size_t) << "\n";

	std::cout << "\n\n----------- DEBUG -----------\n";
	std::cout << "HelloWorld" << "\n";

	miya_chain::MiyaChainMessage msgPackingGetBlosk;
	miya_chain::MiyaChainMSG_GETBLOCKS getbloks;
	msgPackingGetBlosk.payload( getbloks ,miya_chain::MiyaChainMSG_GETBLOCKS::command );
	std::cout << "msg type :: " << msgPackingGetBlosk.commandIndex() << "\n";
	std::shared_ptr<unsigned char> rawMSG; size_t rawMSGLength;
	rawMSGLength = msgPackingGetBlosk.exportRaw( &rawMSG );
	std::cout << "Raw MSG(" << rawMSGLength << "):: ";
	for( int i=0; i<rawMSGLength ; i++){
		printf("%02X", rawMSG.get()[i]);
	} std::cout << "\n";




	miya_chain::MiyaChainMessage msgPackingNotFound;
	miya_chain::MiyaChainMSG_NOTFOUND notfound;
	std::shared_ptr<unsigned char> rawNotFound; size_t rawNotFoundLength;
	rawNotFoundLength = notfound.exportRaw( &rawNotFound );
	std::cout << "Raw notfound(" << rawNotFoundLength << ") :: ";
	for( int i=0; i<rawNotFoundLength; i++){
		printf("%02X",  rawNotFound.get()[i]);
	} std::cout << "\n";

	msgPackingNotFound.payload( notfound ,miya_chain::MiyaChainMSG_NOTFOUND::command );
	std::cout << "msg type :: " << msgPackingNotFound.commandIndex() << "\n";

	rawMSGLength = msgPackingNotFound.exportRaw( &rawMSG );
	std::cout << "Raw MSG(" << rawMSGLength << "):: ";
	for( int i=0; i<rawMSGLength ; i++){
		printf("%02X", rawMSG.get()[i]);
	} std::cout << "\n";


	std::cout << "-----------------------------\n\n";
}
