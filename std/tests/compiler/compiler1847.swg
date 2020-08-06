//#[swag.printbc]
func test(x: f64)
{
    @assert(x > -1 && x < 222)
    //@print(5100's64)
}

#test
{
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

    test(cast(f64) @abs(-5.4'f32))
    test(cast(f64) @abs(-5.4'f64))
    test(cast(f64) @abs(-5's8))
    test(cast(f64) @abs(-5's16))
    test(cast(f64) @abs(-5's32))
    test(cast(f64) @abs(-5's64))

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