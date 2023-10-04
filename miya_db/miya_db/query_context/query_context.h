#ifndef B1C6FBC6_3369_4C02_BB15_4B28B111601E
#define B1C6FBC6_3369_4C02_BB15_4B28B111601E



#include <iostream>
#include <memory>


namespace miya_db
{









struct QueryContext
{
private:
	const int _type;


	// 考慮すべき格納データ
	// 1. add( key , data , dataLength )
	// 2. update( key , data , dataLength )
	// 3. select( key )

	struct 
	{
		std::shared_ptr<unsigned char> _key;
		size_t _keyLength;

		std::shared_ptr<unsigned char> _value;
		size_t _valueLength;

	} _data;


public:
	QueryContext( int type ) : _type(type){};

	int type(){ return _type; };
	void key( std::shared_ptr<unsigned char> key , size_t keyLength );
	void value( std::shared_ptr<unsigned char> data , size_t dataLength );


	std::shared_ptr<unsigned char> key();
	size_t keyLength();

	std::shared_ptr<unsigned char> value();
	size_t valueLength();
};






};



#endif // B1C6FBC6_3369_4C02_BB15_4B28B111601E



