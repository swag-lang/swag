var f = 100'u64
var f1 = 100'u32

//#[Swag.PrintBc]
#test
{
    var v = f / cast(f32) f1
}