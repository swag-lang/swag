#global testerror "Err0424"

var G = 0
#test
{
    if false:
        G = 1
    else
        G = 2
}