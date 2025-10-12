/*#global skip
interface I
{
    func toto(me);
}

struct A
{

}

impl I for A
{
    mtd impl toto() {}
}

#[Swag.Optimize(.All, false)]
#test
{
    var a: A
    var i = cast(I) a

    struct TT
    {
        obj:    *void
        itable: const ^[*] void
    }

    let ptr = cast(*TT) &i
    //@print(&a, " ", ptr.obj)

    let t = cast(const *Swag.TypeInfoStruct) ptr.itable[cast #bit (u64) -1's64]
    //@print(" ", t.fullname)

    let t1 = cast(const *Swag.TypeInfoStruct) ptr.itable[cast #bit (u64) -2's64]
    //@print(" ", t1.fullname)

    var itf1: TT
    itf1.obj = ptr.obj
    itf1.itable = cast() @tableof(t, t1)
    @assert(ptr.itable == itf1.itable)
}*/
