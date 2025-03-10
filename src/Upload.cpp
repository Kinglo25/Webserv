#include "HttpRequest.hpp"

void HttpRequest::handleFileUpload()
{
    // Check if the Content-Type header is set to multipart/form-data
    if (headers_.find("Content-Type") != headers_.end() && headers_["Content-Type"].find("multipart/form-data") != std::string::npos) {
        std::string boundary = headers_["Content-Type"].substr(headers_["Content-Type"].find("boundary=") + 9);
        
        // Parse the multipart/form-data content
        parseMultipartFormData(boundary);
    }
}

void HttpRequest::parseMultipartFormData(std::string &boundary)
{
    boundary = boundary.substr(0, boundary.find("\r"));
    std::string boundaryStart = "--" + boundary;
    std::string boundaryEnd = "--" + boundary + "--";

    // Find the file name in the Content-Disposition header
    std::string::size_type start = body_.find("filename=\"");
    if (start == std::string::npos) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    start += 10;
    std::string::size_type end = body_.find("\"", start);
    if (end == std::string::npos) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    fileName_ = body_.substr(start, end - start);

    // Find the start of the first boundary
    start = body_.find("\r\n\r\n", end);
    if (start == std::string::npos) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    start += 4; // Move past the "\r\n\r\n"
    // Find the start of the end boundary
    end = body_.find(boundaryEnd, start);
    if (end == std::string::npos) {
        throw HttpRequestException(BAD_REQUEST, getStatusCodeName(BAD_REQUEST));
    }
    // Save the body content between the boundaries
    std::ofstream file;
    file.open(fileName_.c_str(), std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        throw HttpRequestException(INTERNAL_SERVER_ERROR, getStatusCodeName(INTERNAL_SERVER_ERROR));
    }
    file.write(body_.c_str() + start, end - start - 2); // Subtract 2 to remove the trailing "\r\n"
    file.close();
}
