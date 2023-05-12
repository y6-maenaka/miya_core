#ifndef B2ADB188_56EE_443E_B27A_29693735AF9E
#define B2ADB188_56EE_443E_B27A_29693735AF9E



namespace miya_db{

class Client;



class SocketManager
{

private:
	int _sock;
	

public:

	int create();
	int bind();

};





class TCPSocketManager : protected SocketManager
{

private:

public:


};



}; // close miya_db namespace 


#endif // B2ADB188_56EE_443E_B27A_29693735AF9E
