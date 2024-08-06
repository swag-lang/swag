#global testerror "Err0403"

#run
{
    var s = 1
    switch s
    {
        case 1:
            fallthrough
//        case 2:
//            @print("toto")
    }
}