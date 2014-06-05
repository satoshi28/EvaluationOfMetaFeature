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
	System::String^ strCommand ="SELECT tb_���O.name, tb_���.info, tb_���P�[�V�������.latitude, tb_���P�[�V�������.longitude, tb_������.[1], tb_������.[2], tb_������.[3], tb_������.[4], tb_������.[5], tb_������.[6], tb_������.[7], tb_������.[8], tb_������.[9], tb_������.[10], tb_������.[11], tb_������.[12], tb_������.[13], tb_������.[14], tb_������.[15], tb_������.[16], tb_������.[17], tb_������.[18], tb_������.[19], tb_������.[20], tb_������.[21], tb_������.[22], tb_������.[23], tb_������.[24], tb_������.[25], tb_������.[26], tb_������.[27], tb_������.[28], tb_������.[29], tb_������.[30], tb_������.[31], tb_������.[32], tb_������.[33], tb_������.[34], tb_������.[35], tb_������.[36], tb_������.[37], tb_������.[38], tb_������.[39], tb_������.[40], tb_������.[41], tb_������.[42], tb_������.[43], tb_������.[44], tb_������.[45], tb_������.[46], tb_������.[47], tb_������.[48], tb_������.[49], tb_������.[50], tb_������.[51], tb_������.[52], tb_������.[53], tb_������.[54], tb_������.[55], tb_������.[56], tb_������.[57], tb_������.[58], tb_������.[59], tb_������.[60], tb_������.[61], tb_������.[62], tb_������.[63], tb_������.[64] FROM (tb_���O INNER JOIN (tb_���P�[�V������� INNER JOIN tb_������ ON tb_���P�[�V�������.[ID] = tb_������.[ID]) ON tb_���O.[name_no] = tb_���P�[�V�������.[name_no]) INNER JOIN tb_��� ON tb_���P�[�V�������.[ID] = tb_���.[ID]";

	//�擾�p�f�[�^�e�[�u��
	System::Data::DataTable^ table = gcnew System::Data::DataTable("data");

	OleDbConnection^ conn = gcnew OleDbConnection(strConn);
	conn->Open();

	//�g�����U�N�V�����̊J�n
	OleDbTransaction^ transaction = conn->BeginTransaction(System::Data::IsolationLevel::ReadCommitted);
	try{
		//���P�[�V�������̍X�V�p
		OleDbDataAdapter^ adapter = gcnew OleDbDataAdapter(strCommand, strConn);
		OleDbCommandBuilder^ builder = gcnew OleDbCommandBuilder(adapter);

		adapter->Fill(table);
		
		//�g�����U�N�V�������R�~�b�g���܂��B
        transaction->Commit();

		System::String^ quote = "";
		System::String^ separator = ",";
		System::String^ replace = "";

		System::String^ filename = "sample.csv";
		SaveToCSV(table, filename, true,separator ,quote, replace);


		return 0;
	}
	catch(System::Exception^){
		//�g�����U�N�V�����̃��[���o�b�N
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
	//�擾�p�f�[�^�e�[�u��
	System::Data::DataTable^ table = gcnew System::Data::DataTable("data");
	//�X�V�p�f�[�^�e�[�u��
	System::Data::DataTable^ dataChanges = gcnew System::Data::DataTable("dataChanges");

	adapter->Fill(table);

	//�f�[�^�̒ǉ�
	System::Data::DataRow^ datarow;
	for(int i = 0; i < patterns.size() ; i++)
	{
		int ad =  patterns.size();
		//�V�����s�̍쐬
		datarow = table->NewRow();
	
		//�V�����ǉ�����s�ɑ΂���,�񖼂��w�肵�ăf�[�^��ǉ�����
		datarow["latitude"]= patterns[i].gps.latitude;
		datarow["longitude"]= patterns[i].gps.longitude;

		//�s�̒ǉ�
		table->Rows->Add(datarow);
	}
	dataChanges = table->GetChanges();

	//DB�̍X�V
	//�I�[�g�i���o�[�擾�p��update�C�x���g��L����
	adapter->RowUpdated += gcnew OleDbRowUpdatedEventHandler(OnRowUpdated);
	adapter->Update(dataChanges);

	//�I�[�g�i���o�[��Patterns�ɕۑ�
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



	//�V�����s�̌^���쐬
	System::Data::DataRow^ descRow;
	for(int i = 0; i < patterns.size() ; i++)
	{
		for(int j = 0; j < patterns[i].descriptors.rows ; j++)
		{
			//�V�����s�̍쐬
			descRow = table->NewRow();

			for(int k = 0; k < patterns[i].descriptors.cols ; k++)//64
			{
				descRow["ID"] = patterns[i].numberOfDB;
				descRow[(k+1).ToString()] = patterns[i].descriptors.at<unsigned char>(j,k);

			}
			//�s�̒ǉ�
			table->Rows->Add(descRow);
		}
	}
	//DB�X�V
	adapter->Update(table);
}

void ConnectingDB::SaveToCSV(System::Data::DataTable^ dt, System::String^ fileName, bool hasHeader, System::String^ separator, System::String^ quote, System::String^ replace)
{

  int rows = dt->Rows->Count;
  
  int cols = dt->Columns->Count;

  System::String^ text;

  //�ۑ��p�̃t�@�C�����J���B�㏑�����[�h�ŁB

  System::IO::StreamWriter^ writer = gcnew System::IO::StreamWriter(fileName, false, System::Text::Encoding::GetEncoding("shift_jis"));
  
  //�J��������ۑ����邩

  if (hasHeader)
  {
      //�J��������ۑ�����ꍇ
      for (int i = 0; i < cols; i++)
      {
          //�J���������擾
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

  //�f�[�^�̕ۑ�����
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
  //�X�g���[�������
  writer->Close();
}
