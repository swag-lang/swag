struct A
{
    x, y, z: s32
}

interface I
{
    func toto(self);
    func toto1(self);
}

impl I for A
{
    func impl toto(self)
    {
        /*@print("toto ")
        @print(cast(s64) self.x)
        @print("\n")*/
    }

    func impl toto1(self)
    {
        /*@print("toto1 ")
        @print(cast(s64) self.x)
        @print("\n")*/
    }
}

var G: *I

func titi(itf: *I)
{
    itf.toto()
    itf.toto1()
}

struct BB
{
    itf: I
    pitf: *I
}

//#[Swag.PrintBc]
#test
{
    var x: I
    var st: A
    x = cast(I) st
    G = &x
    x.toto()
    x.toto1()
    G.toto()
    G.toto1()
    titi(&x)
    let y = &x
    y.toto()

    var b: BB
    b.itf = x
    b.itf.toto()
    b.itf.toto1()

    b.pitf = &x
    b.pitf.toto()
    b.pitf.toto1()

    var xx: Swag.IAllocator
    xx = @getcontext().allocator
}