#ifndef C461E28B_5551_4B4A_893A_6A7E6AE8789F
#define C461E28B_5551_4B4A_893A_6A7E6AE8789F


#include <iostream>
#include <vector>


namespace tx{



class Script{


protected:
	unsigned short OPDataSize( unsigned char target );
	std::vector< std::pair< unsigned char , void *> > _script; // protectedメンバーへ

public:
																														 

	Script(){};

	// decode
	Script( unsigned char* rawScript , unsigned int rawScriptSize );
	int scriptCnt(); // getter

	// encode( export )
	unsigned int exportRaw( unsigned char** ret  ); // return retSize with unsigned int 
	unsigned int exportRawSize();

	
	int takeInScript( unsigned char* from , unsigned int fromSize );

};



}; // close tx namespace


#endif // C461E28B_5551_4B4A_893A_6A7E6AE8789F
