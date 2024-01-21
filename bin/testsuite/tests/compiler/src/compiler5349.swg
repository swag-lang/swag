#global testerror "Err0652"
struct Arr {}

func(T) toto(v: [..] T)
{
}

#test
{
    var arr: Arr
    toto(arr)
}