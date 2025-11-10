struct(T) Toto
{
    ptr: T
}

struct Titi
{
    a:     Toto'{ a, b: s32 }
    b:     Toto'{ a, b: string }
}

struct Tata
{
    titi: Titi
}

func toto(t: Tata = {})
{
    @assert(t.titi.a.ptr.a == 0)
    //@print(#typeof(t.titi.a).name)
}

#test
{
    toto()
}
