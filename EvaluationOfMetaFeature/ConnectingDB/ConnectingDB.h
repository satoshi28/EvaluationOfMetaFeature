#ifndef CONNECTING_DB
#define CONNECTING_DB

////////////////////////////////////////////////////////////////////
using namespace System::Diagnostics;
using namespace System::Data::OleDb;

#include "../Pattern.h"

class ConnectingDB
{
public:
	ConnectingDB();
	~ConnectingDB();
	
	/* Pattern内部の情報をデータベースに追加する */
	bool loadDB(std::vector<Pattern>& patterns, std::string fileName);

private:
	/* DBのtb_ロケーション情報を更新 */
	void createQueryString(System::String^& str, double latitude, double longitude);

	/* DBのtb_特徴量を更新 */
	void loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns);
};



#endif