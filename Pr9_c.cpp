// Pr9_c.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "net.h"

typedef struct
{
	std::string s;
	int k;

} test;


int main(int argc, char* argv[])
{

	BormNet Bn;
	boost::thread th = boost::thread(boost::bind(&BormNet::accept_command, &Bn));
 	while (true)
	{

	}

	return 0;
}