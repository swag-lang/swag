#global testerror "Err0255"

#test
{
    var g = 66
    switch g
    {
    case let ptr1 as 0 where ptr1 == 66:
        g = 5
    }
}
