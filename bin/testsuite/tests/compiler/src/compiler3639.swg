#global skip
var G = 0

func(T) toto(x: const [..] T)
{
    G += 2
}

func(T) toto(x: [4] T)
{
    G += 1
}

#test
{
    var a: [4] s32
    toto(a)
}