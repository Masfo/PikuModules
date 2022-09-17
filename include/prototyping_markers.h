/**********************************************************************************************
 * author:  Thomas Schwarzl (hermitC)
 * website: www.blackgolem.com
 * email:   hermitc@blackgolem.com
 * date:    January 2010
 *********************************************************************************************/

#pragma once

#include <string>
#include <exception>


// stringization for line number
#if defined(_MSC_VER)
#   define PROTOTYPE_MARKER_STRINGIZE(x) PROTOTYPE_MARKER_STRINGIZE_2((x))
#   define PROTOTYPE_MARKER_STRINGIZE_2(x) PROTOTYPE_MARKER_STRINGIZE_1 x
#else
#   define PROTOTYPE_MARKER_STRINGIZE(x) PROTOTYPE_MARKER_STRINGIZE_1(x)
#endif
#define PROTOTYPE_MARKER_STRINGIZE_1(x) #x


/*
 *  Prototyping markers.
 *
 *  Usage:
 *
 *    int ReturnNonZeroAfterPrototyping()
 *    {
 *    #pragma TODO("ReturnNonZeroAfterPrototyping() is just a stub!")
 *      return 0;
 *    }
 *
 *    int Return7()
 *    {
 *    #pragma OPTIMIZE("Return7() can be faster!")
 *      int a = 1;
 *      int b = 2;
 *      int c = 4;
 *      return a + b + c;
 *    }
 *
 *    void ThrowsWhilePrototyping()
 *    {
 *    #pragma NOT_IMPL("ThrowsAsPrototype()")
 *      THROW_IMPL_MISSING("ThrowsAsPrototype()");
 *    }
 */

#if 1
#define COMPILE_TIME_WARNING(type, msg) message(__FILE__ "(" PROTOTYPE_MARKER_STRINGIZE(__LINE__) ") : " type ": " msg)

#define TODO(x)     COMPILE_TIME_WARNING("TODO", x)
#define LABEL(x)     COMPILE_TIME_WARNING("LABEL", x)
#define OPTIMIZE(x) COMPILE_TIME_WARNING("OPTIMIZE", x)
#define NOT_IMPL(x) COMPILE_TIME_WARNING("NOT IMPLEMENTED", x " is not implemented yet")
#define NOTE(x)     COMPILE_TIME_WARNING("NOTE", x)
#define HACK(x)     COMPILE_TIME_WARNING("HACK", x)
#define FIX(x)     COMPILE_TIME_WARNING("FIX", x)


#define THROW_IMPL_MISSING(x) throw std::exception((std::string("implementation missing: ") + (x)).c_str())

#else
#define COMPILE_TIME_WARNING(type, msg) message(__FILE__ "(" PROTOTYPE_MARKER_STRINGIZE(__LINE__) ") : " type ": " msg)

#define TODO(x)     
#define LABEL(x)    
#define OPTIMIZE(x) 
#define NOT_IMPL(x) 
#define NOTE(x)     
#define HACK(x)     
#define FIX(x)     


#define THROW_IMPL_MISSING(x) 
#endif