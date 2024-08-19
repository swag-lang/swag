#global testerror "Err0611"
#test
{
    var x = 5
    var y = #ref &x
    y = #constref &x
}