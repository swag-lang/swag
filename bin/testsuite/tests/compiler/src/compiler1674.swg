#global testerror "Err0317"

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