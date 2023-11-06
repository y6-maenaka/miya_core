#include <unistd.h>
#include <random>



namespace miya_db
{


constexpr int MIYA_DB_QUERY_ADD = 1;
constexpr int MIYA_DB_QUERY_GET = 2;
constexpr int MIYA_DB_QUERY_EXIST = 3;
constexpr int MIYA_DB_QUERY_REMOVE = 4;

constexpr int MIYA_DB_KEY_LENGTH = 20;

constexpr int MIYA_DB_STATUS_OK = 0;
constexpr int MIYA_DB_STATUS_BAD = -1;


static uint32_t generateQueryID()
{
	std::random_device rd; // シードの初期化
	std::mt19937 gen(rd());  // 乱数生成機の初期化

	std::uniform_int_distribution<uint32_t> dis; // 生成範囲の設定
	return dis(gen);

}



};
