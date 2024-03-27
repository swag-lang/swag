#global testerror "Err0382"

#test
{
    var x: struct{v: f32}
    x[0] = x
}
