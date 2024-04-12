#include "unit_test.h"

#include "../stream_buffer.h"
#include "../stream_buffer_container.h"




void streamBuffer_UnitTest()
{
	std::shared_ptr<StreamBufferContainer> sbContainer = std::make_shared<StreamBufferContainer>();



	std::thread th1([&]()
	{	
		std::cout << "th1 started" << "\n";
		for(int i=0; i<5; i++ )
		{
			usleep(500000);
			std::unique_ptr<SBSegment> segment = std::make_unique<SBSegment>();
			segment->_controlBlock._reserved = htons(10);

			int num = 100;
			segment->options.option1 = num;

			sbContainer->pushOne( std::move(segment) );
			std::cout << ">>> push" << "\n";
		}
	
	});



	std::thread th3([&](){
		std::cout << "th3 started" << "\n";
		for( int i=0; i<5; i++ ){
			usleep(500000);
			std::unique_ptr<SBSegment> segment = std::make_unique<SBSegment>();
			segment->_controlBlock._reserved = htons(30);
			sbContainer->pushOne( std::move(segment) );
			std::cout << "->->-> push" << "\n";
		}
	});



	/* --------------------------------------- */


	std::thread th2([&]()
	{
		std::cout << "th2 started" << "\n";
		for(int i=0; i<5; i++ )
		{
			usleep(3000000);
			std::unique_ptr<SBSegment> segment;
			segment = sbContainer->popOne();
			std::cout << "\n======================" << "\n";
			std::cout << "poped <- " << ntohs( segment->_controlBlock._reserved ) << "\n";
			if( segment->options.option1.type() == typeid(int) ){
			std::cout << "casted" << "\n";
				std::cout << std::any_cast<int>(segment->options.option1) << "\n";
			}

			std::cout << "======================" << "\n";
		}
	});




	std::thread th4([&](){
		std::cout << "th4 started" << "\n";
		for( int i=0; i<5; i++ )
		{
			usleep(3000000);
			std::unique_ptr<SBSegment> segment;
			segment = sbContainer->popOne();
			std::cout << "\n======================" << "\n";
			std::cout << "poped <- " << ntohs( segment->_controlBlock._reserved ) << "\n";
			std::cout << "======================" << "\n";

		}

	});



	th1.join();
	th2.join();
	th3.join();
	th4.join();

	

	return;
}

