#ifndef C4EB1366_9BEB_4552_AB53_FE075A04AB1E
#define C4EB1366_9BEB_4552_AB53_FE075A04AB1E



#include "./base_command.h"


namespace miya_chain
{


struct MiyaCHainMSG_TX
{
public:
    static constexpr unsigned char command[12] = "tx";

		size_t exportRaw( std::shared_ptr<unsigned char> *retRaw );
		bool importRaw( std::shared_ptr<unsigned char> fromRaw , size_t fromRawLength );


}

}




#endif // C4EB1366_9BEB_4552_AB53_FE075A04AB1E



