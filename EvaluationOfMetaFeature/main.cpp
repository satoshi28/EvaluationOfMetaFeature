#include <opencv2\opencv.hpp>

#include "ConnectingDB\ConnectingDB.h"

int main()
{
	ConnectingDB db;

	db.loadDB(38.0, 141.0);

	return 0;
}