#ifndef Response_H
#define Response_H

#include <fstream>
#include <iostream>
#include <sstream>
#include "Request.h"

#define OPEN_FILE 1
#define CLOSE_FILE 2
#define DELETE_FILE 3

class Response
{
	// Data members
	string m_Protocol;
	int m_StatusNumber;
	string m_StatusText;
	map<string, string> m_Headers;
	string m_Body;

	//Methos
public:
	void UpdateResponse(const Request& i_Request);
	string GetResponse() const;

private:
	void initResponse();
	void handleGetRequest(const Request& i_Request);
	void handlePostRequest(const Request& i_Request);
	void handlePutRequest(const Request& i_Request);
	void handleDeleteRequest(const Request& i_Request);
	void handleHeadRequest(const Request& i_Request);
	void handleTraceRequest(const Request& i_Request);
	void handleOptionsRequest(const Request& i_Request);
	string getFilePathByLanguage(const Request& i_Request);
	static bool fileExists(const string& filename);
	void notFoundStatus(string filePath);//404
	void conflictStatus(string filePath);//409
	void OKStatus();//200
	void noContentStatus();//204
	void internalServerErrorStatus(int what, string filePath); //500
	void createdStatus(); //201
	void notAllowedStatus(); //405

	void handleResponse(const Request& i_Request);
	void setHeaders(const Request& i_Request);
};


#endif // !Response_H
