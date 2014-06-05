#include <opencv2\opencv.hpp>

#include "ConnectingDB\ConnectingDB.h"

int main()
{
	ConnectingDB db;

	db.loadDB();

	return 0;
}