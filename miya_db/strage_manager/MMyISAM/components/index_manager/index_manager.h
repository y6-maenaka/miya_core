#ifndef D4434870_B0A9_4BF6_BF2D_11DC9C256726
#define D4434870_B0A9_4BF6_BF2D_11DC9C256726


#include <memory>


namespace miya_db
{


class optr;

constexpr int DATA_OPTR_LOCATED_AT_NORMAL = 0;
constexpr int DATA_OPTR_LOCATED_AT_SAFE = 1;



class IndexManager
{

public:
    virtual void add( std::shared_ptr<unsigned char> key , std::shared_ptr<optr> dataOptr ) = 0;
    virtual void remove( std::shared_ptr<unsigned char> key ) = 0;
    virtual std::pair< std::shared_ptr<optr>, int > find( std::shared_ptr<unsigned char> key ) = 0;
		// dataOptrの本体の所在が,ノーマルファイルかセーフファイルかのフラグ



    // debug
    virtual void printIndexTree() = 0;
};








};






#endif // D4434870_B0A9_4BF6_BF2D_11DC9C256726
