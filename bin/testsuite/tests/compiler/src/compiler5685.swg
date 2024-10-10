#global skip
#global testerror "Err0401"

#test
{
    var g = 66
    const aa = 0
    switch g
    {
    case let ptr1 as aa where ptr1 == 66:
        g = 5
    }
}
