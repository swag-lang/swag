//#global testerror "Err0029"
#test
{
    var x = 5
    var y = #constref &x
    y = #constref &x
}
