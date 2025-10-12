struct A
{
    vv:  s32 = 666
}

var G = 0
impl A
{
    mtd opDrop()
    {
        G += 1
        @assert(me.vv == 666)
        //@print("drop ", vv, "\n");
    }

    mtd opCount()->u64 { return 6; }
    mtd opSlice(a, b: u64)->string { return "abcdef"; }
    mtd toString()->string { return "var abc =0"; }
}

//#[Swag.Compiler]
#[Swag.ConstExpr]
func toto()->A
{
    var x: A
    return x
}

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    //#[Swag.PrintBc]
    //#ast
    //{
    let r = toto()
    @assert(cast(string) r == "abcdef")
    //@print(cast(string) r)
    //   return cast(string) r //.toString()
    //}

    if @bytecode do
        @assert(G == 1)
}
