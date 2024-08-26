#global testerror "Err0772"

#test
{
    var g = 66
    switch g
    {
    case 0 var ptr1 where ptr1 == 66:
        g = 5
    }
}
