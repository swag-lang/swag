#global testerror "Err0647"
struct Arr {}

func(T) toto(v: [..] T)
{
}

#test
{
    var arr: Arr
    toto(arr)
}