//#global testerror "Err0608"
#test
{
    var x = 5
    var y = #constref &x
    y = #constref &x
}