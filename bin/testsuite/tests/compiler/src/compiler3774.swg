interface I
{
    toto: func(self)
}

struct A
{

}

impl I for A
{
    mtd impl toto() {}
}

#[Swag.Optim("", false)]
#test
{
    var a: A
    i := cast(I) a

    struct TT
    {
        obj:    *void
        itable: const ^^void
    }

    ptr := cast(*TT) &i
    //@print(&a, " ", ptr.obj)

    t := cast(const *Swag.TypeInfoStruct) ptr.itable[cast,bit(u64) -1's64]
    //@print(" ", t.fullname)

    t1 := cast(const *Swag.TypeInfoStruct) ptr.itable[cast,bit(u64) -2's64]
    //@print(" ", t1.fullname)

    var itf1: TT
    itf1.obj = ptr.obj
    itf1.itable = acast @itftableof(t, t1)
    @assert(ptr.itable == itf1.itable)
}