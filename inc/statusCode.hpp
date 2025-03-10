#pragma once

#include <string>

enum
{
	CONTINUE = 100,
	OK = 200,
	CREATED,
	ACCEPTED,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY,
	FOUND,
	SEE_OTHER,
	NOT_MODIFIED,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT,
	BAD_REQUEST = 400,
	UNAUTHORIZED,
	FORBIDDEN = 403,
	NOT_FOUND,
	METHOD_NOT_ALLOWED,
	NOT_ACCEPTABLE,
	REQUEST_TIMEOUT = 408,
	CONTENT_TOO_LARGE = 413,
	UNSUPPORTED_MEDIA_TYPE = 415,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED,
};

bool isValidStatusCode(int code);

std::string	getStatusCodeName(int code);


/*
Malformed request	        400 Bad Request	                Syntax or semantic errors in the request
Unsupported HTTP method	    405 Method Not Allowed	        Method not supported for the resource
Unsupported HTTP version	505 HTTP Version Not Supported	HTTP version not supported
Missing or invalid headers	400 Bad Request	                Required headers are missing or invalid
Body size exceeds limits	413 Payload Too Large	        Request body exceeds server's limit
Unsupported content type	415 Unsupported Media Type	    Request uses an unsupported media type
Authentication required	    401 Unauthorized	            Request requires authentication credentials
Forbidden access	        403 Forbidden	                Server refuses to authorize the request
Resource not found	        404 Not Found	                Requested resource does not exist 
*/
