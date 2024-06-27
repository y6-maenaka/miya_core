#include <core/net_processing.hpp>
#include <json.hpp>
#include <node_gateway/message/message.hpp>
#include <node_gateway/services.hpp>
#include <chain/chain_manager.hpp>

using json = nlohmann::json;


namespace core
{


net_processing::net_processing( chain::chain_manager &chain_manager, chain::mempool &mempool, ss::message_pool &message_pool ) : 
  _chain_manager( chain_manager )
  , _mempool( mempool )
  , _message_pool( message_pool )
{
  _message_hub = &(_message_pool.get_message_hub());
}

void net_processing::start()
{ 
  return _message_hub->start( std::bind( &net_processing::process_message, this, std::placeholders::_1, std::placeholders::_2) );
}

void net_processing::process_message( ss::peer::ref peer, ss::ss_message::ref income_msg )
{
  json dumped_msg = income_msg->get( "miya"/*仮*/ ); // 本アプリケーションのメッセージを取り出す
  chain::miya_core_message::ref income_miya_msg = std::make_shared<chain::miya_core_message>(dumped_msg);

  switch( income_miya_msg->get_services() )
  {
	case MIYA_SERVICES::NETWORKING :
	  {
		std::cout << "this is networking request msg" << "\n";
	  }
	case MIYA_SERVICES::TRANSACTION :
	  {
		std::cout << "this is transaction request msg" << "\n";

		/*
		 Chain Manager に転送するmiya_core_command
		 // inv : 
		*/
		switch( income_miya_msg->get_command_type() )
		{
		  case chain::miya_core_command::command_type::INV :
			// ノードが他ノードに自分が持っているブロックやトランザクションのハッシュリストを通知する
		  {
			// INVの中身 : TX -> mempool
			// INVの中身 : BLOCK -> chain_sync_manager
			
			auto &command_inv = income_miya_msg->get_command<chain::MiyaCoreMSG_INV>(); // メッセージからコマンド本体の取り出し
			auto inv_tx_vector = command_inv.pick_inv_by_key( chain::inv::type_id::MSG_TX );
			auto inv_block_vector = command_inv.pick_inv_by_key( chain::inv::type_id::MSG_BLOCK );

			/*
			tx : 新規発行のトランザクションの通知等
			block : 新たなブロックの通知
			*/

			// _mempool.add( inv_tx_vector ); // mempoolに追加
			_chain_manager.income_command_block_invs( peer, inv_block_vector );

			break;
		  };
		  case chain::miya_core_command::command_type::BLOCK :
			// 実際のブロックデータ本体を送信する(GETDATAのレスポンスに用いられる)
		  {
			break;
		  }
		  case chain::miya_core_command::command_type::GETBLOCKS :
			// ノードが他のノードに対して、特定の範囲のブロックのハッシュをリクエストするため
			// 特定のブロックから最新のブロックまでのハッシュリストを取得するために使用
		  {
			break;
		  }
		  case chain::miya_core_command::command_type::GETHEADERS :
			// ノードが他のノードに対して、特定の範囲のブロックヘッダーをリクエストするため
			// 特定のヘッダーから最新のブロックまでのハッシュリストを取得するために使用(bitcoin coreでは最大2000個を一回にリクエストできる)
		  {
			break;
		  }
		  case chain::miya_core_command::command_type::HEADERS : 
			// GETHEADERSメッセージに応答して、ブロックヘッダーのリストを送信するため
		  {
			break;
		  }
		  case chain::miya_core_command::command_type::MEMPOOL :
			// ノードが他のノードに対して、そのノードのメモリプール（未確認トランザクションのプール）内のトランザクションのリストをリクエストするため
			// 新規にネットワークに参加した時などに使用
		  {
			break;
		  }
		  case chain::miya_core_command::command_type::NOTFOUND :
			// ノードが他のノードから要求されたデータが見つからない場合に、そのことを通知するため
			// 見つからなかったトランザクションやブロックのハッシュリストが乗る
		  {
			break;
		  }
		  default :
		  {
			return;
		  }
		};
	  }
	case MIYA_SERVICES::API :
	  {
		std::cout << "this is api request msg" << "\n";
	  } 
  };

  // ルーティングの優先順位
  // 1. サービス
  // 2. コマンド
  // 3> 

}


};
