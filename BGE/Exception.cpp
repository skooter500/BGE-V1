#include "Exception.h"

using namespace BGE;

Exception::Exception(const char * what)
{
	this->what = what;
}

const char * Exception::What()
{
	return what;
}


Exception::~Exception(void)
{
}
