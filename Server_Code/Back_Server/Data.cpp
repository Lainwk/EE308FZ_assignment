#include "Data.h"

Data* Data::instance = nullptr;

Data::Data()
{
}

Data::~Data()
{
}

Data* Data::getInstance()
{
	if (instance == nullptr) {
		instance = new Data();
	}
	return instance;
}




