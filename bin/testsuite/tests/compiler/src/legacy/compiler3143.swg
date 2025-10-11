func(T) toto(x: #null const [..] T) {}

#test
{
    var x1: [..] s32
    var x2: const [..] s32
    toto(x1)
    toto(x2)
}
