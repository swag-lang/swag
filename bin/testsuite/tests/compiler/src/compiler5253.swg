#global testerror "Err0599"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}