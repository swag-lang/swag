#global testerror "Err0771"

#test
{
    var g = 66
    const aa = 0
    switch g
    {
    case aa var ptr1 where ptr1 == 66:
        g = 5
    }
}
