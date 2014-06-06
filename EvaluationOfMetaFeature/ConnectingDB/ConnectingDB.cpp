#include "ConnectingDB.h"


ConnectingDB::ConnectingDB()
{
}

ConnectingDB::~ConnectingDB()
{
}


bool ConnectingDB::loadDB(double latitude, double longitude)
{
	//conect
	System::String^ strConn = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\\Users\\satoshi\\Documents\\Visual Studio 2012\\DB\\imageDatabase.accdb";
	System::String^ strCommand ="SELECT tb_ロケーション情報.ID, tb_名前.name, tb_情報.info, tb_ロケーション情報.latitude, tb_ロケーション情報.longitude, tb_特徴量.[1], tb_特徴量.[2], tb_特徴量.[3], tb_特徴量.[4], tb_特徴量.[5], tb_特徴量.[6], tb_特徴量.[7], tb_特徴量.[8], tb_特徴量.[9], tb_特徴量.[10], tb_特徴量.[11], tb_特徴量.[12], tb_特徴量.[13], tb_特徴量.[14], tb_特徴量.[15], tb_特徴量.[16], tb_特徴量.[17], tb_特徴量.[18], tb_特徴量.[19], tb_特徴量.[20], tb_特徴量.[21], tb_特徴量.[22], tb_特徴量.[23], tb_特徴量.[24], tb_特徴量.[25], tb_特徴量.[26], tb_特徴量.[27], tb_特徴量.[28], tb_特徴量.[29], tb_特徴量.[30], tb_特徴量.[31], tb_特徴量.[32], tb_特徴量.[33], tb_特徴量.[34], tb_特徴量.[35], tb_特徴量.[36], tb_特徴量.[37], tb_特徴量.[38], tb_特徴量.[39], tb_特徴量.[40], tb_特徴量.[41], tb_特徴量.[42], tb_特徴量.[43], tb_特徴量.[44], tb_特徴量.[45], tb_特徴量.[46], tb_特徴量.[47], tb_特徴量.[48], tb_特徴量.[49], tb_特徴量.[50], tb_特徴量.[51], tb_特徴量.[52], tb_特徴量.[53], tb_特徴量.[54], tb_特徴量.[55], tb_特徴量.[56], tb_特徴量.[57], tb_特徴量.[58], tb_特徴量.[59], tb_特徴量.[60], tb_特徴量.[61], tb_特徴量.[62], tb_特徴量.[63], tb_特徴量.[64] FROM (tb_名前 INNER JOIN (tb_ロケーション情報 INNER JOIN tb_特徴量 ON tb_ロケーション情報.[ID] = tb_特徴量.[ID]) ON tb_名前.[name_no] = tb_ロケーション情報.[name_no]) INNER JOIN tb_情報 ON tb_ロケーション情報.[ID] = tb_情報.[ID]";
	//位置情報に基づいて接続文字列の作成
	createQueryString(strCommand , latitude, longitude);

	//取得用データテーブル
	System::Data::DataTable^ table = gcnew System::Data::DataTable("data");

	OleDbConnection^ conn = gcnew OleDbConnection(strConn);
	conn->Open();

	//トランザクションの開始
	OleDbTransaction^ transaction = conn->BeginTransaction(System::Data::IsolationLevel::ReadCommitted);
	try{
		//ロケーション情報の更新用
		OleDbDataAdapter^ adapter = gcnew OleDbDataAdapter(strCommand, strConn);
		OleDbCommandBuilder^ builder = gcnew OleDbCommandBuilder(adapter);

		adapter->Fill(table);
		
		//トランザクションをコミットします。
        transaction->Commit();

		System::String^ quote = "";
		System::String^ separator = ",";
		System::String^ replace = "";

		System::String^ filename = "sample.csv";
		SaveToCSV(table, filename, true,separator ,quote, replace);

		std::vector<Pattern> patterns;

		loadFeatureIntoPattern(table, patterns);

		for(int i = 0 ; i < patterns.size() ; i++)
		{
			std::cout << patterns[i].descriptors << std::endl;

		}

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

void ConnectingDB::createQueryString(System::String^& str, double latitude, double longitude)
{	
	double minLatitude = latitude - 1;
	double maxLatitude = latitude + 1;
	double minLongitude = longitude - 1;
	double maxLongitude = longitude + 1;

	System::String^ tmpScopeLatitude = "WHERE ( ((tb_ロケーション情報.latitude) Between min And max) AND ";
	System::String^ tmpScopeLongitude = "((tb_ロケーション情報.longitude) Between min And max))";
	System::String^ strScopeLatitude;
	System::String^ strScopeLongitude;

	//位置情報検索文字列の作成
	System::String^ min = System::Convert::ToString(minLatitude);
	System::String^ max = System::Convert::ToString( maxLatitude);

	strScopeLatitude = tmpScopeLatitude->Replace("min",min);
	strScopeLatitude = strScopeLatitude->Replace("max",max);

	min = System::Convert::ToString( minLongitude);
	max = System::Convert::ToString( maxLongitude);
	strScopeLongitude = tmpScopeLongitude->Replace("min",min);
	strScopeLongitude = strScopeLongitude->Replace("max",max);

	//文字列の連結
	str += strScopeLatitude;
	str += strScopeLongitude;
}

void ConnectingDB::loadFeatureIntoPattern(System::Data::DataTable^ table, std::vector<Pattern>& patterns)
{
	cv::Mat descriptors = cv::Mat::zeros(1, 64,  CV_8U);
	int num = 0,id = 0;
	System::Data::DataRow^ datarow;
	System::String^ descNum;


	datarow = table->Rows[0];

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
}

void ConnectingDB::SaveToCSV(System::Data::DataTable^ dt, System::String^ fileName, bool hasHeader, System::String^ separator, System::String^ quote, System::String^ replace)
{

  int rows = dt->Rows->Count;
  
  int cols = dt->Columns->Count;

  System::String^ text;

  //保存用のファイルを開く。上書きモードで。

  System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter(fileName, false, System::Text::Encoding::GetEncoding("shift_jis"));
  
  //カラム名を保存するか

  if (hasHeader)
  {
      //カラム名を保存する場合
      for (int i = 0; i < cols; i++)
      {
          //カラム名を取得
          if (quote != "")
          {	  
              text = dt->Columns[i]->ColumnName->Replace(quote, replace);
          }
          else
          {
              text = dt->Columns[i]->ColumnName;
          }

          if (i != cols - 1)
          {
              writer->Write(quote + text + quote + separator);
          }
          else
          {
              writer->WriteLine(quote + text + quote);
          }
      }
  }

  //データの保存処理
  for (int i = 0; i < rows; i++)
  {
      for (int j = 0; j < cols; j++)
      {
          if (quote != "")
          {
              text = dt->Rows[i][j]->ToString()->Replace(quote, replace);
          }
          else
          {
              text = dt->Rows[i][j]->ToString();
          }

          if (j != cols - 1)
          {
              writer->Write(quote + text + quote + separator);
          }
          else
          {
              writer->WriteLine(quote + text + quote);
          }
      }
  }
  //ストリームを閉じる
  writer->Close();
}
