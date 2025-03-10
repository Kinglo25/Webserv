#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>
#include <sstream>

// Represents an HTTP response.
class HttpResponse {
public:
	// Default constructor
	HttpResponse() : statusCode_(0) {}

    // Constructor: Initializes response with status code and message
    HttpResponse(int statusCode, const std::string &statusMsg);

    // Set a header in the response
    void setHeader(const std::string &header, const std::string &value);

    // Set the body content of the response
    void setBody(const std::string &body);

    // Convert the response to a string format
    std::string toString();

	// Set the status code of the response
	void setStatusCode(int statusCode);

	// Set the status message of the response
	void setStatusMsg(const std::string &statusMsg);

private:
    int statusCode_;              					// HTTP status code (e.g., 200)
    std::string statusMsg_;       					// HTTP status message (e.g., OK)
    std::map<std::string, std::string> headers_;  	// HTTP headers
    std::string body_;            					// Response body content

    // Get the current date and time
    std::string getCurrentDate() const;
};

#endif // HTTP_RESPONSE_HPP