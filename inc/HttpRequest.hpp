#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <map>
#include <string>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "Error.hpp"
#include "utils.hpp"
#include "statusCode.hpp"


// Represents an HTTP request. Defined in HttpRequest.cpp and Upload.cpp
class HttpRequest {
public:
    // Default constructor
    HttpRequest();

    // Constructor: Parses the raw HTTP request string
    HttpRequest(const std::string &rawRequest);

    // Copy constructor
    HttpRequest(const HttpRequest &cpy);

    // Copy assignment operator
    HttpRequest& operator=(const HttpRequest &rhs);

    // Destructor
    ~HttpRequest();

    // Get the HTTP method (e.g., GET, POST)
    std::string getMethod() const;

    // Get the requested path (e.g., /index.html)
    std::string getPath() const;

    // Get the HTTP version (e.g., HTTP/1.1)
    std::string getVersion() const;

    // Get a specific header value by header name
    std::string getHeader(const std::string &header) const;

    // Get the body content of the request
    std::string getBody() const;

    // Get the uploaded files in the request
    std::map<std::string, std::string> getUploadedFiles() const;

    // Get the form fields in the request
    std::map<std::string, std::string> getFormFields() const;

    // Get the content types of each part in the multipart/form-data content
    std::map<std::string, std::string> getPartContentTypes() const;

	// Get the request target (e.g., /index.html?q=1)
	std::string getRequest() const;

	// Get the port number
	u_int32_t getPort() const;

	// Get the host name
	std::string getHost() const;

	// Get the query string
	std::string getQuery() const;

	// Get the file name from the request
	std::string getFileName() const;
	
	// Setter for headers
	void setHeader(const std::string &header, const std::string &value);

private:
    std::string method_;                            		// HTTP method (GET, POST, etc.)
    std::string path_;                              		// Requested path (e.g., /index.html)
    std::string version_;                           		// HTTP version (e.g., HTTP/1.1)
    std::map<std::string, std::string> headers_;    		// HTTP headers as key-value pairs
    std::string body_;                             			// Body content of the request
    std::string::size_type contentLength_;                  // Length of the body (from Content-Length header)
	std::string request_;									// Requested target (e.g., /index.html?q=1)
	u_int32_t port_;										// Port number
	std::string host_;										// Host name
	std::string query_;										// Query string
	std::string fileName_;									// File name in the request

    // Parse the raw request string into components
    void parse(const std::string &rawRequest);

    // Validate the parsed request (check for errors)
    void check() const;

    // Remove the chunked encoding from the body
    std::string unchunkBody(const std::string &chunkedBody);

    // Handle file uploads in the request
    void handleFileUpload();

    // Parse the multipart/form-data content
    void parseMultipartFormData(std::string &boundary);
};

#endif // HTTP_REQUEST_HPP

/*
POST /upload HTTP/1.1
Host: localhost:3000
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABC123XYZ
Content-Length: 579

------WebKitFormBoundaryABC123XYZ
Content-Disposition: form-data; name="username"

john_doe
------WebKitFormBoundaryABC123XYZ
Content-Disposition: form-data; name="email"

john@example.com
------WebKitFormBoundaryABC123XYZ
Content-Disposition: form-data; name="MAX_FILE_SIZE"

2000000
------WebKitFormBoundaryABC123XYZ
Content-Disposition: form-data; name="profile_picture"; filename="profile.jpg"
Content-Type: image/jpeg

<binary data of profile.jpg>
------WebKitFormBoundaryABC123XYZ
Content-Disposition: form-data; name="resume"; filename="resume.pdf"
Content-Type: application/pdf

<binary data of resume.pdf>
------WebKitFormBoundaryABC123XYZ--
*/