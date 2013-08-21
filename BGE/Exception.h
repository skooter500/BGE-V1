#pragma once

namespace BGE
{
	class Exception
	{
	public:
		const char * what;
		Exception(const char * what);
		~Exception(void);
		const char * What();

	};
}
