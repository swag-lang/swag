//#global testerror "Err0578"
#test
{
    var x = 5
    var y = #constref &x
    y = #constref &x
}