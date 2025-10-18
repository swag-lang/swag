#global testerror "Err0745"
struct Arr {}

func(T) toto(v: [..] T) {}

#test
{
    var arr: Arr
    toto(arr)
}
