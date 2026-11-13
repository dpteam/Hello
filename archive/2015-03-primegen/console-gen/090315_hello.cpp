#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <ctime>
#include <fstream>
#include <iostream>

using namespace std;
std::ofstream logfile;

int nPrime;
int Success;

bool isPrime (int n);
void Logger_WriteFile (char *reason);

// *** main ***

int FindPrimeNumber()
{
    do
    {
        nPrime += rand()%INT_MAX+2;
        std::cout << nPrime << std::endl;
    } while(isPrime(nPrime));
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

bool isPrime (int n)        // Функция проверки простого числа
{
    for (int i = 2; i < sqrt(n); i++)
        if (n % i == 0)
            return false;   // число делится, сл. непростое
    return n > 1;
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
