
#ifndef Max
#define Max(a,b) (a > b ? a : b)
#endif

#ifndef Min
#define Min(a,b) (a < b ? a : b)
#endif

#ifndef Clamp
#define Clamp(x, a, b) Max(a, Min(b, x))
#endif

#ifndef Saturate
#define Saturate(x) Max(0, Min(1, x))
#endif

#ifndef FloatAbout
#define FloatAbout(value, target, deviation) (value > target - deviation && value < target + deviation
#endif

#ifndef Remap
#define Remap(In, InMin, InMax, OutMin, OutMax) (OutMin + (In - InMin) * (OutMax - OutMin) / (InMax - InMin));
#endif

#ifndef Remap01
#define Remap01(In, InMin, InMax) ((In - InMin) / (InMax - InMin));
#endif