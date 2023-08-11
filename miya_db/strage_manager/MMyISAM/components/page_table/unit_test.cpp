#include "unit_test.h"




#include "./overlay_ptr.h"
#include "./overlay_memory_manager.h"
#include "./cache_manager/cache_table.h"
#include "./cache_manager/mapper/mapper.h"





int common_test()
{


	int fd = open("../miya_db/table_files/test_table/test.oswap", O_RDWR , (mode_t)0600 );

	miya_db::OverlayMemoryManager *manager = new miya_db::OverlayMemoryManager(fd);





	return 0;
}




