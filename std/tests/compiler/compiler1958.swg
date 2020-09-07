struct A
{
    x, y, z: s32
}

interface I
{
    toto: func(self)
    toto1: func(self)
}

impl I for A
{
    func toto(self)
    {
        /*@print("toto ")
        @print(cast(s64) self.x)
        @print("\n")*/
    }

    func toto1(self)
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

//#[swag.printbc]
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
    y := &x
    y.toto()

    var b: BB
    b.itf = x
    b.itf.toto()
    b.itf.toto1()

    b.pitf = &x
    b.pitf.toto()
    b.pitf.toto1()

    var xx: swag.IAllocator
    xx = @getcontext().allocator
}