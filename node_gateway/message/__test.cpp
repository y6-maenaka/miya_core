#include "__test.h"

#include "./message.hpp"
#include "./command/command_set.h"


/*
void MiyaChainMessageTest()
{
	std::cout << ">>>>>>> " << sizeof(size_t) << "\n";

	std::cout << "\n\n----------- DEBUG -----------\n";
	std::cout << "HelloWorld" << "\n";

	chain::MiyaChainMessage msgPackingGetBlosk;
	chain::MiyaCoreMSG_GETBLOCKS getbloks;
	msgPackingGetBlosk.payload( getbloks ,chain::MiyaCoreMSG_GETBLOCKS::command );
	std::cout << "msg type :: " << msgPackingGetBlosk.commandIndex() << "\n";
	std::shared_ptr<unsigned char> rawMSG; size_t rawMSGLength;
	rawMSGLength = msgPackingGetBlosk.exportRaw( &rawMSG );
	std::cout << "Raw MSG(" << rawMSGLength << "):: ";
	for( int i=0; i<rawMSGLength ; i++){
		printf("%02X", rawMSG.get()[i]);
	} std::cout << "\n";




	chain::MiyaChainMessage msgPackingNotFound;
	chain::MiyaCoreMSG_NOTFOUND notfound;
	std::shared_ptr<unsigned char> rawNotFound; size_t rawNotFoundLength;
	rawNotFoundLength = notfound.exportRaw( &rawNotFound );
	std::cout << "Raw notfound(" << rawNotFoundLength << ") :: ";
	for( int i=0; i<rawNotFoundLength; i++){
		printf("%02X",  rawNotFound.get()[i]);
	} std::cout << "\n";

	msgPackingNotFound.payload( notfound ,chain::MiyaCoreMSG_NOTFOUND::command );
	std::cout << "msg type :: " << msgPackingNotFound.commandIndex() << "\n";

	rawMSGLength = msgPackingNotFound.exportRaw( &rawMSG );
	std::cout << "Raw MSG(" << rawMSGLength << "):: ";
	for( int i=0; i<rawMSGLength ; i++){
		printf("%02X", rawMSG.get()[i]);
	} std::cout << "\n";


	std::cout << "-----------------------------\n\n";
}
*/
