#ifndef FLOATHEADER_H
#define FLOATHEADER_H

#if defined(__cplusplus)
extern "C" {                 // Make sure we have C-declarations in C++ programs
#endif

extern float __wrap___aeabi_fmul( float x, float y);
extern float __real___aeabi_fmul( float x, float y);
extern float __aeabi_fmul( float x, float y);

extern float __wrap___aeabi_fdiv( float x, float y);
extern float __real___aeabi_fdiv( float x, float y);
extern float __aeabi_fdiv( float x, float y);


extern float __wrap_sqrtf( float x);
extern float __real_sqrtf( float x);
extern float sqrtf( float x);

//#define sqrtf __wrap_sqrtf

#if defined(__cplusplus)
}
#endif

#endif

