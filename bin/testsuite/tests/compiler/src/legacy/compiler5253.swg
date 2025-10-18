#global testerror "Err0029"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}
