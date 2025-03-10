#include "statusCode.hpp"

/*
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
*/

bool isValidStatusCode(int code)
{
	if (code == OK
		|| code == FOUND
		|| code == CREATED
		|| code == ACCEPTED
		|| code == CONTINUE
		|| code == NOT_FOUND
		|| code == SEE_OTHER
		|| code == FORBIDDEN
		|| code == BAD_REQUEST
		|| code == NOT_MODIFIED
		|| code == UNAUTHORIZED
		|| code == NOT_ACCEPTABLE
		|| code == GATEWAY_TIMEOUT
		|| code == NOT_IMPLEMENTED
		|| code == REQUEST_TIMEOUT
		|| code == MULTIPLE_CHOICES
		|| code == CONTENT_TOO_LARGE
		|| code == MOVED_PERMANENTLY
		|| code == TEMPORARY_REDIRECT
		|| code == PERMANENT_REDIRECT
		|| code == METHOD_NOT_ALLOWED
		|| code == INTERNAL_SERVER_ERROR
		|| code == UNSUPPORTED_MEDIA_TYPE
		|| code == HTTP_VERSION_NOT_SUPPORTED
	)
			return (true);
	return (false);
}

std::string	getStatusCodeName(int code)
{
	switch (code)
	{
		case OK:
			return ("Ok");
		case FOUND:
			return ("Found");
		case CREATED:
			return ("Created");
		case ACCEPTED:
			return ("Accepted");
		case CONTINUE:
			return ("Continue");
		case NOT_FOUND:
			return ("Not Found");
		case FORBIDDEN:
			return ("Forbidden");
		case SEE_OTHER:
			return ("See Other");
		case BAD_REQUEST:
			return ("Bad Request");
		case NOT_MODIFIED:
			return ("Not Modified");
		case UNAUTHORIZED:
			return ("Unauthorized");
		case REQUEST_TIMEOUT:
			return ("Request Timeout");
		case NOT_ACCEPTABLE:
			return ("Not Acceptable");
		case GATEWAY_TIMEOUT:	
			return ("Gateway Timeout");
		case NOT_IMPLEMENTED:
			return ("Not Implemented");
		case MULTIPLE_CHOICES:
			return ("Multiple Choices");
		case MOVED_PERMANENTLY:
			return ("Moved Permanently");
		case CONTENT_TOO_LARGE:
			return ("Content Too Large");
		case METHOD_NOT_ALLOWED:
			return ("Method Not Allowed");
		case TEMPORARY_REDIRECT:
			return ("Temporary Redirect");
		case PERMANENT_REDIRECT:
			return ("Permanent Redirect");
		case INTERNAL_SERVER_ERROR:
			return ("Internal Server Error");
		case UNSUPPORTED_MEDIA_TYPE:
			return ("Unsupported Media Type");
		case HTTP_VERSION_NOT_SUPPORTED:
			return ("HTTP Version Not Supported");
	};
	return ("NOT FOUND");
}
