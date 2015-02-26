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
	
	/**
	* @brief 特徴量DBからpatternを構築する
	* @param[in] databaseName データベースの名前
	* @param[out] patterns
	*/
	bool loadDB(std::string databaseName, std::vector<Pattern>& patterns);

private:
	/**
	* @brief 特徴量テーブルからpatternを構築する
	* @param[in] データベースの名前
	* @param[out] patterns
	*/
	void loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns);
};



#endif