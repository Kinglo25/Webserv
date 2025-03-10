#ifndef ERROR_HPP
#define ERROR_HPP

#include <exception>
#include <string>
#include <stdexcept>

// Exception class for HTTP request errors
class HttpRequestException : public std::exception {
public:
    // Constructor: Initializes with a status code and message
    HttpRequestException(int code, const std::string &message) 
        : statusCode_(code), statusMsg_(message) {}

    // Get the HTTP error code
    int code() const { return statusCode_; }

    // Return the error message
    virtual const char* what() const throw() { return statusMsg_.c_str(); }

    // Destructor
    virtual ~HttpRequestException() throw() {}

private:
    int statusCode_;      // HTTP status code (e.g., 400, 404)
    std::string statusMsg_;  // Error message (e.g., "Bad Request")
};

// Add other error classes here, using HttpRequestException as a template !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#endif // ERROR_HPP