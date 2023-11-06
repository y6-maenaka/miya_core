#ifndef B9953423_DA9F_432C_853A_858FBAFBC4BE
#define B9953423_DA9F_432C_853A_858FBAFBC4BE



#include <iostream>
#include <memory>
#include <string>
#include <vector>






namespace miya_db{


class ValueStoreManager;
class IndexManager;
class ONode;



using ONodeConversionEntry = std::pair< std::shared_ptr<ONode>, std::shared_ptr<unsigned char> >;
/*
 通常モードで使用するONodeとセーフモードで使用するSafeONodeとの変換テーブル
 ONodeはoptrで管理されており,SafeONodeは通常のポインタで管理する　その対応テーブル
 */
struct ONodeConversionTable // optr to poitner / pointer to optr
{

private:
};







class MMyISAMSafeModeManager
{
	


private:

	struct 
	{
		std::shared_ptr<ValueStoreManager> _valueStoreManager;
		std::shared_ptr<IndexManager> _indexManager;

	} _normalModules;

	
	struct 
	{
		std::shared_ptr<ValueStoreManager> _valueStoreManager;
		std::shared_ptr<IndexManager> _indexManager;

	} _safeModules;



public:
	MMyISAMSafeModeManager( std::shared_ptr<ValueStoreManager> valueStoreManager, std::shared_ptr<IndexManager> indexManager );

};






};


#endif //  B9953423_DA9F_432C_853A_858FBAFBC4BE



