#pragma once

#include "stdafx.h"

using namespace std;
using namespace boost;
using namespace boost::container;
#ifndef PARAM
#define PARAM

#include "stdafx.h"

enum defaultType
{
	Char = 0,
	String = 1,
	Int = 2,
	Double = 3,
	CharPointer = 4
};

struct A {
	static std::map<int, int> create_map()
	{
		std::map<int, int> m;
		m[defaultType::Int] = sizeof(int);
		m[defaultType::Char] = 1;
		m[defaultType::String] = 100;
		m[defaultType::Double] = sizeof(double);
		m[defaultType::CharPointer] = sizeof(char*);
		return m;
	}
	static const std::map<int, int> sizeMap;

};

typedef defaultType dType;

class Param
{

public:
	Param(defaultType a1, defaultType a2, defaultType a3, defaultType a4)
	{
		defaultType arr[4] = { a1, a2 , a3 , a4 };
		set(4, arr);

	}

	Param(defaultType a1, defaultType a2, defaultType a3)
	{
		defaultType arr[3] = { a1, a2 , a3 };
		set(3, arr);
	}

	Param(defaultType a1, defaultType a2)
	{
		defaultType arr[2] = { a1, a2 };
		set(2, arr);

	}

	Param(defaultType a1)
	{
		defaultType arr[1] = { a1 };
		set(1, arr);
	}
	Param()
	{

	}

private:
	int fAllocated;
	int allocated;

	int allocMap[5];

	int pNum;

	std::map<std::string, char*> aMap;

	char* p_args[10];
	char* P;

	void allocate_bloc(int a) { fAllocated = a;  P = new char[a]; p_args[0] = P; };
	void allocate_at(int idx, defaultType t)
	{
		allocMap[idx] = A::sizeMap.find(t)->second;
		p_args[idx] = P + allocated;

		allocated += allocMap[idx];

	};

	int get_all_size(int num, defaultType* arr)
	{
		int Allocated = 0;

		for (int i = 0; i < num; i++)
		{
			Allocated += A::sizeMap.find(*arr++)->second;

		}

		return Allocated;

	}

protected:

	void set(int num, defaultType* tar)
	{
		fAllocated = get_all_size(num, tar);
		pNum = num;

		allocate_bloc(fAllocated);

		for (int i = 0; i < num; i++)
			allocate_at(i, *tar++);

	}

public:

	void set_args(int idx, std::string s)
	{
		aMap.insert(std::pair<std::string, char*>(s, p_args[idx]));
	};

	template <typename T>
	T get_value(std::string s);
	template <typename T>
	void set_value(std::string name, T& v);
};

template <typename T>
T Param::get_value(std::string s)
{
	auto it = aMap.find(s);

	if (it == aMap.end())
	{

	}

	else
		return (T)*aMap.find(s)->second;

};

template <typename T>
void Param::set_value(std::string name, T& v)
{
	memcpy((void*)aMap.find(name)->second, &v, sizeof(v));

	return;
}


#endif
class nof
{
	short type;

	int rowid_checked_last;
	bool _new;

	std::string _tag;

public:
	nof() : _new(false)
	{};

	void resee() { };
	bool get_new() { return _new; };

	inline void task_end() { _new = false; }

	void renew() { _new = true; };

	void set_params(defaultType a0, defaultType a1, defaultType a2, defaultType a3)
	{
		pram_point = new Param(a0, a1, a2, a3);
	};

	void set_params(defaultType a0, defaultType a1)
	{
		pram_point = new Param(a0, a1);
	};

	void set_params(defaultType a0, defaultType a1, defaultType a2)
	{
		pram_point = new Param(a0, a1, a2);
	};

	void set_params(defaultType a0)
	{
		pram_point = new Param(a0);
	};

	void set_tag(std::string s)
	{
		_tag = s;
	}
	template <class T>
	T get_value(std::string s) { return *pram_point->get_args<T>(s); };

	Param* get_param() { return pram_point; };

	template <class T>
	void set_value(std::string s, T& t) { pram_point->set_args<T>(s, t); };

private:
	Param* pram_point;

};

class notifier
{

public:
	notifier(int id) {};
	notifier();

private:

	int uid;
	boost::array<nof, 10> notification;

public:

	bool any();

	void task_end(int i) { notification[i].task_end(); };
	bool get_notification(int i) { return notification[i].get_new(); };

	nof* at(int i) { return &notification[i]; };

};


class nMap
{
public:
	nMap() {};

	notifier* insert(int id) {
		auto res = Map.insert(std::pair<int, notifier>(id, notifier()));
		return &res.first->second;
	};

	notifier* get_notifier(int id)
	{
		return &Map.find(id)->second;
	};

private:
	std::map<int, notifier > Map;

};



