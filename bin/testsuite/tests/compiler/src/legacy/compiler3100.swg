//#global testerror

func msg() => "msg"

#test
{
    var x = 1
    #if false: #error(#run msg())
}