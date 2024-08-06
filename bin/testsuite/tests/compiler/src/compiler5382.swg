#global testerror "Err0483"

var G = 0
#test
{
    if false do
        G = 1
    else
        G = 2
}