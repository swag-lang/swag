#global testerror "Err0604"
struct Arr {}

func(T) toto(v: [..] T) {}

#test
{
    var arr: Arr
    toto(arr)
}
