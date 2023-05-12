#include "receive_handler.h"


namespace ekp2p{

ReceiveHandler::ReceiveHandler(){
	_bootHandler = NULL;
}



void ReceiveHandler::test(){
	std::cout << "[ TEST MSG ] In ReceiveHandler | LINE::" << __LINE__ << "\n";
}



} // close ekp2p namespace
