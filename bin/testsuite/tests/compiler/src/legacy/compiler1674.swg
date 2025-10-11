#global testerror "Err0327"

#run
{
    var s = 1
    switch s
    {
    case 1:
        fallthrough
    }
}
