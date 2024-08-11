#global testerror "Err0598"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}