#pragma once

namespace BGE
{
	class BGEException
	{
	public:
		char * what;
		BGEException(char * what);
		~BGEException(void);
		const char * What();

	};
}
