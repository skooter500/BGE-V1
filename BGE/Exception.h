#pragma once

namespace BGE
{
	class Exception
	{
	public:
		char * what;
		Exception(char * what);
		~Exception(void);
		const char * What();

	};
}
