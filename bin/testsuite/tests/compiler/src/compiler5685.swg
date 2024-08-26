#global testerror "Err0771"

#test
{
    var g = 66
    const aa = 0
    switch g
    {
    case var ptr1: aa where ptr1 == 66:
        g = 5
    }
}
