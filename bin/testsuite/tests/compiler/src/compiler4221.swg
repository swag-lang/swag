//#global skip
interface I
{
    mtd call2();
    mtd call1();
    mtd call()->s32;
}

struct A {}
impl I for A
{
    mtd impl call1() { @assert(false) }
    mtd impl call2() { @assert(false) }

    mtd impl call()->s32
    {
        @assert(me == &x)
        return 666
    }
}

var x: A
var G: s32

#[Swag.Inline]
func toto()->I => &x

//#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    let h = toto().call()
    @assert(h == 666)
}
