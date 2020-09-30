namespace std.math;

public namespace constF64
{
    const Pi: f64           = 3.1415927410125732421875
    const E: f64            = 2.71828174591064453125
    const TwoPi: f64        = 2 * Pi
    const PiBy2: f64        = Pi / 2
    const PiBy3: f64        = Pi / 3
    const PiBy4: f64        = Pi / 4
    const PiBy6: f64        = Pi / 6
    const PiBy8: f64        = Pi / 8
    const OneByPi: f64      = 1 / Pi
    const TwoByPi: f64      = 2 / Pi
    const Sqrt2: f64        = sqrt(2'f64)
    const OneBySqrt2: f64   = 1 / Sqrt2
    const Ln2 : f64         = log(2'f64)
    const Ln10 : f64        = log(10'f64)
    const Log2E : f64       = log2(E)
    const Log10E : f64      = log10(E)
    const Epsilon: f64      = 1e-06'f64
}

public namespace constF32
{
    const Pi            = cast(f32) constF64.Pi
    const E             = cast(f32) constF64.E
    const TwoPi         = cast(f32) constF64.TwoPi
    const PiBy2         = cast(f32) constF64.PiBy2
    const PiBy3         = cast(f32) constF64.PiBy3
    const PiBy4         = cast(f32) constF64.PiBy4
    const PiBy6         = cast(f32) constF64.PiBy6
    const PiBy8         = cast(f32) constF64.PiBy8
    const OneByPi       = cast(f32) constF64.OneByPi
    const TwoByPi       = cast(f32) constF64.TwoByPi
    const Sqrt2         = cast(f32) constF64.Sqrt2
    const OneBySqrt2    = cast(f32) constF64.OneBySqrt2
    const Ln2           = cast(f32) constF64.Ln2
    const Ln10          = cast(f32) constF64.Ln10
    const Log2E         = cast(f32) constF64.Log2E
    const Log10E        = cast(f32) constF64.Log10E
    const Epsilon       = cast(f32) constF64.Epsilon
}

#[swag.constexpr]
public
{
    func saturate(x: f32) => clamp(x, 0, 1)
    func saturate(x: f64) => clamp(x, 0, 1)
    func lerp(x, y, factor: f32) => ((1 - factor) * x) + (factor * y)
    func lerp(x, y, factor: f64) => ((1 - factor) * x) + (factor * y)
    func toDegrees(x: f32) => x * 57.295779513082320876798154814105'f32
    func toRadians(x: f32) => x * 0.017453292519943295769236907684886'f32
    func toDegrees(x: f64) => x * (180'f64 / constF64.Pi)
    func toRadians(x: f64) => x * (constF64.Pi / 180'f64)
    func isZeroEpsilon(x: f32, eps: f32 = constF32.Epsilon) => abs(x) < eps
    func isZeroEpsilon(x: f64, eps: f64 = constF64.Epsilon) => abs(x) < eps
    func isEqualEpsilon(x, y: f32, eps: f32 = constF32.Epsilon) => abs(x - y) < eps
    func isEqualEpsilon(x, y: f64, eps: f64 = constF64.Epsilon) => abs(x - y) < eps
}

#[swag.constexpr, swag.inline]
public
{
    func sqrt(x: f32)->f32 => @sqrt(x)
    func sqrt(x: f64)->f64 => @sqrt(x)

    func sin(x: f32)->f32 => @sin(x)
    func sin(x: f64)->f64 => @sin(x)
    func cos(x: f32)->f32 => @cos(x)
    func cos(x: f64)->f64 => @cos(x)
    func tan(x: f32)->f32 => @tan(x)
    func tan(x: f64)->f64 => @tan(x)

    func sinh(x: f32)->f32 => @sinh(x)
    func sinh(x: f64)->f64 => @sinh(x)
    func cosh(x: f32)->f32 => @cosh(x)
    func cosh(x: f64)->f64 => @cosh(x)
    func tanh(x: f32)->f32 => @tanh(x)
    func tanh(x: f64)->f64 => @tanh(x)

    func asin(x: f32)->f32 => @asin(x)
    func asin(x: f64)->f64 => @asin(x)
    func acos(x: f32)->f32 => @acos(x)
    func acos(x: f64)->f64 => @acos(x)
    func atan(x: f32)->f32 => @atan(x)
    func atan(x: f64)->f64 => @atan(x)

    func log(x: f32)->f32 => @log(x)
    func log(x: f64)->f64 => @log(x)
    func log2(x: f32)->f32 => @log2(x)
    func log2(x: f64)->f64 => @log2(x)
    func log10(x: f32)->f32 => @log10(x)
    func log10(x: f64)->f64 => @log10(x)

    func floor(x: f32)->f32 => @floor(x)
    func floor(x: f64)->f64 => @floor(x)
    func ceil(x: f32)->f32 => @ceil(x)
    func ceil(x: f64)->f64 => @ceil(x)
    func trunc(x: f32)->f32 => @trunc(x)
    func trunc(x: f64)->f64 => @trunc(x)
    func round(x: f32)->f32 => @round(x)
    func round(x: f64)->f64 => @round(x)

    func abs(x: f32)->f32 => @abs(x)
    func abs(x: f64)->f64 => @abs(x)

    func exp(x: f32)->f32 => @exp(x)
    func exp(x: f64)->f64 => @exp(x)
    func exp2(x: f32)->f32 => @exp2(x)
    func exp2(x: f64)->f64 => @exp2(x)
    func pow(x, y: f32)->f32 => @pow(x, y)
    func pow(x, y: f64)->f64 => @pow(x, y)
}