#include "query_parser.h"





UnifiedStrageManager *QueryParser::suggestTable()
{

	
	switch( command ) // すでにメモリ上にテーブルが展開されていたら,それを返す
	{
		case 1:		
			UnifiedStrageManager *MMyISAMStrageManager = new MMyISAM; 
			return MMyISAMStrageManager;

			break;

		default:
			break;

	}

	return nullptr;
}

