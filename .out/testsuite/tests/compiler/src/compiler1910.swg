
#global testerror

//#[Swag.printbc]
#test
{
    x := 666
    val := @mkany(&x, f32)

    value := cast(f32) val
}