#include "HttpResponse.hpp"
#include "Error.hpp"

// Constructor: Initializes the status code and message
HttpResponse::HttpResponse(int statusCode, const std::string &statusMsg) {
    this->statusCode_ = statusCode;
    this->statusMsg_ = statusMsg;
}

// Set a header in the response
void HttpResponse::setHeader(const std::string &header, const std::string &value) {
    headers_[header] = value;
}

// Set the body content of the response
void HttpResponse::setBody(const std::string &body) {
    body_ = body;
}

// Get the current date and time
std::string HttpResponse::getCurrentDate() const {
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    char buf[128];
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", &tm);
    return buf;
}

// Set the status code of the response
void HttpResponse::setStatusCode(int statusCode) {
	statusCode_ = statusCode;
}

// Set the status message of the response
void HttpResponse::setStatusMsg(const std::string &statusMsg) {
	statusMsg_ = statusMsg;
}

// Convert the response to a string
std::string HttpResponse::toString() {
    std::ostringstream oss;

	if (statusCode_ == 0 || statusMsg_.empty()) {
		throw HttpRequestException(500, "Internal Server Error: Missing status code or message");
	}
    // Add status line
    oss << "HTTP/1.1 " << statusCode_ << " " << statusMsg_ << "\r\n";
	// Add date
	this->setHeader("Date", getCurrentDate());
    // Add headers
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers_.begin(); it != headers_.end(); ++it) {
        oss << it->first << ": " << it->second << "\r\n";  // Format each header as "Key: Value"
    }

    // Add body
    oss << "\r\n" << body_;
	
    return oss.str();
}