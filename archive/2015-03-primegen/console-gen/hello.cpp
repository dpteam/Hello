#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;
std::ofstream logfile;

int Success;

int PrimeNumber(int number);
int NextPrimeNumber(int previous);
void Logger_WriteFile (char *reason);

// *** main ***

//RAND_MAX

int FindPrimeNumber()
{
    int n1;
    int n2;
    int n1prime;
    int n2prime;
    int n;
	do
	{
        n1 << rand();
        if(PrimeNumber(n1)==1)
            n1prime << n1;
        n2 << rand();
        if(PrimeNumber(n2)==2)
            n2prime << n2;
        n << n1prime + n2prime;
		std::cout << n << std::endl;
	} while(PrimeNumber(n)==1);
	return Success = 1;
}

int main(int argc, char *argv[])
{
	system("title Hello");
	logfile.open("hello.log", std::ofstream::out | std::ofstream::trunc);
	while(Success == 0)
	{
		FindPrimeNumber();
	}
	Logger_WriteFile("Info: Hello is closed");
	logfile.close();
	system("pause");
	return 0;
}

int PrimeNumber(int number)
{
	for (int i=2; i<number; i++)
	{
		if (number%i == 0)
		{
			return 0;
		}
		if ((i==number) || (i>sqrt(number)))
		{
			return 1;
		}
	}
	return 1;
}

int NextPrimeNumber(int previous)
{
	int temp = previous + 1;
	int next = 0;
	int exit = 0;
	int i = 2;
	
	while (exit == 0)
	{
		if (temp%i == 0 && i!=temp)
		{
			temp++;
			i=2;
			continue;
		}
		if ((i==temp) || (i>sqrt(temp)))
		{
			next = temp;
			exit = 1;
		}
		i++;
	}
	return next;
}

void Logger_WriteFile (char *reason)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	
	strftime(buffer,80,"[%H:%M:%S] ",timeinfo); //"%d-%m-%Y %I:%M:%S"
	std::string str(buffer);
	
	logfile << str << reason << "\n" << endl;
}
