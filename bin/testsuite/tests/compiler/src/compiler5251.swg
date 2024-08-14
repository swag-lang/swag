#global testerror "Err0600"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}