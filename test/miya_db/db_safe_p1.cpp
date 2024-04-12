#include <iostream>
#include <memory>
#include <string>

#include "./common.h"

#include "../../miya_db/miya_db/database_manager.h"

#include "../../share/stream_buffer/stream_buffer.h"
#include "../../share/stream_buffer/stream_buffer_container.h"
#include "../../share/miya_db_client/miya_db_sb_client.h"




/*
 [ セーフモード並列起動テスト ]

 - セーフモード 1,2,3 の起動
*/






int db_safe_p1( std::string dbName )
{
  std::cout << "テストケース [ DB_SAFE_P2 ]" << "\n";


  std::shared_ptr<miya_db::DatabaseManager> dbManager = std::make_shared<miya_db::DatabaseManager>();
  std::shared_ptr<StreamBufferContainer> toDBSBC = std::make_shared<StreamBufferContainer>(); // DB行きSBCセットアップ
  std::shared_ptr<StreamBufferContainer> fromDBSBC = std::make_shared<StreamBufferContainer>(); // DB戻りのSBCセットアップ

  dbManager->startWithLightMode( toDBSBC , fromDBSBC , dbName ); // データベース名を指定して簡易モードで起動

  std::shared_ptr<MiyaDBSBClient> dbClient = std::make_shared<MiyaDBSBClient>( toDBSBC , fromDBSBC ); // データベースクライアントのセットアップ




  bool flag;
  std::shared_ptr<unsigned char> rawResponse = nullptr;
  size_t rawResponseLength = 0;
  short registryWith_1 = 2;
  short registryWith_2 = 3;
  short registryWith_3 = 4;

  // 以降がデータ追加シーケンス
  // <データ1>
  auto storeTargetPair = generateDBKeyValuePair( "bbbbbbbbbbbbbbbbbbbb", "HelloWorld1", 11 );
  std::shared_ptr<unsigned char> key_1 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 );
  dbClient->get( storeTargetPair.first , &rawResponse );
  std::cout << "保存結果(1) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";
  // <データ2>
  storeTargetPair = generateDBKeyValuePair( "zzzzzzzzzzzzzzzzzzzz", "HelloWorld2", 11 );
  std::shared_ptr<unsigned char> key_2 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 );
  dbClient->get( storeTargetPair.first , &rawResponse );
  std::cout << "保存結果(2) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";



  //　ここからセーフモードに移行する
  flag =dbClient->safeMode(registryWith_1 );
  std::cout << "SafeModeStatus(2) :: " << flag << "\n";
  std::cout << "\n\n\n\n\n\n\n\n";



  // <データ3>
  storeTargetPair = generateDBKeyValuePair( "aaaaaaaaaaaaaaaaaaaa", "HelloWorld3", 11 );
  std::shared_ptr<unsigned char>  key_3 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_1 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_1 );
  std::cout << "保存結果(3) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";
  // <データ4>
  storeTargetPair = generateDBKeyValuePair( "cccccccccccccccccccc", "HelloWorld4", 11 );
  std::shared_ptr<unsigned char> key_4 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_1 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_1 );
  std::cout << "保存結果(4) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";



  flag = dbClient->safeMode( registryWith_2 );
  std::cout << "SafeModeStatus(3) :: " << flag << "\n";
  std::cout << "\n\n\n\n\n\n\n\n";



  // <データ5>
  storeTargetPair = generateDBKeyValuePair( "gggggggggggggggggggg", "HelloWorld5", 11 );
  std::shared_ptr<unsigned char> key_5 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_2 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_2 );
  std::cout << "保存結果(5) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";
  // <データ6>
  storeTargetPair = generateDBKeyValuePair( "ffffffffffffffffffff", "HelloWorld6", 11 );
  std::shared_ptr<unsigned char> key_6 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_2 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_2 );
  std::cout << "保存結果(6) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";
  // <データ7>
  storeTargetPair = generateDBKeyValuePair( "yyyyyyyyyyyyyyyyyyyy", "HelloWorld7", 11 );
  std::shared_ptr<unsigned char> key_7 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_2 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_2 );
  std::cout << "保存結果(7) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";
  // <データ8>
  storeTargetPair = generateDBKeyValuePair( "qqqqqqqqqqqqqqqqqqqq", "HelloWorld8", 11 );
  std::shared_ptr<unsigned char> key_8 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_2 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_2 );
  std::cout << "保存結果(8) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";
  // <データ9>
  storeTargetPair = generateDBKeyValuePair( "uuuuuuuuuuuuuuuuuuuu", "HelloWorld9", 11 );
  std::shared_ptr<unsigned char> key_9 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 , registryWith_2 );
  dbClient->get( storeTargetPair.first , &rawResponse , registryWith_2 );
  std::cout << "保存結果(9) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11  ) << "\n";


  flag = dbClient->abort(registryWith_2);
  // flag = dbClient->commit(registryWith_1 );
  std::cout << "Abort Status :: " << flag << "\n";


  //commit後データ追加
  storeTargetPair = generateDBKeyValuePair( "vvvvvvvvvvvvvvvvvvvv", "HelloWorld10", 12 );
  std::shared_ptr<unsigned char> key_10 = storeTargetPair.first;
  flag = dbClient->add( storeTargetPair.first , storeTargetPair.second , 12 , registryWith_1 );
  std::cout << "Commit::before Add Status :: " << flag << "\n";
  flag = dbClient->get( storeTargetPair.first , &rawResponse , registryWith_1 );
  std::cout << "Commit::before Get Status :: " << flag << "\n";


  // セーフモードコミット後追加検証
  storeTargetPair = generateDBKeyValuePair( "zzzzzzzzzzzzzzzzzzzz", "HelloWorld30", 12 );
  std::shared_ptr<unsigned char> key_0 = storeTargetPair.first;
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 12 );
  dbClient->get( storeTargetPair.first , &rawResponse );
  std::cout << "保存結果(0) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 12  ) << "\n";

  std::cout << (1 == 1 ) << "\n";
  return 0;


  // 以降がデータ削除シーケンス
  flag = dbClient->remove( key_1 );
  std::cout << "Remove Status(1) :: " << flag << "\n";
  flag = dbClient->remove( key_8 );
  std::cout << "Remove Status(8) :: " << flag << "\n";
  flag = dbClient->remove( key_10 );
  std::cout << "Remove Status(10) :: " << flag << "\n";
  flag = dbClient->remove( key_7 );
  std::cout << "Remove Status(7) :: " << flag << "\n";
  flag = dbClient->remove( key_4 );
  std::cout << "Remove Status(4) :: " << flag << "\n";
  flag = dbClient->remove( key_5 );
  std::cout << "Remove Status(5) :: " << flag << "\n";
  flag = dbClient->remove( key_6 );
  std::cout << "Remove Status(6) :: " << flag << "\n";
  flag = dbClient->remove( key_9 );
  std::cout << "Remove Status(9) :: " << flag << "\n";
  flag = dbClient->remove( key_3 );
  std::cout << "Remove Status(3) :: " << flag << "\n";
  flag = dbClient->remove( key_2 );
  std::cout << "Remove Status(2) :: " << flag << "\n";



  // 要素が正常に削除されている
  flag = dbClient->get( key_1 , &rawResponse );
  std::cout << "Get Statuts :: " << flag << "\n";


  // 削除後要素追加テスト
  storeTargetPair = generateDBKeyValuePair("cccccccccccccccccccc", "HelloWorld0", 11 );
  dbClient->add( storeTargetPair.first , storeTargetPair.second , 11 );
  dbClient->get( storeTargetPair.first , &rawResponse );
  std::cout << "保存結果(0) : " << verifyDataConsistency( storeTargetPair.second  , rawResponse , 11 ) << "\n";


}
