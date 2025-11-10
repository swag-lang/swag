//#global skip
struct(K, V) A
{
    k:     K
    v:     V
}

#test
{
    enum E: string
    {
        N = null
        R = "toto"
    }

    var a: A'(s32, E)
}
