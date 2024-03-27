#global testerror "Err0649"
struct Arr
{
}

func(T) toto(v: [..] T)
{
}

#test
{
    var arr: Arr
    toto(arr)
}
