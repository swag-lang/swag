#global testerror "Err0069"

#test
{
    var x: struct{ v: f32 }
    x[0] = x
}
