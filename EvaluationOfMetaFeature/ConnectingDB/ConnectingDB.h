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
	
	/* Pattern�����̏����f�[�^�x�[�X�ɒǉ����� */
	bool loadDB(std::vector<Pattern>& patterns, std::string fileName);

private:
	/* DB��tb_���P�[�V���������X�V */
	void createQueryString(System::String^& str, double latitude, double longitude);

	/* DB��tb_�����ʂ��X�V */
	void loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns);
};



#endif