#include "Request.h"

//Update the data member of the request according to the content of the client's request 
void Request::UpdateRequest(char* i_RecMessage, size_t i_Length)
{
	size_t index = 0;
	initRequest();
	//cout << i_RecMessage;
	SetType(i_RecMessage, index);
	SetPathAndQueries(i_RecMessage, index);
	SetProtocol(i_RecMessage, index);
	SetHeaders(i_RecMessage, index);
	SetBody(i_RecMessage, index, i_Length);
}

//Initialize/clear the request
void Request::initRequest()
{
	this->m_Type = NONE;
	this->m_Path.clear();
	this->m_Queries.clear();
	this->m_Protocol.clear();
	this->m_Headers.clear();
	this->m_Body.clear();
}

//Get the type request from the client message and update m_Type member
void Request::SetType(char* i_RecMessage, size_t& i_Index)
{
	char type[10];
	
	//copy the type from the message
	while (i_RecMessage[i_Index] != ' ')
	{
		type[i_Index] = i_RecMessage[i_Index];
		i_Index++;
	}
	type[i_Index++] = '\0';

	//update m_Type member.
	if (strcmp(type, "GET") == 0)
	{
		this->m_Type = GET;
	}
	else if (strcmp(type, "POST") == 0)
	{
		this->m_Type = POST;
	}
	else if (strcmp(type, "PUT") == 0)
	{
		this->m_Type = PUT;
	}
	else if (strcmp(type, "DELETE") == 0)
	{
		this->m_Type = DELETE;
	}
	else if (strcmp(type, "HEAD") == 0)
	{
		this->m_Type = HEAD;
	}
	else if (strcmp(type, "TRACE") == 0)
	{
		this->m_Type = TRACE;
	}
	else if (strcmp(type, "OPTIONS") == 0)
	{
		this->m_Type = OPTIONS;
	}
}

//Get the path and if exist get queries parameters from the client message and update m_Path and m_Queries members
void Request::SetPathAndQueries(char* i_RecMessage, size_t& i_Index)
{

	string path, firstBuffString, secondBuffString;

	//copy the path from the message
	while (i_RecMessage[i_Index] != '?' && i_RecMessage[i_Index] != ' ')
	{
		path.push_back(i_RecMessage[i_Index]);
		i_Index++;
	}
	//update m_Path member
	this->m_Path = path;
	path.clear();

	//check for queries
	if (i_RecMessage[i_Index] == '?')
	{
		//copy the queries data and update m_Queries member
		i_Index++;
		while (i_RecMessage[i_Index] != ' ')
		{
			while (i_RecMessage[i_Index] != '=' && i_RecMessage[i_Index] != ' ')
			{
				firstBuffString.push_back(i_RecMessage[i_Index]);
				i_Index++;
			}

			i_Index++;

			while (i_RecMessage[i_Index] != '&' && i_RecMessage[i_Index] != ' ')
			{
				secondBuffString.push_back(i_RecMessage[i_Index]);
				i_Index++;
			}

			this->m_Queries[firstBuffString] = secondBuffString;
			firstBuffString.clear();
			secondBuffString.clear();
		}
	}

	i_Index++;
}

//Get the protocol of the request from the client message and update m_Protocol member
void Request::SetProtocol(char* i_RecMessage, size_t& i_Index)
{
	string protocol;

	//copy the protocol from the message
	while (i_RecMessage[i_Index] != '\r')
	{
		protocol.push_back(i_RecMessage[i_Index]);
		i_Index++;
	}
	i_Index += 2;

	//updte m_Protocol message.
	this->m_Protocol = protocol;
}

//Get the headers from the client message and update m_headers member
void Request::SetHeaders(char* i_RecMessage, size_t& i_Index)
{
	string key;
	string value;

	//copy the headers from the message and update m_Headers member.
	while (!(i_RecMessage[i_Index] == '\r' && i_RecMessage[i_Index - 2] == '\r'))
	{
		while (i_RecMessage[i_Index] != ':')
		{
			key.push_back(i_RecMessage[i_Index]);
			i_Index++;
		}

		i_Index += 2;

		while (i_RecMessage[i_Index] != '\r')
		{
			value.push_back(i_RecMessage[i_Index]);
			i_Index++;
		}

		m_Headers.insert(std::pair<string, string>(key, value));
		key.clear();
		value.clear();
		i_Index += 2;
	}
	i_Index += 2;

}

//Get the body from the client message (if exist) and update m_Body member
void Request::SetBody(char* i_RecMessage, size_t& i_Index, size_t& i_Length)
{
	string body;

	while (i_RecMessage[i_Index] != '\0')
	{
		body.push_back(i_RecMessage[i_Index]);
		i_Index++;
	}

	this->m_Body = body;
}
