#global testerror "Err0632"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}