#global testerror "Err0461"

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