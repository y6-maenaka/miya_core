#include <core/core.hpp>


namespace core
{


/* void core::message_broker::on_income_message( ss::message_pool::_message_ msg )
{
  auto miya_msg_j = msg.get_param("core"); // 本アプリケーションメッセージの取り出し
  auto miya_msg = chain::decode( miya_msg_j );

  if( !(miya_msg.is_valid()) ) return; // 本来はちゃんとバリデーションする
  switch( miya_msg.get_command_type() ) // 主要マネージャーに移譲する
  {
	case chain::MiyaChainMessage::command_type::MiyaCoreMSG_BLOCK :
	{
	  this->income_command_block();
	  break;
	};

	case chain::MiyaCHianMessage::command_type::MiyaCore_GETDATA :
	{
	  // this->income_command_getdata();
	  break;
	};

	default :
	{
	  break;
	};
  };

  std::cout << "(recv) <- " << msg.src_ep << "\n";
  std::cout << "message received" << "\n";
} */


}; // namespace core
