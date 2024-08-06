#global testerror "Err0640"
#test
{
    var x = 5
    var y =,ref &x
    y =,constref &x
}