#global testerror "Err0264"

#test
{
    var g = 66
    switch g
    {
    case 0 as ptr1 where ptr1 == 66:
        g = 5
    }
}
