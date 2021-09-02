//#global testerror

func msg() => "msg"

#test
{
    x := 1
    #assert(true, #run msg())
}