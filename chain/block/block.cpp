#include "block.hpp"
#include "block.h"


namespace chain
{


Block::Block()
{
	_header = std::make_shared<BlockHeader>();
	_coinbase = nullptr;
}

void Block::header( std::shared_ptr<BlockHeader> target )
{
	// _header = *target;
	_header = target;
}

std::shared_ptr<tx::coinbase> Block::coinbase()
{
	return _coinbase;
}


std::vector< std::shared_ptr<tx::P2PKH> > Block::txVector()
{
	return _txVector;
}

void Block::coinbase( std::shared_ptr<tx::coinbase> coinbase )
{
  _coinbase = coinbase;
}


void Block::add( std::shared_ptr<tx::P2PKH> p2pkh )
{
	_txVector.push_back( p2pkh );
}

unsigned int Block::calcMerkleRoot( std::shared_ptr<unsigned char> *ret )
{

	std::vector< std::shared_ptr<unsigned char> > txIDVector;
	std::shared_ptr<unsigned char> txID; unsigned int txIDLength;

	if( _coinbase == nullptr ){
		std::cout << "コインベーストランザクションがセットされていません" << "\n";
		return 0;
	}
	txIDLength = _coinbase->calcTxID( &txID ); // コインベーストランザクションIDの算出と追加
	txIDVector.push_back( txID );


	for( auto itr : _txVector ) // 通常トランザクションのトランザクションIDの算出と追加
	{
		txIDLength = itr->calcTxID( &txID );
		txIDVector.push_back( txID );
	}


	if( txIDVector.size() % 2 != 0 ){ // ID要素が奇数個の場合は最後尾の要素を複製する
		txIDVector.push_back( txIDVector.back() );
	}



	std::function<std::vector<std::shared_ptr<unsigned char>>(std::vector<std::shared_ptr<unsigned char>>)> calcRoot = [&]( std::vector<std::shared_ptr<unsigned char>> txIDVector ){
		if( txIDVector.size() <= 1 ) return txIDVector;

		std::vector< std::shared_ptr<unsigned char>> altTxVector; // 代替ベクター
		std::shared_ptr<unsigned char> joinedTxID = std::shared_ptr<unsigned char>( new unsigned char[32*2] );
		std::shared_ptr<unsigned char> hashedJoinedTxID; unsigned int hashedJoinedTxIDLength;

		for( int i=0; i<txIDVector.size() ; i+=2 )
		{
			memcpy( joinedTxID.get(), txIDVector.at(i).get() , 32 );
			memcpy( joinedTxID.get() + 32 , txIDVector.at(i).get() , 32 );
			hashedJoinedTxIDLength = hash::SHAHash( joinedTxID, 32 * 2 , &hashedJoinedTxID , "sha256" );

			altTxVector.push_back( hashedJoinedTxID );
		}

		return calcRoot( altTxVector );
	};


	std::vector< std::shared_ptr<unsigned char> > retVector;
	retVector = calcRoot( txIDVector );

	if( retVector.empty() || retVector.size() != 1 ) return 0;

	*ret = std::shared_ptr<unsigned char>( new unsigned char[32] );
	*ret = retVector.front();

	return 32;
}

void Block::merkleRoot( std::shared_ptr<unsigned char> target )
{
	return _header->merkleRoot( target );
}

unsigned int Block::exportHeader( std::shared_ptr<unsigned char> *retRaw )
{
	return _header->exportRaw( retRaw );
}


size_t Block::blockHash( std::shared_ptr<unsigned char> *ret )
{
	return _header->headerHash( ret );
}

Block::block_hash Block::get_block_hash() const
{

}

std::shared_ptr<unsigned char> Block::blockHash()
{
	std::shared_ptr<unsigned char> ret;
	_header->headerHash( &ret );
	return ret;
}


uint32_t Block::time()
{
	return _header->time();
}

uint32_t Block::height()
{
  _coinbase->height();
}


bool Block::verify( std::shared_ptr<LightUTXOSet> utxoSet )
{
	bool flag;
	flag = _header->verify();
	if( !flag ) return false;

	// マークルルートを計算して,ヘッダのマークルルートと一致しているか確認
	std::shared_ptr<unsigned char> merkle;
	calcMerkleRoot( &merkle );
	flag = _header->cmpMerkleRoot( merkle );
	if( !flag ) return false;

	// 正常に残高を参照しているか否かの検証(内部でscriptが実行される)
	for( auto itr : _txVector ){
		flag = itr->verify( utxoSet );
		if( !flag ) return false;
	}

	return true;

	/* 検証項目 */
	// 1. ブロックヘッダの検証
	// 2. マイニング難易度の検証
	// 3. トランザクションの検証
	// 4. ダブルスペンディングの検証
	// 5. コインベーストランザクションの宝珠確認
	// 6. マージン(prev_out)との関係
	// 7. スクリプトの実行
}

bool Block::compare( std::shared_ptr<unsigned char> targetBlockHash )
{
	return memcmp( this->blockHash().get(), targetBlockHash.get(), 32 ) == 0;
}

#if L1_DEBUG
void Block::__printBlockHash()
{
	std::shared_ptr<unsigned char> blockHash; size_t blockHashLength;
	blockHashLength = this->blockHash( &blockHash );
	std::cout << "Block Hash :: ";
	std::cout << "\x1b[33m";
	for( int i=0; i<blockHashLength; i++ ){
		printf("%02X", blockHash.get()[i]);
	} std::cout << "\n";
	std::cout << "\x1b[39m";
}
#endif

#if L1_DEBUG
void Block::__printPrevBlockHash()
{
	std::shared_ptr<unsigned char> prevHash; size_t prevHashLength;
	prevHash = _header->prevBlockHash();

	for( int i=0; i<32; i++ )
	{
		printf("%02X", prevHash.get()[i] );
	} std::cout << "\n";
}
#endif









block::block_hash block::get_block_hash() const
{
  return _header.get_header_hash();
}

block::block_hash block::get_prev_block_hash() const
{
  return _header.get_prev_block_hash();
}

std::vector<std::uint8_t> block::export_to_binary()
{
  std::vector<std::uint8_t> block_header_binary = _header.export_to_binary();
  std::vector<std::uint8_t> coinbase_binary ;//= _coinbase.export_to_binary();
  std::vector<std::uint8_t> tx_v_binary;
  for( auto &itr : _tx_v ){
	std::vector< std::uint8_t > tx_binary = itr.export_to_binary();
	tx_v_binary.insert( tx_v_binary.end() ,tx_binary.begin(), tx_binary.end() );
  }

  std::vector< std::uint8_t > ret;
  ret.insert( ret.end(), block_header_binary.begin(), block_header_binary.end() );
  ret.insert( ret.end(), coinbase_binary.begin(), coinbase_binary.end()) ; 
  ret.insert( ret.end(), tx_v_binary.begin(), tx_v_binary.end() );
  return ret;
}

block::id block::get_id() const
{
  return get_block_hash();
}

};
