#global testerror "Err0025"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}
