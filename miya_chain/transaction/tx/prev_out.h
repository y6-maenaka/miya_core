#ifndef DE5677D0_654D_474B_A2A3_F555DC3E3A7F
#define DE5677D0_654D_474B_A2A3_F555DC3E3A7F


#include <stdlib.h>
#include <memory>
#include <variant>
#include <string.h>
#include "openssl/evp.h"
#include <algorithm>
#include <iostream>
#include <endian.h>


#include "../../../shared_components/json.hpp"
using json = nlohmann::json;




namespace tx
{

struct PrevOut
{
//private:
public:
	struct __attribute__((packed))
	{
		std::shared_ptr<unsigned char> _txID; // 参照するトランザクションID
		uint32_t _index; // 参照するtx_outのインデックス
	} _body;

//public:
	PrevOut();

	std::shared_ptr<unsigned char> txID();
	void txID( std::shared_ptr<unsigned char> target );
	void txID( const unsigned char *target );
	unsigned short index();
	void index( int target );
	void index( uint32_t target );

	unsigned int exportRaw( std::shared_ptr<unsigned char> *retRaw );
	//int importRaw( std::shared_ptr<unsigned char> fromRaw );
	int importRaw( unsigned char* fromRaw );
	
};


};


#endif // 


