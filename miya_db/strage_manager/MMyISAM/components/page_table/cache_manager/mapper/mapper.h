#ifndef CC58B2AA_3145_4696_ACF1_029658A27A58
#define CC58B2AA_3145_4696_ACF1_029658A27A58


#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cmath>
#include <math.h>


namespace miya_db{


const unsigned int DEFAULT_FRAME_CACHE_SIZE = std::pow( 2, 2*8 );


class Mapper
{


//private:
public:
	int _fd;
	//off_t _currentFileSize;
	long _systemPageSize;
	off_t _currentOffset;
	struct stat _fileStat;

public:
	Mapper( int fd  );
	int fd(){ return _fd;};

	void* map( unsigned int offsetIdx ); 
	int unmap( void *targetMappedPtr );
	int sync( void* targetMappedPtr ); // 同期
																								 
	void *expandRegion( unsigned int fileSize );

};



};


#endif // CC58B2AA_3145_4696_ACF1_029658A27A58

