//#global testerror

func msg() => "msg"

#test
{
    x := 1
    #if false #error #run msg()
}