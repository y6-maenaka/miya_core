#include "database_manager.h"

#include "../../shared_components/stream_buffer/stream_buffer.h"
#include "../../shared_components/stream_buffer/stream_buffer_container.h"


#include "../strage_manager/MMyISAM/MMyISAM.h"

namespace miya_db{






void DatabaseManager::loadInformationSchema( char *path )
{
	FILE *schema = nullptr;

	//fopen( path , "");
}




void DatabaseManager::startQueryHandleThread( bool isAdditionalThread )
{
	/*
	std::thread queryHanalder( [&](){
		std::cout << "query handler started" << "\n";

		unsigned char* segment;  unsigned int segmentSize;

		int availableSpace = 0;

		if( isAdditionalThread ){
			for( int i=0; i<3; i++) // 3回ブロッキングなしでバッファをチェックしていずれも空だったら,そのスレッドは破棄する
			{ 
			 	availableSpace =	_inbandMiddleBuffer->popOne() &segment , &segmentSize , false ;
				if( avaiableSpace > 0 ) break;
				else continue;

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			if( avaiableSpace <= 0 ) return; // ３回バッファに問い合わせても空だったらリターンする

		}else{
			availableSpace = _inbandMiddleBuffer->popOne( &segment , &segmentSize , true );
		}

		if( (( avaiableSpace / _inbandMiddleBuffer->_bufferSize )*100)  >= 30 ){ // バッファ使用率が任意の値を超えたら
			_startQueryHandleThread( true ); // 追加ハンドラの起動
		}

			
		QueryPack *queryPack = new QueryPack;
		queryPack->importRaw( segment , segmentSize );
		delete segment;


		
		delete queryPack;
	});
	*/
}




void DatabaseManager::startWithLightMode( std::shared_ptr<StreamBufferContainer> sbContainer, std::string fileName )
{
	std::cout << "Started MiyaDB [ Light Mode ]" << "\n";


	// respondスレッドを用意する


	

}





}; // close miya_db namespace

