#ifndef DC8B8BFF_FE59_4105_A0C9_5984D4E0FC54
#define DC8B8BFF_FE59_4105_A0C9_5984D4E0FC54


#include <memory>


namespace chain
{


template <typename P> concept IsPair = requires(P p){
	{ p.first };
	{ p.second };
	requires std::is_pointer_v<decltype(p.first)>;
	requires std::is_same_v<decltype(p.second), std::size_t>;
};


struct MiyaCoreMSG_Utils
{
public:
  template < typename T, typename... Args > requires(IsPair<Args> && ...) std::vector<T> format( Args... args ) const;
};


template < typename T, typename... Args > 
requires (IsPair<Args> && ...)
std::vector<T> MiyaCoreMSG_Utils::format( Args... args ) const
{
  auto export_length = (0 + ... + args.second);
  std::vector<T> ret; ret.reserve( export_length );
  
  auto itr = ret.begin();
  ([&](const auto& arg){
	itr = std::copy_n( arg.first, arg.second, itr );
  } (args), ... );

  return ret;
}


}


#endif // 


