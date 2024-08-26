#global testerror "Err0615"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}