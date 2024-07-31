#global testerror "Err0648"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}