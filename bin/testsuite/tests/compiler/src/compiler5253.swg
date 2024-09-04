#global testerror "Err0566"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}