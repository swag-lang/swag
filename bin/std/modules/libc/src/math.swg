#global export
using Swag

#[Foreign(ForeignLib), Discardable]
{
	func acos(x: f64)->f64;
	func acosf(x: f32)->f32;
	func asin(x: f64)->f64;
	func asinf(x: f32)->f32;
	func atan(x: f64)->f64;
	func atanf(x: f32)->f32;
	func atan2(y: f64, x: f64)->f64;
	func atan2f(y: f32, x: f32)->f32;
	func cos(x: f64)->f64;
	func cosf(x: f32)->f32;
	func sin(x: f64)->f64;
	func sinf(x: f32)->f32;
	func tan(x: f64)->f64;
	func tanf(x: f32)->f32;

	func acosh(x: f64)->f64;
	func acoshf(x: f32)->f32;
	func asinh(x: f64)->f64;
	func asinhf(x: f32)->f32;
	func atanh(x: f64)->f64;
	func atanhf(x: f32)->f32;
	func cosh(x: f64)->f64;
	func coshf(x: f32)->f32;
	func sinh(x: f64)->f64;
	func sinhf(x: f32)->f32;
	func tanh(x: f64)->f64;
	func tanhf(x: f32)->f32;

	func exp(x: f64)->f64;
	func expf(x: f32)->f32;
	func exp2(x: f64)->f64;
	func exp2f(x: f32)->f32;
	func expm1(x: f64)->f64;
	func expm1f(x: f32)->f32;
	func frexp(value: f64, exp: *int)->f64;
	func frexpf(value: f32, exp: *int)->f32;
	func ilogb(x: f64)->int;
	func ilogbf(x: f32)->int;
	func ldexp(x: f64, exp: int)->f64;
	func ldexpf(x: f32, exp: int)->f32;
	func log(x: f64)->f64;
	func logf(x: f32)->f32;
	func log10(x: f64)->f64;
	func log10f(x: f32)->f32;
	func log1p(x: f64)->f64;
	func log1pf(x: f32)->f32;
	func log2(x: f64)->f64;
	func log2f(x: f32)->f32;
	func logb(x: f64)->f64;
	func logbf(x: f32)->f32;
	func modf(value: f64, iptr: *f64)->f64;
	func modff(value: f32, iptr: *f32)->f32;
	func scalbn(x: f64, n: int)->f64;
	func scalbnf(x: f32, n: int)->f32;
	func scalbln(x: f64, n: s32)->f64;
	func scalblnf(x: f32, n: s32)->f32;

	func cbrt(x: f64)->f64;
	func cbrtf(x: f32)->f32;
	func fabs(x: f64)->f64;
	func fabsf(x: f32)->f32;
	func hypot(x: f64, y: f64)->f64;
	func hypotf(x: f32, y: f32)->f32;
	func pow(x: f64, y: f64)->f64;
	func powf(x: f32, y: f32)->f32;
	func sqrt(x: f64)->f64;
	func sqrtf(x: f32)->f32;

	func erf(x: f64)->f64;
	func erff(x: f32)->f32;
	func erfc(x: f64)->f64;
	func erfcf(x: f32)->f32;
	func lgamma(x: f64)->f64;
	func lgammaf(x: f32)->f32;
	func tgamma(x: f64)->f64;
	func tgammaf(x: f32)->f32;

	func ceil(x: f64)->f64;
	func ceilf(x: f32)->f32;
	func floor(x: f64)->f64;
	func floorf(x: f32)->f32;
	func nearbyint(x: f64)->f64;
	func nearbyintf(x: f32)->f32;
	func rint(x: f64)->f64;
	func rintf(x: f32)->f32;
	func lrint(x: f64)->s32;
	func lrintf(x: f32)->s32;
	func llrint(x: f64)->s64;
	func llrintf(x: f32)->s64;
	func round(x: f64)->f64;
	func roundf(x: f32)->f32;
	func lround(x: f64)->s32;
	func lroundf(x: f32)->s32;
	func llround(x: f64)->s64;
	func llroundf(x: f32)->s64;
	func trunc(x: f64)->f64;
	func truncf(x: f32)->f32;

	func fmod(x: f64, y: f64)->f64;
	func fmodf(x: f32, y: f32)->f32;
	func remainder(x: f64, y: f64)->f64;
	func remainderf(x: f32, y: f32)->f32;
	func remquo(x: f64, y: f64, quo: *int)->f64;
	func remquof(x: f32, y: f32, quo: *int)->f32;

	func copysign(x: f64, y: f64)->f64;
	func copysignf(x: f32, y: f32)->f32;
	func nan(tagp: const *u8)->f64;
	func nanf(tagp: const *u8)->f32;
	func nextafter(x: f64, y: f64)->f64;
	func nextafterf(x: f32, y: f32)->f32;

	func fdim(x: f64, y: f64)->f64;
	func fdimf(x: f32, y: f32)->f32;
	func fmax(x: f64, y: f64)->f64;
	func fmaxf(x: f32, y: f32)->f32;
	func fmin(x: f64, y: f64)->f64;
	func fminf(x: f32, y: f32)->f32;
	func fma(x, y, z: f64)->f64;
	func fmaf(x, y, z: f32)->f32;
}