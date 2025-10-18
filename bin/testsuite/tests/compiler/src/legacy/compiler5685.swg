#global skip
#global testerror "Err0310"

#test
{
    var g = 66
    const aa = 0
    switch g
    {
    case aa as ptr1 where ptr1 == 66:
        g = 5
    }
}
