#global testerror "Err0481"

var G = 0
#test
{
    if false do
        G = 1
    else
        G = 2
}