#global testerror "Err0772"

#test
{
    var g = 66
    switch g
    {
    case var ptr1: 0 where ptr1 == 66:
        g = 5
    }
}
