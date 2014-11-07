#include "llyHttpErrorCode.h"

const char* lly::getErrorString( int e )
{
	switch (e)
	{
	case HTTP_ERROR_OK:
		return "OK";
	case HTTP_ERROR_EMPTY:
		return "EMPTY";
	case HTTP_ERROR_CHANGE_SERVER:
		return "CHANGE_SERVER";
	case HTTP_ERROR_UNKNOWN:
		return "UNKNOWN";
	case HTTP_ERROR_CONNECT_FAIL:
		return "CONNECT_FAIL";
	case HTTP_ERROR_TIMEOUT:
		return "TIMEOUT";
	case HTTP_ERROR_SEND_FAIL:
		return "SEND_FAIL";
	case HTTP_ERROR_RECV_FAIL:
		return "RECV_FAIL";
	default:
		return "unknown error";
	}
}



