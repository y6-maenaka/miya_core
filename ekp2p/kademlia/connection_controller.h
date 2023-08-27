#ifndef E87D89B4_52BD_44AE_8C24_D56A085F8E60
#define E87D89B4_52BD_44AE_8C24_D56A085F8E60


#include <memory>


namespace ekp2p
{

class SmartMiddleBuffer;
class SocketManager;
class KNodeAddr;










class ConnectionController
{
private:
	SmartMiddleBuffer *middleBuffer;


public:
	void start();

	static std::shared_ptr<KNodeAddr> outsideGlobal( void *rawPayload );

};






};







#endif // E87D89B4_52BD_44AE_8C24_D56A085F8E60

