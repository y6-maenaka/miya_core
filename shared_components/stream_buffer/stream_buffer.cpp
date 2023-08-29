#include "stream_buffer.h"



StreamBuffer::StreamBuffer()
{
	_sb._capacity = DEFAULT_STREAM_BUFFER_CAPACITY;
}






void StreamBuffer::enqueue( std::unique_ptr<SBSegment> target )
{

	// ロックの獲得
	std::unique_lock<std::mutex> lock(_mtx);  // 他が使用中だとブロッキングする. 他プロセスが解放するとそこからスタート


	_pushContributer._pushCV.wait( lock , [&]{
			return (_sb._queue.size() < _sb._capacity);
		});
		// 評価される度に第二引数がtrueであることを確認する
		// 第２引数がtrueだったらwaitを抜ける

	_sb._queue.push_back( std::move(target) );

	_popContributer._popCV.notify_all();

	return;

}



std::unique_ptr<SBSegment> StreamBuffer::dequeue()
{
	std::unique_lock<std::mutex> lock(_mtx);

	
	_popContributer._popCV.wait( lock , [&]{
		return !(_sb._queue.empty()); // emptyでなければ待機状態を解除する
	});
	
	
	std::unique_ptr<SBSegment> ret = std::move( _sb._queue.front() );
	_sb._queue.erase( _sb._queue.cbegin() );

	_pushContributer._pushCV.notify_all();
	
	return std::move( ret );
}




/*
	- 入出力がネックになったらSBを増やす
	- 入出力が詰まってきたらコンシューマーを増やす
*/
