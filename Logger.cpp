#include <iostream>
#include <fstream>
#include "IndProj.h"

using namespace std;

void Flush()
{
	system("cls");
	ofstream output("log.txt");
	output.flush();
	output.close();
}

void Enter()
{
	cout << '\n';
	ofstream output("log.txt", ios::app);
	output << '\n';
	output.close();
}

void LogStr(string message)
{
	cout << message;
	ofstream output("log.txt", ios::app);
	output << message;
	output.close();
}
void Log(bool message)
{
	cout << message;
	ofstream output("log.txt", ios::app);
	output << message;
	output.close();
}
void Log(int message)
{
	cout << message;
	ofstream output("log.txt", ios::app);
	output << message;
	output.close();
}
void Log(float message)
{
	cout << message;
	ofstream output("log.txt", ios::app);
	output << message;
	output.close();
}
void Log(Vector message)
{
	cout << "{ " << message.X << "; " << message.Y << " }";
	ofstream output("log.txt", ios::app);
	output << "{ " << message.X << "; " << message.Y << " }";
	output.close();
}