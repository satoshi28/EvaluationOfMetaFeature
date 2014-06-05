#include "ConnectingDB.h"


ConnectingDB::ConnectingDB()
{
}

ConnectingDB::~ConnectingDB()
{
}


int ConnectingDB::loadDB()
{

	//conect
	System::String^ strConn = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=C:\\Users\\satoshi\\Documents\\Visual Studio 2012\\DB\\objectDatabase.accdb";
	System::String^ strCommand ="SELECT tb_名前.name, tb_情報.info, tb_ロケーション情報.latitude, tb_ロケーション情報.longitude, tb_特徴量.[1], tb_特徴量.[2], tb_特徴量.[3], tb_特徴量.[4], tb_特徴量.[5], tb_特徴量.[6], tb_特徴量.[7], tb_特徴量.[8], tb_特徴量.[9], tb_特徴量.[10], tb_特徴量.[11], tb_特徴量.[12], tb_特徴量.[13], tb_特徴量.[14], tb_特徴量.[15], tb_特徴量.[16], tb_特徴量.[17], tb_特徴量.[18], tb_特徴量.[19], tb_特徴量.[20], tb_特徴量.[21], tb_特徴量.[22], tb_特徴量.[23], tb_特徴量.[24], tb_特徴量.[25], tb_特徴量.[26], tb_特徴量.[27], tb_特徴量.[28], tb_特徴量.[29], tb_特徴量.[30], tb_特徴量.[31], tb_特徴量.[32], tb_特徴量.[33], tb_特徴量.[34], tb_特徴量.[35], tb_特徴量.[36], tb_特徴量.[37], tb_特徴量.[38], tb_特徴量.[39], tb_特徴量.[40], tb_特徴量.[41], tb_特徴量.[42], tb_特徴量.[43], tb_特徴量.[44], tb_特徴量.[45], tb_特徴量.[46], tb_特徴量.[47], tb_特徴量.[48], tb_特徴量.[49], tb_特徴量.[50], tb_特徴量.[51], tb_特徴量.[52], tb_特徴量.[53], tb_特徴量.[54], tb_特徴量.[55], tb_特徴量.[56], tb_特徴量.[57], tb_特徴量.[58], tb_特徴量.[59], tb_特徴量.[60], tb_特徴量.[61], tb_特徴量.[62], tb_特徴量.[63], tb_特徴量.[64] FROM (tb_名前 INNER JOIN (tb_ロケーション情報 INNER JOIN tb_特徴量 ON tb_ロケーション情報.[ID] = tb_特徴量.[ID]) ON tb_名前.[name_no] = tb_ロケーション情報.[name_no]) INNER JOIN tb_情報 ON tb_ロケーション情報.[ID] = tb_情報.[ID]";

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


		return 0;
	}
	catch(System::Exception^){
		//トランザクションのロールバック
		transaction->Rollback();

		return -1;
	}
	finally
	{
		conn->Close();
	}

/*
#ifdef _DEBUG
	System::String^ quote = "";
    System::String^ separator = ",";
    System::String^ replace = "";

	System::String^ filename = "sample.csv";
	SaveToCSV(table, filename, true,separator ,quote, replace);
#endif //_DEBUG
	
*/
}

void ConnectingDB::updateLocationTable(OleDbDataAdapter^ adapter, std::vector<Pattern>& patterns)
{
	//取得用データテーブル
	System::Data::DataTable^ table = gcnew System::Data::DataTable("data");
	//更新用データテーブル
	System::Data::DataTable^ dataChanges = gcnew System::Data::DataTable("dataChanges");

	adapter->Fill(table);

	//データの追加
	System::Data::DataRow^ datarow;
	for(int i = 0; i < patterns.size() ; i++)
	{
		int ad =  patterns.size();
		//新しい行の作成
		datarow = table->NewRow();
	
		//新しく追加する行に対して,列名を指定してデータを追加する
		datarow["latitude"]= patterns[i].gps.latitude;
		datarow["longitude"]= patterns[i].gps.longitude;

		//行の追加
		table->Rows->Add(datarow);
	}
	dataChanges = table->GetChanges();

	//DBの更新
	//オートナンバー取得用にupdateイベントを有効化
	adapter->RowUpdated += gcnew OleDbRowUpdatedEventHandler(OnRowUpdated);
	adapter->Update(dataChanges);

	//オートナンバーをPatternsに保存
	for(int i = 0; i < patterns.size() ; i++)
	{
		int num = System::Convert::ToInt32( dataChanges->Rows[i][0]->ToString() );
		patterns[i].numberOfDB= num;
	}
}

void ConnectingDB::updateDescTable(OleDbDataAdapter^ adapter, std::vector<Pattern> patterns)
{
	System::Data::DataTable^ table = gcnew System::Data::DataTable("descriptors");
	adapter->Fill(table);



	//新しい行の型を作成
	System::Data::DataRow^ descRow;
	for(int i = 0; i < patterns.size() ; i++)
	{
		for(int j = 0; j < patterns[i].descriptors.rows ; j++)
		{
			//新しい行の作成
			descRow = table->NewRow();

			for(int k = 0; k < patterns[i].descriptors.cols ; k++)//64
			{
				descRow["ID"] = patterns[i].numberOfDB;
				descRow[(k+1).ToString()] = patterns[i].descriptors.at<unsigned char>(j,k);

			}
			//行の追加
			table->Rows->Add(descRow);
		}
	}
	//DB更新
	adapter->Update(table);
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
