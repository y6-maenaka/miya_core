#ifndef B9953423_DA9F_432C_853A_858FBAFBC4BE
#define B9953423_DA9F_432C_853A_858FBAFBC4BE



#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>






namespace miya_db{


class ValueStoreManager;
class IndexManager;
class OverlayMemoryManager;
class ONode;
class SafeONode;











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
	MMyISAMSafeModeManager( std::shared_ptr<ValueStoreManager> normalValueStoreManager, std::shared_ptr<IndexManager> normalIndexManager );

};






};


#endif //  B9953423_DA9F_432C_853A_858FBAFBC4BE



