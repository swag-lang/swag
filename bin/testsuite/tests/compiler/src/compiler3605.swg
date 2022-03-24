//#global skip
struct(K, V) A
{
    k: K
    v: V
}

#test
{
    enum E: string
    {
        R = "toto"
    }

    var a: A'(s32, E)
}