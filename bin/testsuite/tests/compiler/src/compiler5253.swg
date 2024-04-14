#global testerror "Err0643"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}