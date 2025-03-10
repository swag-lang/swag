#global public
#global namespace Math

// Structure containing mathematical constants of type 'T'
struct(T) Const
{
    const Pi:             T = 3.1415927410125732421875     // Pi constant
    const E:              T = 2.71828174591064453125       // Euler's number
    const TwoPi:          T = 2 * Pi                       // 2 times Pi
    const PiBy2:          T = Pi / 2                       // Pi divided by 2
    const PiBy3:          T = Pi / 3                       // Pi divided by 3
    const PiBy4:          T = Pi / 4                       // Pi divided by 4
    const PiBy6:          T = Pi / 6                       // Pi divided by 6
    const PiBy8:          T = Pi / 8                       // Pi divided by 8
    const ThreePiBy4:     T = 3 * Pi / 4                   // 3 times Pi divided by 4
    const OneByPi:        T = 1 / Pi                       // Reciprocal of Pi
    const TwoByPi:        T = 2 / Pi                       // 2 divided by Pi
    const Sqrt2:          T = sqrt(cast(T) 2)              // Square root of 2
    const OneBySqrt2:     T = 1 / Sqrt2                    // Reciprocal of square root of 2
    const Ln2:            T = log(cast(T) 2)               // Natural logarithm of 2
    const Ln10:           T = log(cast(T) 10)              // Natural logarithm of 10
    const Log2E:          T = log2(E)                      // Log base 2 of Euler's number
    const Log10E:         T = log10(E)                     // Log base 10 of Euler's number
    const Epsilon:        T = 1e-06                        // Smallest value for floating-point precision comparisons
}

// Aliases for 32-bit and 64-bit floating-point constants
alias ConstF32 = Const'f32
alias ConstF64 = Const'f64

#[Swag.ConstExpr, Swag.Inline, Swag.Overload]
{
    // Returns the sign of 'x' (-1 for negative, 1 for positive)
    func sign(x: f32) => x < 0 ? -1 : 1
    func sign(x: f64) => x < 0 ? -1 : 1

    // Clamps 'x' between 0 and 1
    func saturate(x: f32) => clamp(x, 0'f32, 1'f32)
    func saturate(x: f64) => clamp(x, 0'f64, 1'f64)

    // Linear interpolation between 'x' and 'y' based on 'factor'
    func lerp(x, y, factor: f32) => x + factor * (y - x)
    func lerp(x, y, factor: f64) => x + factor * (y - x)

    // Cubic interpolation between 'a', 'b', 'c', and 'd' based on 't'
    func cubicLerp(a, b, c, d, t: f32)->f32 { let p = (d - c) - (a - b); return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b; }
    func cubicLerp(a, b, c, d, t: f64)->f64 { let p = (d - c) - (a - b); return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b; }

    // Hermite interpolation function
    func interpHermite(t: f32) => t * t * (3 - 2 * t)
    func interpHermite(t: f64) => t * t * (3 - 2 * t)

    // Quintic interpolation function
    func interpQuintic(t: f32) => t * t * t * (t * (t * 6 - 15) + 10)
    func interpQuintic(t: f64) => t * t * t * (t * (t * 6 - 15) + 10)

    // Convert radians to degrees
    func toDegrees(x: f32) => x * 57.295779513082320876798154814105'f32
    func toRadians(x: f32) => x * 0.017453292519943295769236907684886'f32
    func toDegrees(x: f64) => x * (180'f64 / ConstF64.Pi)
    func toRadians(x: f64) => x * (ConstF64.Pi / 180'f64)

    // Check if 'x' is approximately zero within an epsilon threshold
    func isZeroEpsilon(x: f32, eps = ConstF32.Epsilon) => abs(x) < eps
    func isZeroEpsilon(x: f64, eps = ConstF64.Epsilon) => abs(x) < eps

    // Check if 'x' and 'y' are approximately equal within an epsilon threshold
    func isEqualEpsilon(x, y: f32, eps = ConstF32.Epsilon) => abs(x - y) < eps
    func isEqualEpsilon(x, y: f64, eps = ConstF64.Epsilon) => abs(x - y) < eps

    // Check if 'x' is NaN (not a number)
    #[Swag.Safety("nan", false)]
    func isNan(x: f32) => x != x
    #[Swag.Safety("nan", false)]
    func isNan(x: f64) => x != x

    // Smoothstep interpolation between 'edge0' and 'edge1'
    func smoothstep(edge0, edge1, x: f32)->f32
    {
        let t = clamp((x - edge0) / (edge1 - edge0), 0'f32, 1'f32)
        return t * t * (3 - 2 * t)
    }

    func smoothstep(edge0, edge1, x: f64)->f64
    {
        let t = clamp((x - edge0) / (edge1 - edge0), 0'f64, 1'f64)
        return t * t * (3 - 2 * t)
    }

    // Maps 'val' from the range [minSrc, maxSrc] to [minDst, maxDst]
    func map(val, minSrc, maxSrc, minDst, maxDst: f32)->f32
    {
        let r = (val - minSrc) / (maxSrc - minSrc)
        return minDst + r * (maxDst - minDst)
    }

    func map(val, minSrc, maxSrc, minDst, maxDst: f64)->f64
    {
        let r = (val - minSrc) / (maxSrc - minSrc)
        return minDst + r * (maxDst - minDst)
    }

    // Calculates the smallest power of 10 greater than or equal to the given number val
    func powerOf10(val: f32)->s32
    {
        if val == 0:
            return 1
        return cast() pow(10, floor(log10(val)) + 1)
    }

    func powerOf10(val: f64)->s32
    {
        if val == 0:
            return 1
        return cast() pow(10, floor(log10(val)) + 1)
    }
}

#[Swag.ConstExpr, Swag.Inline, Swag.Overload]
{
    func atan2(x, y: f32) => @atan2(x, y)
    func atan2(x, y: f64) => @atan2(x, y)

    func sqrt(x: f32) => @sqrt(x)
    func sqrt(x: f64) => @sqrt(x)

    func sin(x: f32) => @sin(x)
    func sin(x: f64) => @sin(x)
    func cos(x: f32) => @cos(x)
    func cos(x: f64) => @cos(x)
    func tan(x: f32) => @tan(x)
    func tan(x: f64) => @tan(x)

    func sinh(x: f32) => @sinh(x)
    func sinh(x: f64) => @sinh(x)
    func cosh(x: f32) => @cosh(x)
    func cosh(x: f64) => @cosh(x)
    func tanh(x: f32) => @tanh(x)
    func tanh(x: f64) => @tanh(x)

    func asin(x: f32) => @asin(x)
    func asin(x: f64) => @asin(x)
    func acos(x: f32) => @acos(x)
    func acos(x: f64) => @acos(x)
    func atan(x: f32) => @atan(x)
    func atan(x: f64) => @atan(x)

    func log(x: f32)   => @log(x)
    func log(x: f64)   => @log(x)
    func log2(x: f32)  => @log2(x)
    func log2(x: f64)  => @log2(x)
    func log10(x: f32) => @log10(x)
    func log10(x: f64) => @log10(x)

    func floor(x: f32) => @floor(x)
    func floor(x: f64) => @floor(x)
    func ceil(x: f32)  => @ceil(x)
    func ceil(x: f64)  => @ceil(x)
    func trunc(x: f32) => @trunc(x)
    func trunc(x: f64) => @trunc(x)
    func round(x: f32) => @round(x)
    func round(x: f64) => @round(x)

    func abs(x: f32) => @abs(x)
    func abs(x: f64) => @abs(x)

    func exp(x: f32)    => @exp(x)
    func exp(x: f64)    => @exp(x)
    func exp2(x: f32)   => @exp2(x)
    func exp2(x: f64)   => @exp2(x)
    func pow(x, y: f32) => @pow(x, y)
    func pow(x, y: f64) => @pow(x, y)

    func mulAdd(x, y, z: f32) => @muladd(x, y, z)
    func mulAdd(x, y, z: f64) => @muladd(x, y, z)
}
