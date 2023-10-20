#ifndef AD837352_92D6_438E_96E5_662363E55A01
#define AD837352_92D6_438E_96E5_662363E55A01




#include <iostream>
#include <memory>



namespace ekp2p
{



struct EKP2PMessageHeader;




struct EKP2PMessage
{
private:
	std::shared_ptr<EKP2PMessageHeader> _header;
	std::shared_ptr<unsigned char> _payload;


public:

	EKP2PMessage();

	void payload( std::shared_ptr<unsigned char> payload , size_t payloadLength );
	size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );

	void protocol( int type );


	// Getter
	std::shared_ptr<EKP2PMessageHeader> header();


	// Setter
	void payload( std::shared_ptr<unsigned char> target );
	std::shared_ptr<unsigned char> payload();

	void printRaw();
};



};


#endif // AD837352_92D6_438E_96E5_662363E55A01



