#ifndef Request_H
#define Request_H

#define NONE 0
#define GET 1
#define HEAD 2
#define POST 3
#define PUT 4
#define DELETE 5
#define TRACE 6
#define OPTIONS 7

#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Request
{
	// Data members
	int m_Type;
	string m_Path;
	map<string, string> m_Queries;
	string m_Protocol;
	map<string, string> m_Headers;
	string m_Body;

public:

	void UpdateRequest(char* i_RrecMessege, size_t i_Length);
	//gets methods
	int GetType() const {return this->m_Type;} 
	const string& GetPath() const{ return this->m_Path; }
	const map<string, string>& GetQueries() const{ return this->m_Queries; }
	const string& GetProtocol() const{ return this->m_Protocol; }
	const map<string, string>& GetHeaders() const{ return this->m_Headers; }
	const string& GetBody() const{ return this->m_Body;}

private:

	void initRequest();
	void SetType(char* i_RrecMessege, size_t& i_Index);
	void SetPathAndQueries(char* i_RrecMessege, size_t& i_Index);
	void SetProtocol(char* i_RrecMessege, size_t& i_Index);
	void SetHeaders(char* i_RrecMessege, size_t& i_Index);
	void SetBody(char* i_RrecMessege, size_t& i_Index, size_t& i_Length);

};

#endif
