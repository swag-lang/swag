#global testerror "Err0197"

#run
{
    var s = 1
    switch s
    {
    case 1:
        fallthrough
    }
}
