#include "Response.h"

// Update the response based on the type of request received
void Response::UpdateResponse(const Request& i_Request)
{
    // Initialize/clear the response
    initResponse(); 

    // Set the protocol (e.g., HTTP/1.1)
    this->m_Protocol = i_Request.GetProtocol(); 

    // Determine the type of request and handle accordingly
    handleResponse(i_Request);

    //set headers
    setHeaders(i_Request);
}

// Initialize/clear the response
void Response::initResponse()
{
    this->m_Body.clear();
    this->m_Headers.clear();
    this->m_Protocol.clear();
    this->m_StatusNumber = NONE;
    this->m_StatusText.clear();
}

// Get the response as a string
string Response::GetResponse() const
{
    stringstream response;
    response << this->m_Protocol << " " << to_string(this->m_StatusNumber) << " " + this->m_StatusText << "\r\n";

    for (const auto& header : this->m_Headers)
    {
        response << header.first << ": " << header.second << "\r\n";
    }

    response << ("\r\n" + this->m_Body);
    return response.str();
}

// Handle GET request
void Response::handleGetRequest(const Request& i_Request)
{
    //Get the file path 
    string filePath = getFilePathByLanguage(i_Request);

    //If the file does noe exits
    if (!fileExists(filePath)) 
    {    notFoundStatus(filePath);
        return; }

    //Attempt to open the file for reading
    ifstream infile;
    infile.open(filePath,ios_base::in); 

    //If there is an error opening file
    if (!infile.is_open()) 
    {   internalServerErrorStatus(OPEN_FILE, filePath);
        return;  }

    // Read file contents into buffer
    stringstream buffer;
    buffer << infile.rdbuf(); 

    // Close the file
    infile.close(); 

    //If there is an error closing the file
    if (infile.is_open())
    { internalServerErrorStatus(CLOSE_FILE, filePath);
        return; }

    //Set successful response
    OKStatus();
    this->m_Body = buffer.str(); // Set response body to file contents
}

// Handle POST request
void Response::handlePostRequest(const Request& i_Request)
{
    //Get the file path 
    string filePath = getFilePathByLanguage(i_Request);

    //If file is already exits
    if (fileExists(filePath))
    {   conflictStatus(filePath);
        return; }

    //Attempt to create a new file for writing
    ofstream outfile;
    outfile.open(filePath, ios_base::out);

    //If there is an error opening the file
    if (!outfile.is_open()) 
    {    internalServerErrorStatus(OPEN_FILE, filePath);
        return; }

    // Write request body to file
    outfile << i_Request.GetBody(); 

    // Close the file
    outfile.close();

    //If there is an error closing the file
    if (outfile.is_open()) {
        internalServerErrorStatus(CLOSE_FILE, filePath);
        return; }

    //Set successful response
    createdStatus();
}

// Handle PUT request
void Response::handlePutRequest(const Request& i_Request)
{
    //Get the file path 
    string filePath = getFilePathByLanguage(i_Request);
    
    //check if the file exits (for a Status Number)
    bool fileExisted = fileExists(filePath);

    // Open file for writing (create or overwrite)
    std::ofstream outfile(filePath, std::ofstream::out | std::ofstream::trunc);

    //If there is an error opening file
    if (!outfile.is_open()) 
    { internalServerErrorStatus(OPEN_FILE, filePath);
        return;  }

    // Write request body to file
    outfile << i_Request.GetBody();

    // Close the file
    outfile.close(); 

    //If there is an error closing the file
    if (outfile.is_open()) 
    {   internalServerErrorStatus(CLOSE_FILE, filePath);
        return;}

    // Determine response code based on whether the file existed before
    if (!fileExisted)
    {
        createdStatus();
    }
    else
    {
        OKStatus();
        this->m_Body = "The file was updated successfully.";
    }
}

// Handle DELETE request
void Response::handleDeleteRequest(const Request& i_Request)
{
    //Get the file path 
    string filePath = getFilePathByLanguage(i_Request);

    //If the file does noe exits
    if (!fileExists(filePath)) 
    {  notFoundStatus(filePath);
        return;  }

    // Delete the file
    if (remove(filePath.c_str()) != 0) 
    {  internalServerErrorStatus(DELETE_FILE, filePath);
        return;  }

    //Set successful response
    noContentStatus();
}

// Handle HEAD request
void Response::handleHeadRequest(const Request& i_Request)
{
    //Get the file path 
    string filePath = getFilePathByLanguage(i_Request);

    //If the file does noe exits
    if (!fileExists(filePath))
    {     notFoundStatus(filePath);
          this->m_Body.clear();
        return; }

    //Attempt to open the file for reading
    ifstream infile;
    infile.open(filePath, ios_base::ate);

    //If there is an error opening file
    if (!infile.is_open()) 
    {   internalServerErrorStatus(OPEN_FILE, filePath);
        return; }

    //Get file length
    long fileSize = infile.tellg();
    string length;
    if (fileSize != -1)
    {
        length = to_string(fileSize);
    }
    else
    {
        length = to_string(0);
    }

    //Update Content-Length header
    this->m_Headers["Content-Length"] = length;

    // Close the file
    infile.close(); 

    //If there is an error closing the file
    if (infile.is_open())
    {     internalServerErrorStatus(CLOSE_FILE, filePath);
        return;  }

    //Set successful response
    OKStatus();
}

// Handle TRACE request
void Response::handleTraceRequest(const Request& i_Request)
{
    OKStatus();
    this->m_Body = i_Request.GetBody(); // Echo back the request body
}

// Handle OPTIONS request
void Response::handleOptionsRequest(const Request& i_Request)
{
    OKStatus();
    this->m_Headers["Allow"] = "GET, POST, PUT, DELETE, HEAD, TRACE, OPTIONS";
}

// Check if a file exists
bool Response::fileExists(const string& filename)
{
    ifstream infile(filename);
    return infile.good();
}

//Appends a language suffix to the file name based on the "lang" query parameter.
string Response::getFilePathByLanguage(const Request& i_Request)
{
    stringstream filePath;
    //Adjust file path 
    filePath << "c:/temp" << i_Request.GetPath();
    string fileName, fileEnding;

    getline(filePath, fileName, '.');
    getline(filePath, fileEnding);

    //Handle queries (if exist)
    map<string, string> requestQueries = i_Request.GetQueries();

    if (requestQueries.find("lang") != requestQueries.end())
    {
        if (requestQueries["lang"] == "he")
        {
            fileName.append("_he");
        }
        else if (requestQueries["lang"] == "fr")
        {
            fileName.append("_fr");
        }
    }

    return fileName.append(".").append(fileEnding);
}

//Sets the response status to 404 Not Found.
void Response::notFoundStatus(string filePath)//404
{
    this->m_StatusNumber = 404;
    this->m_StatusText = "Not Found";
    this->m_Body = "The file was not found.";
    cerr << "The file was not found:" << filePath << endl;
}

//Sets the response status to 409 Conflict.
void Response::conflictStatus(string filePath)
{
    this->m_StatusNumber = 409; 
    this->m_StatusText = "Conflict";
    this->m_Body = "The file already exists.";
    std::cerr << "The file already exists: " << filePath << std::endl;
}

//Sets the response status to 500 Internal Server Error.
void Response::internalServerErrorStatus(int what, string filePath) //500
{
    this->m_StatusNumber = 500;
    this->m_StatusText = "Internal Server Error";
    if (what == OPEN_FILE)
    {
        this->m_Body = "Failed to open file.";
        std::cerr << "Failed to open file: " << filePath << std::endl;
    }
    else if(what == CLOSE_FILE)
    {
        this->m_Body = "Failed to close the file.";
        cerr << "Failed to close the file." << filePath << endl;
    }
    else
    {
        this->m_Body = "Error deleting file: " + filePath;
        cerr << "Error deleting file: " << filePath << endl;
    }
}

//Sets the response status to 200 OK.
void Response::OKStatus()//200
{
    this->m_StatusNumber = 200;
    this->m_StatusText = "OK";
}

//Sets the response status to 201 Created.
void Response::createdStatus()
{
    this->m_StatusNumber = 201;
    this->m_StatusText = "Created";
    this->m_Body = "The resource was successfully created.";
}

//Sets the response status to 204 No Content.
void Response::noContentStatus()//204
{
    this->m_StatusNumber = 204;
    this->m_StatusText = "No Content";
}

//Sets the response status to 405 Not Allowed.
void Response::notAllowedStatus()
{
    this->m_StatusNumber = 405;
    this->m_StatusText = "Method Not Allowed";
    this->m_Headers["Allow"] = "GET, POST, PUT, DELETE, HEAD, TRACE, OPTIONS";
}

// Determine the type of request and handle accordingly
void Response::handleResponse(const Request& i_Request)
{
    switch (i_Request.GetType())
    {
    case GET:
        handleGetRequest(i_Request);
        break;
    case POST:
        handlePostRequest(i_Request);
        break;
    case PUT:
        handlePutRequest(i_Request);
        break;
    case DELETE:
        handleDeleteRequest(i_Request);
        break;
    case HEAD:
        handleHeadRequest(i_Request);
        break;
    case TRACE:
        handleTraceRequest(i_Request);
        break;
    case OPTIONS:
        handleOptionsRequest(i_Request);
        break;
    default:
        //The server does not support the request type
        notAllowedStatus();
        break;
    }
}

//Set response headers
void  Response::setHeaders(const Request& i_Request)
{
    // Set Content-Type if provided in request headers
    map<string, string> requestHeaders = i_Request.GetHeaders();
    if (requestHeaders.find("Accept") != requestHeaders.end())
    {
        this->m_Headers["Content-Type"] = "text/html";
    }
    // Set the Content-Length header
    if (i_Request.GetType() != HEAD || this->m_StatusNumber == 404)
    {
        this->m_Headers["Content-Length"] = to_string(this->m_Body.length());
    }
}