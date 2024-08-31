#global testerror "Err0622"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}