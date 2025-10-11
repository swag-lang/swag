//#global skip

struct A {}
var G = [1, 2]
impl A
{
    #[Swag.Overload]
    mtd const toto()->const [..] s32 { return G; }
    #[Swag.Overload]
    mtd toto()->[..] s32 { return G; }
}

#test
{
    var a: A
    var b: const [..] s32 = a.toto()
}
