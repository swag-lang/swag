#global testerror "Err0637"
struct Arr {}

func(T) toto(v: [..] T)
{
}

#test
{
    var arr: Arr
    toto(arr)
}