#include "stream_buffer.h"



void StreamBuffer::pushOne( std::unique_ptr<SBSegment> target )
{

	// ロックの獲得
	std::unique_lock<std::mutex> lock(_mtx);  // 他が使用中だとブロッキングする. 他プロセスが解放するとそこからスタート

	_sb.push_back( std::move(target) ); // 要素の追加
}



std::unique_ptr<SBSegment> StreamBuffer::popOne()
{
	std::unique_lock<std::mutex> lock(_mtx);

	if( _sb.size() <= 0 ) return nullptr;

	std::unique_ptr<SBSegment> ret = std::move( _sb.front() ); // 要素の参照
	_sb.erase( _sb.begin() ); // 要素の削除

	return std::move( ret );
}
