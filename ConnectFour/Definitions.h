#pragma once
#ifndef DEFINITIONS_H
#define DEFINITIONS_H


#include "boost/thread.hpp"


typedef unsigned long ULong;
typedef unsigned int UInt;
typedef unsigned short UShort;
typedef unsigned char UChar;
typedef signed char SChar;

typedef UChar Byte;
typedef UShort Word;
typedef UInt DWord;
typedef ULong QWord;

namespace Constants
{
	constexpr double Pi = 3.1415926535897932385;
	constexpr long double GravitationalConstant = 0.0000000000667428;
}




#define REPEAT(n) for (int Index=0; Index<n; ++Index) 
#define FOREACH(A,B) for each(auto A in B)
#define FORN(A) for (int Index = 0; Index < A.size() ;Index++) {auto Element = A[Index];
#define NEXT }

#define PASS std::cout << ""; 

#define BREAK_IF_EMPTY(Container) if (Container.empty()) break
#define RETURN_IF_EMPTY(Container) if (Container.empty()) return


#define RETURN_NULLPTR return nullptr
#define RETURN_NULLPTR_IF(Condition) if (Condition) return nullptr
#define RETURN_IF(Condition) if (Condition) return
#define RETURN_IF_NULLPTR(Condition) if (Condition == nullptr) return
#define RETURN_NULLPTR_IF_NULLPTR(Condition) if (Condition == nullptr) return nullptr

#define BREAK_IF(Condition) if (Condition) break
#define CONTINUE_IF(Condition) if (Condition) continue


#define DELETE_IF_EXISTING(Ptr) if(Ptr) delete Ptr
#define DELETE_AND_RESET_PTR(Ptr) if(Ptr) {delete Ptr; Ptr = nullptr;}


#define THREAD_SLEEP_MS(Millisec) boost::this_thread::sleep(boost::posix_time::millisec(Millisec));

#define TIME_NS_TO_MS(Nanosec) (Nanosec/1000000.0)
#define TIME_S_TO_MS(Sec) (Sec * 1000)
#define TIME_NS_TO_S(Nanosec) (Nanosec/1000000000.0)

using namespace std;

typedef boost::unique_lock<boost::shared_mutex> AWriteLock;
typedef boost::shared_lock<boost::shared_mutex> AReadLock;

#define SPACE << "  " <<
#define TAB << "\t" <<
#define END << std::endl
#define PRINT std::cout << 

#define PRINTPOINT(Point) ((Point).x) << ";" << ((Point).y)


#define ALWAYS_FALSE false && 
#define ALWAYS_TRUE true ||  


#define STD_CONTAINS(Container, Element) (Container.find(Element) != Container.end())


#define STD_STACK_DELETE_CLEAR(Container) while(!Container.empty()) {delete Container.top(); Container.pop();}
#define STD_QUEUE_DELETE_CLEAR(Container) STD_STACK_DELETE_CLEAR(Container)


#define STD_STACK_CLEAR(Container) while(!Container.empty()) {Container.pop();}
#define STD_QUEUE_CLEAR(Container) STD_STACK_CLEAR(Container)

#define FLAGINDEX(Index) (1 << (Index))
#define BIT_MASK(Bit) (1 << (Bit))



#define UCHAR_TO_SCHAR(Val) (SChar(short(Val)-128))
#define SCHAR_TO_UCHAR(Val) (UChar(short(Val)+128))


#endif