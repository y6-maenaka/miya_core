#ifndef A286557D_4052_4B52_9CDD_2A969E8BEA1F
#define A286557D_4052_4B52_9CDD_2A969E8BEA1F

#include <iostream>
#include <memory>
#include <thread>
#include <vector>



class StreamBuffer;
class StreamBufferContainer;



namespace ekp2p
{



constexpr int EKP2P_SEND_UNICAST = 0;
constexpr int EKP2P_SEND_BROADCAST = 1;



class KRoutingTable;
class KNodeAddr;





struct EKP2PSenderOptions
{
public:
	int castType;
	size_t castSize = 0;
	std::vector< std::shared_ptr<ekp2p::KNodeAddr> > destinationNodeAddr;
};







// --------------------------------------------------------------------------------










class Sender
{

private:
	std::shared_ptr<StreamBufferContainer> _incomingSB;
	std::shared_ptr<KRoutingTable> _kRoutingTable;

	std::vector<std::thread::id> _activeSenderThreadIDVector; // 念の為管理しておく

public:
	Sender( std::shared_ptr<KRoutingTable> kRoutingTable ,std::shared_ptr<StreamBufferContainer> incomingSB );

	void start(); // ノードへの送信リクエストなどは全てStreamBufferを介して行う
};







};




#endif // A286557D_4052_4B52_9CDD_2A969E8BEA1F



