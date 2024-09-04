#global testerror "Err0413"

var G = 0
#test
{
    if false:
        G = 1
    else
        G = 2
}