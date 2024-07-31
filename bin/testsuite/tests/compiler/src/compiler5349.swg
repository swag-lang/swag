#global testerror "Err0656"
struct Arr {}

func(T) toto(v: [..] T)
{
}

#test
{
    var arr: Arr
    toto(arr)
}