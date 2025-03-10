#include "HttpRequest.hpp"

// Default constructor
HttpRequest::HttpRequest() : contentLength_(0) {}

// Constructor that parses and checks the raw HTTP request
HttpRequest::HttpRequest(const std::string &rawRequest) : contentLength_(0) {
    // Parse the raw request
    parse(rawRequest);
    // Handle file uploads
    handleFileUpload();
    // Check the request for errors
    check();
}

// Copy constructor
HttpRequest::HttpRequest(const HttpRequest &cpy)
    : method_(cpy.method_), path_(cpy.path_), version_(cpy.version_),
      headers_(cpy.headers_), body_(cpy.body_), contentLength_(cpy.contentLength_), 
	  request_(cpy.request_), port_(cpy.port_), host_(cpy.host_), query_(cpy.query_),
      fileName_(cpy.fileName_) {}

// Copy assignment operator
HttpRequest& HttpRequest::operator=(const HttpRequest &rhs) {
    if (this != &rhs) {
        method_ = rhs.method_;
        path_ = rhs.path_;
        version_ = rhs.version_;
        headers_ = rhs.headers_;
        body_ = rhs.body_;
        contentLength_ = rhs.contentLength_;
		port_ = rhs.port_;
		host_ = rhs.host_;
		request_ = rhs.request_;
        query_ = rhs.query_;
        fileName_ = rhs.fileName_;
    }
    return *this;
}

// Destructor
HttpRequest::~HttpRequest() {}

// Parses the raw request string and fills the HttpRequest fields
void HttpRequest::parse(const std::string &rawRequest) {
    std::istringstream issRequest(rawRequest);
    std::string line;

    // Parse the first line
    if (!std::getline(issRequest, line)) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    std::istringstream issLine(line);
    if (!(issLine >> method_ >> request_ >> version_)) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
	// Extract the path from the request target
    request_ = urlDecode(request_);
	std::string::size_type qPos = request_.find('?');
	if (qPos != std::string::npos) {
		path_ = request_.substr(0, qPos);
		query_ = request_.substr(qPos);
	}
	else {
		path_ = request_;
		query_ = "";
	}
    // Parse headers
    while (std::getline(issRequest, line) && !line.empty() && line != "\r") {
        std::string::size_type colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
        // Extract the key and value from the line using the colon position
        std::string key = line.substr(0, colonPos);
        // Skip the space after the colon and extract the value until the end of the line (excluding the trailing \r)
        std::string value = line.substr(colonPos + 2, line.size() - colonPos - 2);
        headers_[key] = value;
    }
	// Extract the host and port from the Host header
	std::string host = getHeader("Host");
	if (!host.empty()) {
		std::istringstream issHost(host);
		std::getline(issHost, host_, ':');
		if (!(issHost >> port_)) {
			// If no port is specified, you might want to set a default port, e.g., 80 for HTTP
			port_ = 80;
		}
	} else {
    // Handle the case where the Host header is missing or empty
    	host_ = "localhost"; // Default host
    	port_ = 80; // Default port
	}
    // Parse the body if Transfer-Encoding is chunked
    if (headers_.find("Transfer-Encoding") != headers_.end() && headers_["Transfer-Encoding"] == "chunked") {
        std::string::size_type bodyPos = rawRequest.find("\r\n\r\n");
        if (bodyPos == std::string::npos) {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
        bodyPos += 4; // Move past the "\r\n\r\n"
        std::string chunkedBody = rawRequest.substr(bodyPos);
        body_ = unchunkBody(chunkedBody);
    } 
    // Parse the body if Content-Length is provided
    else if (headers_.find("Content-Length") != headers_.end()) {
        std::istringstream iss(headers_["Content-Length"]);
        if (!(iss >> contentLength_)) {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
        // Check body
        if (rawRequest.size() < contentLength_) {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
        body_ = rawRequest.substr(rawRequest.size() - contentLength_);
    }
}

// Remove the chunked encoding from the body
std::string HttpRequest::unchunkBody(const std::string &chunkedBody) {
    std::string unchunkedBody;
    std::istringstream iss(chunkedBody);
    std::string line;

    while (std::getline(iss, line)) {
        // Remove any trailing CRLF from the line
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        // Convert the chunk size from hexadecimal to decimal
        std::istringstream chunkSizeStream(line);
        size_t chunkSize;
        if (!(chunkSizeStream >> std::hex >> chunkSize)) {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
        // Stop reading chunks if the size is 0
        if (chunkSize == 0) {
            break;
        }
        // Read the chunk data
        std::string chunk(chunkSize, '\0');
        if (!iss.read(&chunk[0], chunkSize)) {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
        unchunkedBody += chunk;

        // Read the trailing CRLF after the chunk
        if (!std::getline(iss, line) || line != "\r") {
            throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
        }
    }
    return unchunkedBody;
}

// Check if the request is valid
void HttpRequest::check() const {
    // Check method
    if (method_ != "GET" && method_ != "POST" && method_ != "DELETE") {
        throw HttpRequestException(METHOD_NOT_ALLOWED, getStatusCodeName(METHOD_NOT_ALLOWED));
    }
    // Check version
    if (version_ != "HTTP/1.1") {
        throw HttpRequestException(HTTP_VERSION_NOT_SUPPORTED, getStatusCodeName(HTTP_VERSION_NOT_SUPPORTED));
    }
    // Check path
    if (path_.empty() || path_[0] != '/') {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    // Check headers
    if (headers_.find("Host") == headers_.end()) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    // Additional checks for specific methods
    if ((method_ == "POST" || method_ == "PUT") &&
        (headers_.find("Content-Length") == headers_.end()
         && headers_.find("Transfer-Encoding") == headers_.end())) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    // Check body
    if (headers_.find("Content-Length") != headers_.end() && body_.size() != contentLength_) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
}

// Getter for HTTP method
std::string HttpRequest::getMethod() const {
    return method_;
}

// Getter for the requested path
std::string HttpRequest::getPath() const {
    return path_;
}

// Getter for HTTP version
std::string HttpRequest::getVersion() const {
    return version_;
}

// Getter for a specific header value
std::string HttpRequest::getHeader(const std::string &header) const {
    std::map<std::string, std::string>::const_iterator it = headers_.find(header);
    if (it != headers_.end()) {
        return it->second;
    }
    return "";
}

// Getter for the body content
std::string HttpRequest::getBody() const {
    return body_;
}

// Getters for the request target
std::string HttpRequest::getRequest() const {
	return request_;
}

// Getter for the port number
u_int32_t HttpRequest::getPort() const {
	return port_;
}

// Getter for the host name
std::string HttpRequest::getHost() const {
	return host_;
}

// Getter for the query string
std::string HttpRequest::getQuery() const {
	return query_;
}

// Getter for the file name in the request
std::string HttpRequest::getFileName() const {
	return fileName_;
}

// Setter for headers
void HttpRequest::setHeader(const std::string &key, const std::string &value) {
	headers_[key] = value;
}