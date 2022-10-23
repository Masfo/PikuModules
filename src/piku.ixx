module;
#if (_MSC_VER >= 1914 && __cplusplus == 199711L)
#    if (__cplusplus >= 202302L)  
	#warning "Warning: Consider adding /Zc:__cplusplus compiler option to you build for better version checking"
#else
	#pragma warning("Warning: Consider adding /Zc:__cplusplus compiler option to you build for better version checking")
#endif
#endif
export module piku;


export import piku.types;
export import piku.assert;
export import piku.win32;
export import piku.debug;


export import hash.fnv;
export import hash.sha2;
export import hash.blake2b;


export import piku.file;
