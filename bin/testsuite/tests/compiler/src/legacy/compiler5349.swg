#global testerror "Err0561"
struct Arr {}

func(T) toto(v: [..] T) {}

#test
{
    var arr: Arr
    toto(arr)
}
