
#include "stdafx.h"
#include "nof.h"

const std::map<int, int> A::sizeMap = A::create_map();

template <>
std::string Param::get_value(std::string s)
{
	auto it = aMap.find(s);
	std::string k;

	if (it == aMap.end())
	{

	}

	else
	{
		k.resize(100);
		char* src = aMap.find(s)->second;

		memcpy((void*)k.c_str(), src, 100);
	}
	return k;

}

notifier::notifier()
{
	notification[0].set_params(dType::String, dType::String);

	Param* loginParam = notification[0].get_param();
	/*
	notification[1].set_params(dType::Int, dType::Int, dType::String);

	Param* msgreqParam = notification[1].get_param();
	msgreqParam->set_tag(0, "TableKey");
	msgreqParam->set_tag(1, "Sender");
	msgreqParam->set_tag(2, "Text");
	*/


}

bool notifier::any()
{
	auto it = notification.begin();

	for (; it != notification.end(); it++)
	{
		if (it->get_new())
			return true;
	}

	return false;
}
