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
	System::String^ strCommand ="SELECT tb_���P�[�V�������.ID, tb_���O.name, tb_���.info, tb_���P�[�V�������.latitude, tb_���P�[�V�������.longitude, tb_������.[1], tb_������.[2], tb_������.[3], tb_������.[4], tb_������.[5], tb_������.[6], tb_������.[7], tb_������.[8], tb_������.[9], tb_������.[10], tb_������.[11], tb_������.[12], tb_������.[13], tb_������.[14], tb_������.[15], tb_������.[16], tb_������.[17], tb_������.[18], tb_������.[19], tb_������.[20], tb_������.[21], tb_������.[22], tb_������.[23], tb_������.[24], tb_������.[25], tb_������.[26], tb_������.[27], tb_������.[28], tb_������.[29], tb_������.[30], tb_������.[31], tb_������.[32], tb_������.[33], tb_������.[34], tb_������.[35], tb_������.[36], tb_������.[37], tb_������.[38], tb_������.[39], tb_������.[40], tb_������.[41], tb_������.[42], tb_������.[43], tb_������.[44], tb_������.[45], tb_������.[46], tb_������.[47], tb_������.[48], tb_������.[49], tb_������.[50], tb_������.[51], tb_������.[52], tb_������.[53], tb_������.[54], tb_������.[55], tb_������.[56], tb_������.[57], tb_������.[58], tb_������.[59], tb_������.[60], tb_������.[61], tb_������.[62], tb_������.[63], tb_������.[64] FROM (tb_���O INNER JOIN (tb_���P�[�V������� INNER JOIN tb_������ ON tb_���P�[�V�������.[ID] = tb_������.[ID]) ON tb_���O.[name_no] = tb_���P�[�V�������.[name_no]) INNER JOIN tb_��� ON tb_���P�[�V�������.[ID] = tb_���.[ID]";
	//�ʒu���Ɋ�Â��Đڑ�������̍쐬
	createQueryString(strCommand , latitude, longitude);

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

		std::vector<Pattern> patterns;

		loadFeatureIntoPattern(table, patterns);

		for(int i = 0 ; i < patterns.size() ; i++)
		{
			std::cout << patterns[i].descriptors << std::endl;

		}

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

void ConnectingDB::createQueryString(System::String^& str, double latitude, double longitude)
{	
	double minLatitude = latitude - 1;
	double maxLatitude = latitude + 1;
	double minLongitude = longitude - 1;
	double maxLongitude = longitude + 1;

	System::String^ tmpScopeLatitude = "WHERE ( ((tb_���P�[�V�������.latitude) Between min And max) AND ";
	System::String^ tmpScopeLongitude = "((tb_���P�[�V�������.longitude) Between min And max))";
	System::String^ strScopeLatitude;
	System::String^ strScopeLongitude;

	//�ʒu��񌟍�������̍쐬
	System::String^ min = System::Convert::ToString(minLatitude);
	System::String^ max = System::Convert::ToString( maxLatitude);

	strScopeLatitude = tmpScopeLatitude->Replace("min",min);
	strScopeLatitude = strScopeLatitude->Replace("max",max);

	min = System::Convert::ToString( minLongitude);
	max = System::Convert::ToString( maxLongitude);
	strScopeLongitude = tmpScopeLongitude->Replace("min",min);
	strScopeLongitude = strScopeLongitude->Replace("max",max);

	//������̘A��
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
