#global testerror "Err0636"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}