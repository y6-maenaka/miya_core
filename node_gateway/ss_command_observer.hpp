#ifndef BC81A970_4146_4D67_854F_EF0B6FC1267B
#define BC81A970_4146_4D67_854F_EF0B6FC1267B


#include "boost/asio.hpp"

#include <share/observer/observer.hpp>


using namespace boost::asio;


class getdata : public base_observer // 基本的に未知のブロック収集に使用
{
public:
  getdata( io_context &io_ctx );
};



#endif 
