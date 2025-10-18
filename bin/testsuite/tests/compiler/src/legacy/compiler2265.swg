#global testerror "Err0068"

#test
{
    var x: struct{ v: f32 }
    x[0] = x
}
