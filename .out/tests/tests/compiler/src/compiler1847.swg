//#[Swag.printbc]
func test(x: f64)
{
    //@print(x)
    //@print(" ")
    @assert(x >= -1 and x < 222)
}

//#[Swag.printbc]
#test
{
    test(cast(f64) @abs(-5's8))
    test(cast(f64) @abs(-5's16))

    test(cast(f64) @abs(-5's32))
    test(cast(f64) @abs(-5's64))

    test(cast(f64) @abs(-5.4'f32))
    test(cast(f64) @abs(-5.4'f64))
    test(cast(f64) @sqrt(5.0'f32))
    test(cast(f64) @sqrt(5.0'f64))

    test(cast(f64) @sin(5.0'f32))
    test(cast(f64) @sin(5.0'f64))
    test(cast(f64) @cos(5.0'f32))
    test(cast(f64) @cos(5.0'f64))

    test(cast(f64) @log(5.0'f32))
    test(cast(f64) @log(5.0'f64))
    test(cast(f64) @log2(5.0'f32))
    test(cast(f64) @log2(5.0'f64))
    test(cast(f64) @log10(5.0'f32))
    test(cast(f64) @log10(5.0'f64))

    test(cast(f64) @floor(5.4'f32))
    test(cast(f64) @floor(5.4'f64))
    test(cast(f64) @ceil(5.4'f32))
    test(cast(f64) @ceil(5.4'f64))
    test(cast(f64) @trunc(5.4'f32))
    test(cast(f64) @trunc(5.4'f64))
    test(cast(f64) @round(5.4'f32))
    test(cast(f64) @round(5.4'f64))

    test(cast(f64) @exp(5.4'f32))
    test(cast(f64) @exp(5.4'f64))
    test(cast(f64) @exp2(5.4'f32))
    test(cast(f64) @exp2(5.4'f64))

    test(cast(f64) @acos(0.5'f32))
    test(cast(f64) @acos(0.5'f64))
    test(cast(f64) @asin(0.5'f32))
    test(cast(f64) @asin(0.5'f64))

    test(cast(f64) @tan(0.5'f32))
    test(cast(f64) @tan(0.5'f64))
    test(cast(f64) @atan(0.5'f32))
    test(cast(f64) @atan(0.5'f64))

    test(cast(f64) @sinh(0.5'f32))
    test(cast(f64) @sinh(0.5'f64))
    test(cast(f64) @cosh(0.5'f32))
    test(cast(f64) @cosh(0.5'f64))
    test(cast(f64) @tanh(0.5'f32))
    test(cast(f64) @tanh(0.5'f64))

    test(cast(f64) @pow(2.0'f32, 4.0'f32))
    test(cast(f64) @pow(2.0'f64, 4.0'f64))
}

#test
{
    xs8 := -5's8
    xs16 := -5's16
    xs32 := -5's32
    xs64 := -5's64
    xsf32 := 0.5'f32
    xsf64 := 0.5'f64

    @assert(@abs(xs32) == 5)

    test(cast(f64) @abs(xs8))
    test(cast(f64) @abs(xs16))

    test(cast(f64) @abs(xs32))
    test(cast(f64) @abs(xs64))

    test(cast(f64) @abs(xsf32))
    test(cast(f64) @abs(xsf64))
    test(cast(f64) @sqrt(xsf32))
    test(cast(f64) @sqrt(xsf64))

    test(cast(f64) @sin(xsf32))
    test(cast(f64) @sin(xsf64))
    test(cast(f64) @cos(xsf32))
    test(cast(f64) @cos(xsf64))

    test(cast(f64) @log(xsf32))
    test(cast(f64) @log(xsf64))
    test(cast(f64) @log2(xsf32))
    test(cast(f64) @log2(xsf64))
    test(cast(f64) @log10(xsf32))
    test(cast(f64) @log10(xsf64))

    test(cast(f64) @floor(xsf32))
    test(cast(f64) @floor(xsf64))
    test(cast(f64) @ceil(xsf32))
    test(cast(f64) @ceil(xsf64))
    test(cast(f64) @trunc(xsf32))
    test(cast(f64) @trunc(xsf64))
    test(cast(f64) @round(xsf32))
    test(cast(f64) @round(xsf64))

    test(cast(f64) @exp(xsf32))
    test(cast(f64) @exp(xsf64))
    test(cast(f64) @exp2(xsf32))
    test(cast(f64) @exp2(xsf64))

    test(cast(f64) @acos(xsf32))
    test(cast(f64) @acos(xsf64))
    test(cast(f64) @asin(xsf32))
    test(cast(f64) @asin(xsf64))

    test(cast(f64) @tan(xsf32))
    test(cast(f64) @tan(xsf64))
    test(cast(f64) @atan(xsf32))
    test(cast(f64) @atan(xsf64))

    test(cast(f64) @sinh(xsf32))
    test(cast(f64) @sinh(xsf64))
    test(cast(f64) @cosh(xsf32))
    test(cast(f64) @cosh(xsf64))
    test(cast(f64) @tanh(xsf32))
    test(cast(f64) @tanh(xsf64))

    test(cast(f64) @pow(xsf32, xsf32))
    test(cast(f64) @pow(xsf64, xsf64))
}