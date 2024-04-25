

#include "nds.h" 



#define f32 float
#define u32 uint32_t
#define s32 int32_t
#define u64 uint64_t



#ifdef TARGET_NDS
#define NDS_ITCM_CODE __attribute__((section(".itcm")))
#else
#define NDS_ITCM_CODE
#endif


//Hardware accelerated floating point operations on the nds, handling of invalid floating point values is not implemented
// use -Wl,--use-blx,--wrap="__aeabi_fmul",--wrap="__aeabi_fdiv",--wrap="sqrtf" -u sqrtf -u __aeabi_fmul -u __aeabi_fdiv in the linker
// flags to make the linker replace existing operations with them, or call them directly.


NDS_ITCM_CODE f32 __wrap___aeabi_fdiv(f32 x, f32 y ) {
    union { u32 i; f32 f;}xu;
    xu.f=x;
    union {u32 j;f32 f;}yu;
    yu.f=y;
    u64 i=((xu.i & ((1<<23)-1))|(1<<23));    
    u32 j=((yu.j & ((1<<23)-1))|(1<<23));
    REG_DIVCNT = DIV_64_32; 
    REG_DIV_NUMER = (i<<23);
    REG_DIV_DENOM_L = j;
    s16 shift= i<j ? -1 : 0; 
    u64 m=(xu.i) & (0xff<<23);
    u64 n=(yu.j) & (0xff<<23);
    s32 exponent=m-n+(shift<<23)+(127<<23);
    u32 a =xu.i & (1<<31);//get sign of x
    u32 b =yu.j & (1<<31);//get sign of y
    u32 sign=a^b;
    if ((exponent<=(0<<23))){
	while(REG_DIVCNT & DIV_BUSY);
    return 0.0;
    } else{
	while(REG_DIVCNT & DIV_BUSY);
	u32 mantissa=REG_DIV_RESULT_L;
	mantissa=shift>=0 ? mantissa>>(shift) : mantissa<<-shift;
	mantissa=mantissa & ((1<<23)-1);
	xu.i= sign | exponent | mantissa;
	return xu.f;
    }
}


NDS_ITCM_CODE f32 __wrap_sqrtf(f32 x){
    union{f32 f; u32 i;}xu;
    xu.f=x;
    u64 mantissa=xu.i & ((1<<23)-1);
    //check if exponent is odd
    //before subtracting 127 exponent was even if it is odd now
    //therefore check if last digit is 0 
    mantissa=(mantissa+(1<<23))<<   ( (( xu.i & (1<<23))==0  )      +23 );
    REG_SQRTCNT = SQRT_64;
    REG_SQRT_PARAM = mantissa;
    u32 raw_exponent= (xu.i & (0xff<<23));
    if (raw_exponent > 0 ) {
	s32 exponent=raw_exponent-(127<<23);
	exponent=exponent>>1; //right shift on negative number depends on compiler
	exponent=((exponent+(127<<23))& (0xff<<23) );
    //fetch async result here
	while(REG_SQRTCNT & SQRT_BUSY);
	xu.i=  exponent| (REG_SQRT_RESULT & ((1<<23)-1));
	return xu.f;
    } else{
	while(REG_SQRTCNT & SQRT_BUSY);
	return 0.0;
    };

}


NDS_ITCM_CODE f32 __wrap___aeabi_fmul( f32 x, f32 y){
    union {u32 i;f32 f;}xu;
    xu.f=x;
    union {u32 j;f32 f;}yu;
    yu.f=y;
    u32 a =xu.i & (1<<31);//get sign of x
    u32 b =yu.j & (1<<31);//get sign of y
    s32 exponentx=(xu.i ) &( 0xff<<23);
    s32 exponenty=(yu.j) &(0xff<<23);
    if(exponentx==0 || exponenty==0) {
	return 0.0;
    }
    s32 combined_exponent =(exponentx- (127<<23)) +exponenty;
    u32 i=(xu.i & 0x7fffff); //should be explicit bits of mantissa
    u32 j=(yu.j & 0x7fffff);
    u32 mantissa=i+j+(((u64)(i<<4)*(u64)(j<<5))>>32);//this needs to be compiled as ARM code, otherwise the performance sucks
    mantissa =mantissa +  (1<< 23) ;
    s32 clz=__builtin_clz(mantissa);
    s32 shift=8-clz;
    mantissa= shift>=0 ? mantissa>>shift: mantissa<<-shift;
    mantissa= mantissa & ((1<<23)-1);
    u32 exponent=(combined_exponent+(shift<<23));
    u32 sign= a^b;
    union{
    f32 f;
    u32 k;
    }result;
    result.k= sign |exponent | mantissa;
    return combined_exponent<=(0<<23)? 0.0:result.f ;
}
