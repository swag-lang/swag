#global testerror "Err0609"
struct Arr {}

func(T) toto(v: [..] T)
{
}

#test
{
    var arr: Arr
    toto(arr)
}