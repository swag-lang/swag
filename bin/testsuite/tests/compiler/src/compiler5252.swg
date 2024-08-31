//#global testerror "Err0622"
#test
{
    var x = 5
    var y = #constref &x
    y = #constref &x
}