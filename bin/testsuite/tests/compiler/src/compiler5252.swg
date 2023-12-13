//#global testerror "Err0418"
#test
{
    var x = 5
    var y = const ref &x
    y = const ref &x
}