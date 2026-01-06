#pragma once
#include "Protocol.h"
#include <list>
#include <iostream>
using namespace std;
class Data
{
public:
	list<string> codelist;
	~Data();
	static Data* getInstance();
private:
	void initData();
	Data();
	static Data* instance;


};

