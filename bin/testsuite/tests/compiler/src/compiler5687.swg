#global testerror "Err0288"

#test
{
    var g = 66
    switch g
    {
    case let ptr1: 0 where ptr1 == 66:
        g = 5
    }
}
