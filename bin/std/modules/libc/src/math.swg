#global export
using Swag

alias float_t  = f32
alias double_t = f64

const NAN       = cast #bit (f64) ~0'u64
const INFINITY  = 1e5000'f32
const HUGE_VALF = INFINITY
const HUGE_VAL  = cast(f64) INFINITY

const MATH_ERRNO     = 1
const MATH_ERREXCEPT = 2

const FP_ILOGBNAN = -1 - cast(s32) (~0'u32 >> 1)
const FP_ILOGB0   = FP_ILOGBNAN

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
    func frexp(value: f64, exp: *s64)->f64;
    func frexpf(value: f32, exp: *s64)->f32;
    func ilogb(x: f64)->s64;
    func ilogbf(x: f32)->s64;
    func ldexp(x: f64, exp: s64)->f64;
    func ldexpf(x: f32, exp: s64)->f32;
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
    func scalbn(x: f64, n: s64)->f64;
    func scalbnf(x: f32, n: s64)->f32;
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
    func remquo(x: f64, y: f64, quo: *s64)->f64;
    func remquof(x: f32, y: f32, quo: *s64)->f32;

    func copysign(x: f64, y: f64)->f64;
    func copysignf(x: f32, y: f32)->f32;
    func nan(tagp: cstring)->f64;
    func nanf(tagp: cstring)->f32;
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

const FP_NAN       = 0
const FP_INFINITE  = 1
const FP_ZERO      = 2
const FP_NORMAL    = 3
const FP_SUBNORMAL = 4

#[Inline]
func fpclassify(x: f64)->s32
{
    let u = cast #bit (u64) x
    let e = (u >> 52) & 0x7ff
    switch e
    {
    case 0:
        return (u << 1) ? FP_SUBNORMAL : FP_ZERO
    case 0x7ff:
        return (u << 12) ? FP_NAN : FP_INFINITE
    }

    return FP_NORMAL
}

#[Inline]
func fpclassifyf(x: f32)->s32
{
    let u = cast #bit (u32) x
    let e = (u >> 23) & 0xff
    switch e
    {
    case 0:
        return (u << 1) ? FP_SUBNORMAL : FP_ZERO
    case 0x7ff:
        return (u << 9) ? FP_NAN : FP_INFINITE
    }

    return FP_NORMAL
}

#[Inline, Overload]
{
    func signbit(x: f32) => cast(s32) (cast #bit (u32) x >> 31)
    func signbit(x: f64) => cast(s32) (cast #bit (u64) x >> 63)

    func isunordered(x, y: f32) => isnan(x) or isnan(y)
    func isunordered(x, y: f64) => isnan(x) or isnan(y)

    func isfinite(x: f32) => fpclassifyf(x) == FP_INFINITE
    func isfinite(x: f64) => fpclassify(x) == FP_INFINITE
    func isinf(x: f32)    => fpclassifyf(x) > FP_INFINITE
    func isinf(x: f64)    => fpclassify(x) > FP_INFINITE
    func isnan(x: f32)    => fpclassifyf(x) == FP_NAN
    func isnan(x: f64)    => fpclassify(x) == FP_NAN
    func isnormal(x: f32) => fpclassifyf(x) == FP_NORMAL
    func isnormal(x: f64) => fpclassify(x) == FP_NORMAL

    func isgreater(x, y: f32)      => !isunordered(x, y) and x > y
    func isgreater(x, y: f64)      => !isunordered(x, y) and x > y
    func isgreaterequal(x, y: f32) => !isunordered(x, y) and x >= y
    func isgreaterequal(x, y: f64) => !isunordered(x, y) and x >= y
    func isless(x, y: f32)         => !isunordered(x, y) and x < y
    func isless(x, y: f64)         => !isunordered(x, y) and x < y
    func islessequal(x, y: f32)    => !isunordered(x, y) and x <= y
    func islessequal(x, y: f64)    => !isunordered(x, y) and x <= y
    func islessgreater(x, y: f32)  => !isunordered(x, y) and (x < y or x > y)
    func islessgreater(x, y: f64)  => !isunordered(x, y) and (x < y or x > y)
}
