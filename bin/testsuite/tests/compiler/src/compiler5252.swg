//#global testerror "Err0611"
#test
{
    var x = 5
    var y = #constref &x
    y = #constref &x
}