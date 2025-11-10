//#global skip
interface I
{
    mtd call1();
    mtd call();
}

struct A {}
impl I for A
{
    mtd impl call1() { @assert(false) }

    mtd impl call()
    {
        G += 1
        @assert(me == &x)
    }
}

var x: A
var G: s32
func toto()->I => &x

//#[Swag.PrintBc]
#test
{
    toto().call()
    @assert(G == 1)
}
