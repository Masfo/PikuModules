#pragma once
#ifndef _DEBUG
//#    pragma warning(disable : 4668)   // is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
//#    pragma warning(disable : 4464)   //  relative include path contains '..'
#endif

#pragma warning(disable : 4820)   // '4' bytes padding added after data member
#pragma warning(disable : 4514)   // unreferenced inline function has been removed
#pragma warning(disable : 4324)   // structure was padded due to alignment specifier
#pragma warning(disable : 4710)   // <function> function not inlined
#pragma warning(disable : 4711)   //  <function> selected for automatic inline expansion

#pragma warning(disable : 5246)   //  the initialization of a subobject should be wrapped in braces

#pragma warning(disable : 5246)   //  the initialization of a subobject should be wrapped in braces

#pragma warning(disable : 5039)   // pointer or reference to potentially throwing function passed
                                  // to 'extern "C"' function under -EHc. Undefined behavior may occur if this
                                  // function throws an exception.
#pragma warning(disable : 5045)   // Compiler will insert Spectre mitigation for memory load if /Qspectre
                                  // switch specified


//#pragma warning(disable : 5027)  // move assignment operator was implicitly defined as deleted
//#pragma warning(disable : 4505)  // unreferenced local function has been removed
//#pragma warning(disable : 4625)  // copy constructor was implicitly defined as deleted
//#pragma warning(disable : 4626)  // move constructor was implicitly defined as deleted
//#pragma warning(disable : 5026)  // move constructor was implicitly defined as deleted
