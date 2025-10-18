#global testerror "Err0319"

#run
{
    var s = 1
    switch s
    {
    case 1:
        fallthrough
    }
}
