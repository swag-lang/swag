#global testerror "Err0578"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}
