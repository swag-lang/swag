#global testerror "Err0539"

//#[Swag.PrintBc]
#test
{
    var x   = 666
    var val = @mkany(&x, f32)

    var value = cast(f32) val
}
