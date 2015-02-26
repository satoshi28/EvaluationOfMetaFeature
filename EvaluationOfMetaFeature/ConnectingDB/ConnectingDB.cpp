#include "ConnectingDB.h"


ConnectingDB::ConnectingDB()
{
}

ConnectingDB::~ConnectingDB()
{
}


bool ConnectingDB::loadDB(std::string databaseName, std::vector<Pattern>& patterns)
{
	System::String^ strConn = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\\Users\\satoshi\\Documents\\Visual Studio 2012\\study_db\\";
	System::String^ strCommandDesc ="SELECT tb_������.[ID], tb_������.[1], tb_������.[2], tb_������.[3], tb_������.[4], tb_������.[5], tb_������.[6], tb_������.[7], tb_������.[8], tb_������.[9], tb_������.[10], tb_������.[11], tb_������.[12], tb_������.[13], tb_������.[14], tb_������.[15], tb_������.[16], tb_������.[17], tb_������.[18], tb_������.[19], tb_������.[20], tb_������.[21], tb_������.[22], tb_������.[23], tb_������.[24], tb_������.[25], tb_������.[26], tb_������.[27], tb_������.[28], tb_������.[29], tb_������.[30], tb_������.[31], tb_������.[32], tb_������.[33], tb_������.[34], tb_������.[35], tb_������.[36], tb_������.[37], tb_������.[38], tb_������.[39], tb_������.[40], tb_������.[41], tb_������.[42], tb_������.[43], tb_������.[44], tb_������.[45], tb_������.[46], tb_������.[47], tb_������.[48], tb_������.[49], tb_������.[50], tb_������.[51], tb_������.[52], tb_������.[53], tb_������.[54], tb_������.[55], tb_������.[56], tb_������.[57], tb_������.[58], tb_������.[59], tb_������.[60], tb_������.[61], tb_������.[62], tb_������.[63], tb_������.[64] FROM tb_������ ORDER BY tb_������.ID";
	System::String^ strName = gcnew System::String(databaseName.c_str());
	strConn = System::String::Concat( strConn, strName );
	System::Data::DataTable^ descTable = gcnew System::Data::DataTable("data");	//�擾�p�f�[�^�e�[�u��

	OleDbConnection^ conn = gcnew OleDbConnection(strConn);
	conn->Open();

	//�g�����U�N�V�����̊J�n
	OleDbTransaction^ transaction = conn->BeginTransaction(System::Data::IsolationLevel::ReadCommitted);
	try{
		//���P�[�V�������̍X�V�p
		OleDbDataAdapter^ descAdapter = gcnew OleDbDataAdapter(strCommandDesc, strConn);
		OleDbCommandBuilder^ descBuilder = gcnew OleDbCommandBuilder(descAdapter);
		descAdapter->Fill(descTable);

		//�g�����U�N�V�������R�~�b�g���܂��B
        transaction->Commit();
		loadFeatureIntoPattern(descTable, patterns);

		return true;
	}
	catch(System::Exception^){
		//�g�����U�N�V�����̃��[���o�b�N
		transaction->Rollback();

		return false;
	}
	finally
	{
		conn->Close();
	}
}

void ConnectingDB::loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns)
{
	cv::Mat descriptors = cv::Mat::zeros(1, 64,  CV_8U);
	int num = 0,id = 0;
	System::Data::DataRow^ datarow;
	System::String^ descNum;


	datarow = table->Rows[0];

	std::cout << table->Rows->Count << std::endl;

	id= System::Convert::ToInt32( datarow["ID"]->ToString() );

	//�e�[�u���̍s���܂ŌJ��Ԃ�
	for(int i = 0; i < table->Rows->Count; i++)
	{
		datarow = table->Rows[i];

		//ID���قȂ��Ă���Ώ���������
		if(id != System::Convert::ToInt32( datarow["ID"]->ToString()) )
		{
			
			Pattern pattern;
			pattern.descriptors = descriptors.clone();
			patterns.push_back(pattern);
			//������
			num = 0;
			id= System::Convert::ToInt32( datarow["ID"]->ToString() );
			descriptors = cv::Mat::zeros(1, 64,  CV_8U);
		}

		num += 1;
		descriptors.resize(num, 0);
		for(int j = 0; j < 64; j++)
		{
			descNum =  System::Convert::ToString(j+1);
			descriptors.at<unsigned char>(num - 1, j) = System::Convert::ToByte( datarow[descNum]->ToString() );
		}

	}
	Pattern pattern;
	pattern.descriptors = descriptors.clone();
	patterns.push_back(pattern);

}
