#global testerror "Err0639"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}