#include "connection_controller.h"

#include "./k_node.h"


namespace ekp2p
{






std::shared_ptr<KNodeAddr> ConnectionController::outsideGlobal( void *rawPayload )
{
	// とりあえず
	KNodeAddr ret;
	memcpy( &ret , rawPayload , sizeof(ret) );

	return std::make_shared<KNodeAddr>( ret );
}









};
