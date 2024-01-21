
#global testerror "Err0104"

//#[Swag.PrintBc]
#test
{
    var x = "64"
    var y = &x[0]
    y[0] = 0
}