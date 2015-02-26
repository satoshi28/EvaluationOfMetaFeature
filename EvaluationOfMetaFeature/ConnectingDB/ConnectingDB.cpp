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
	System::String^ strCommandDesc ="SELECT tb_特徴量.[ID], tb_特徴量.[1], tb_特徴量.[2], tb_特徴量.[3], tb_特徴量.[4], tb_特徴量.[5], tb_特徴量.[6], tb_特徴量.[7], tb_特徴量.[8], tb_特徴量.[9], tb_特徴量.[10], tb_特徴量.[11], tb_特徴量.[12], tb_特徴量.[13], tb_特徴量.[14], tb_特徴量.[15], tb_特徴量.[16], tb_特徴量.[17], tb_特徴量.[18], tb_特徴量.[19], tb_特徴量.[20], tb_特徴量.[21], tb_特徴量.[22], tb_特徴量.[23], tb_特徴量.[24], tb_特徴量.[25], tb_特徴量.[26], tb_特徴量.[27], tb_特徴量.[28], tb_特徴量.[29], tb_特徴量.[30], tb_特徴量.[31], tb_特徴量.[32], tb_特徴量.[33], tb_特徴量.[34], tb_特徴量.[35], tb_特徴量.[36], tb_特徴量.[37], tb_特徴量.[38], tb_特徴量.[39], tb_特徴量.[40], tb_特徴量.[41], tb_特徴量.[42], tb_特徴量.[43], tb_特徴量.[44], tb_特徴量.[45], tb_特徴量.[46], tb_特徴量.[47], tb_特徴量.[48], tb_特徴量.[49], tb_特徴量.[50], tb_特徴量.[51], tb_特徴量.[52], tb_特徴量.[53], tb_特徴量.[54], tb_特徴量.[55], tb_特徴量.[56], tb_特徴量.[57], tb_特徴量.[58], tb_特徴量.[59], tb_特徴量.[60], tb_特徴量.[61], tb_特徴量.[62], tb_特徴量.[63], tb_特徴量.[64] FROM tb_特徴量 ORDER BY tb_特徴量.ID";
	System::String^ strName = gcnew System::String(databaseName.c_str());
	strConn = System::String::Concat( strConn, strName );
	System::Data::DataTable^ descTable = gcnew System::Data::DataTable("data");	//取得用データテーブル

	OleDbConnection^ conn = gcnew OleDbConnection(strConn);
	conn->Open();

	//トランザクションの開始
	OleDbTransaction^ transaction = conn->BeginTransaction(System::Data::IsolationLevel::ReadCommitted);
	try{
		//ロケーション情報の更新用
		OleDbDataAdapter^ descAdapter = gcnew OleDbDataAdapter(strCommandDesc, strConn);
		OleDbCommandBuilder^ descBuilder = gcnew OleDbCommandBuilder(descAdapter);
		descAdapter->Fill(descTable);

		//トランザクションをコミットします。
        transaction->Commit();
		loadFeatureIntoPattern(descTable, patterns);

		return true;
	}
	catch(System::Exception^){
		//トランザクションのロールバック
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

	//テーブルの行数まで繰り返す
	for(int i = 0; i < table->Rows->Count; i++)
	{
		datarow = table->Rows[i];

		//IDが異なっていれば初期化する
		if(id != System::Convert::ToInt32( datarow["ID"]->ToString()) )
		{
			
			Pattern pattern;
			pattern.descriptors = descriptors.clone();
			patterns.push_back(pattern);
			//初期化
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
