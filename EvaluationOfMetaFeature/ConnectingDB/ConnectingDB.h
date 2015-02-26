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
	* @brief ������DB����pattern���\�z����
	* @param[in] databaseName �f�[�^�x�[�X�̖��O
	* @param[out] patterns
	*/
	bool loadDB(std::string databaseName, std::vector<Pattern>& patterns);

private:
	/**
	* @brief �����ʃe�[�u������pattern���\�z����
	* @param[in] �f�[�^�x�[�X�̖��O
	* @param[out] patterns
	*/
	void loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns);
};



#endif