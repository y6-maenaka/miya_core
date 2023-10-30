#include "inv.h"


namespace miya_chain
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




size_t MiyaChainMSG_INV::exportRaw( std::shared_ptr<unsigned char> *retRaw )
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



void MiyaChainMSG_INV::add( struct inv target )
{
    _body._invVector.push_back( target );
    this->count( this->count() + 1 );
}



size_t MiyaChainMSG_INV::count()
{
    return static_cast<size_t>(_body._count);
}






void MiyaChainMSG_INV::count( size_t count )
{
    _body._count = static_cast<uint32_t>( count );
}

void MiyaChainMSG_INV::addTx( std::shared_ptr<unsigned char> hash )
{
    struct inv target;
    target._typeID = static_cast<unsigned short>(TypeID::MSG_TX);
    target.hash( hash );

    this->add( target );
}


void MiyaChainMSG_INV::addBlock( std::shared_ptr<unsigned char> hash )
{
    struct inv target;
    target._typeID = static_cast<unsigned short>(TypeID::MSG_BLOCK);
    target.hash( hash );

    this->add( target );
}


std::vector<struct inv> MiyaChainMSG_INV::invVector()
{
    return _body._invVector;
}


};