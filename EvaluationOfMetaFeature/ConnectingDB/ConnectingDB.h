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
	bool loadDB(double latitude, double longitude, std::vector<Pattern>& patterns);

		/* DataTableをcsvファイルに保存する */
	void SaveToCSV(System::Data::DataTable^ dt, System::String^ fileName, bool hasHeader, 
		System::String^ separator, System::String^ quote, System::String^ replace);

private:
	/* DBのtb_ロケーション情報を更新 */
	void createQueryString(System::String^& str, double latitude, double longitude);

	/* DBのtb_特徴量を更新 */
	void loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns);

};



#endif