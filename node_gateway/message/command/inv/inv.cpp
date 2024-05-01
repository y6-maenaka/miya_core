#include "inv.hpp"


namespace chain
{


void inv::hash( std::shared_ptr<unsigned char> target )
{
    if( target == nullptr )
        memset( _hash , 0x00 , sizeof(_hash) );
    else
        memcpy( _hash , target.get() , sizeof(_hash) );
}

unsigned char* inv::hash()
{
    return _hash;
}

inv::inv( unsigned short typeID , std::shared_ptr<unsigned char> fromHash )
{
    _typeID = typeID;
    this->hash( fromHash );
}

inv::inv( std::string type , std::shared_ptr<unsigned char> fromHash )
{
    if( type == "TX" ) _typeID = static_cast<unsigned short>(TypeID::MSG_TX);
    else if( type == "BLOCK" ) _typeID = static_cast<unsigned short>(TypeID::MSG_BLOCK);

    this->hash( fromHash );
}




size_t MiyaCoreMSG_INV::exportRaw( std::shared_ptr<unsigned char> *retRaw )
{
    size_t retRawLength = sizeof(_body._count) + (sizeof(struct inv) * _body._invVector.size() );
    *retRaw = std::shared_ptr<unsigned char>( new unsigned char[retRawLength] );
    this->count( _body._invVector.size() ); // inv数の最終更新

    size_t formatPtr = 0;
    // countの書き出し
    memcpy( (*retRaw).get() + formatPtr , &(_body._count), sizeof(_body._count) ); formatPtr += sizeof(_body._count);

    // 各invの書き出し
    for( auto itr : _body._invVector ){
        memcpy( (*retRaw).get() + formatPtr , &itr, sizeof(itr) ); formatPtr += sizeof(itr);
    }

    return formatPtr;
}

void MiyaCoreMSG_INV::add( struct inv target )
{
    _body._invVector.push_back( target );
    this->count( this->count() + 1 );
}

size_t MiyaCoreMSG_INV::count()
{
    return static_cast<size_t>(_body._count);
}

void MiyaCoreMSG_INV::__print()
{
    printf(" .... %s .... \n" , MiyaCoreMSG_INV::command );

    int i=0;
    for( int i=0; i<_body._invVector.size(); i++ ){
        std::cout << "(" << i << ") ";
        std::cout << "type :: " << _body._invVector.at(i)._typeID << "\n";
        auto _ = _body._invVector.at(i);
        for( int j=0; j<32; j++ ){
            printf("%02X", _.hash()[j] );
        } std::cout << "\n\n";
    }
}

void MiyaCoreMSG_INV::count( size_t count )
{
    _body._count = static_cast<uint32_t>( count );
}

void MiyaCoreMSG_INV::addTx( std::shared_ptr<unsigned char> hash )
{
    struct inv target = inv( static_cast<unsigned short>(TypeID::MSG_TX), hash );
    // target._typeID = static_cast<unsigned short>(TypeID::MSG_TX);
    // target.hash( hash );
    this->add( target );
}


void MiyaCoreMSG_INV::addBlock( std::shared_ptr<unsigned char> hash )
{
    struct inv target = inv( static_cast<unsigned short>(TypeID::MSG_BLOCK) , hash );
    // target._typeID = static_cast<unsigned short>(TypeID::MSG_BLOCK);
    // target.hash( hash );
    this->add( target );
}


std::vector<struct inv> MiyaCoreMSG_INV::invVector()
{
    return _body._invVector;
}


};
