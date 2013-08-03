#include "BGEException.h"

using namespace BGE;

BGEException::BGEException(char * what)
{
	this->what = what;
}

const char * BGEException::What()
{
	return what;
}


BGEException::~BGEException(void)
{
}
