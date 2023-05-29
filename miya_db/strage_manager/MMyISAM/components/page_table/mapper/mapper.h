#ifndef CC58B2AA_3145_4696_ACF1_029658A27A58
#define CC58B2AA_3145_4696_ACF1_029658A27A58


#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>


namespace miya_db{


class Mapper
{


private:
	int _fd;
	off_t _currentFileSize;
	long _systemPageSize;


public:
	Mapper( int fd  );


	void* map( unsigned short pageIdx ); 
	void *unmap( void *targetAddr );
	void sync( void* addr , unsigned int lenght ); // 同期
																								 
	void *expandRegion( unsigned int fileSize );

};



};


#endif // CC58B2AA_3145_4696_ACF1_029658A27A58

