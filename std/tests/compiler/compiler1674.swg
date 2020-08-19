#module "compiler1674"
#unittest error

#run
{
    s := 1
    switch s
    {
        case 1:
            fallthrough
//        case 2:
//            @print("toto")
    }
}