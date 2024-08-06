#global testerror "Err0638"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}