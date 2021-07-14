#global public
#global namespace Math

struct(T) Const
{
    const Pi: T           = 3.1415927410125732421875
    const E: T            = 2.71828174591064453125
    const TwoPi: T        = 2 * Pi
    const PiBy2: T        = Pi / 2
    const PiBy3: T        = Pi / 3
    const PiBy4: T        = Pi / 4
    const PiBy6: T        = Pi / 6
    const PiBy8: T        = Pi / 8
    const ThreePiBy4: T   = 3 * Pi / 4
    const OneByPi: T      = 1 / Pi
    const TwoByPi: T      = 2 / Pi
    const Sqrt2: T        = sqrt(cast(T) 2)
    const OneBySqrt2: T   = 1 / Sqrt2
    const Ln2 : T         = log(cast(T) 2)
    const Ln10 : T        = log(cast(T) 10)
    const Log2E : T       = log2(E)
    const Log10E : T      = log10(E)
    const Epsilon: T      = 1e-06
}

alias ConstF32 = Const'f32
alias ConstF64 = Const'f64

#[Swag.ConstExpr, Swag.Inline]
{
    func sign(x: f32) => x < 0 ? -1 : 1
    func sign(x: f64) => x < 0 ? -1 : 1
    func saturate(x: f32) => clamp(x, 0, 1)
    func saturate(x: f64) => clamp(x, 0, 1)
    func lerp(x, y, factor: f32) => ((1 - factor) * x) + (factor * y)
    func lerp(x, y, factor: f64) => ((1 - factor) * x) + (factor * y)
    func toDegrees(x: f32) => x * 57.295779513082320876798154814105'f32
    func toRadians(x: f32) => x * 0.017453292519943295769236907684886'f32
    func toDegrees(x: f64) => x * (180'f64 / ConstF64.Pi)
    func toRadians(x: f64) => x * (ConstF64.Pi / 180'f64)
    func isZeroEpsilon(x: f32, eps = ConstF32.Epsilon) => abs(x) < eps
    func isZeroEpsilon(x: f64, eps = ConstF64.Epsilon) => abs(x) < eps
    func isEqualEpsilon(x, y: f32, eps = ConstF32.Epsilon) => abs(x - y) < eps
    func isEqualEpsilon(x, y: f64, eps = ConstF64.Epsilon) => abs(x - y) < eps
}

#[Swag.ConstExpr, Swag.Inline]
{
    func min(x, y: f32)   => @min(x, y)
    func min(x, y: f64)   => @min(x, y)
    func max(x, y: f32)   => @max(x, y)
    func max(x, y: f64)   => @max(x, y)
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

    func log(x: f32) => @log(x)
    func log(x: f64) => @log(x)
    func log2(x: f32) => @log2(x)
    func log2(x: f64) => @log2(x)
    func log10(x: f32) => @log10(x)
    func log10(x: f64) => @log10(x)

    func floor(x: f32) => @floor(x)
    func floor(x: f64) => @floor(x)
    func ceil(x: f32) => @ceil(x)
    func ceil(x: f64) => @ceil(x)
    func trunc(x: f32) => @trunc(x)
    func trunc(x: f64) => @trunc(x)
    func round(x: f32) => @round(x)
    func round(x: f64) => @round(x)

    func abs(x: f32) => @abs(x)
    func abs(x: f64) => @abs(x)

    func exp(x: f32) => @exp(x)
    func exp(x: f64) => @exp(x)
    func exp2(x: f32) => @exp2(x)
    func exp2(x: f64) => @exp2(x)
    func pow(x, y: f32) => @pow(x, y)
    func pow(x, y: f64) => @pow(x, y)
}