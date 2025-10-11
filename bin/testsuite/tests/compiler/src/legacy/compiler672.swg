//#[Swag.PrintBc]
#test
{
    /*var a : {a,b: s32}
	a.0 = 1
	a.1 = 2*/

    let a: s32 = 5
    let b: f32 = 10
    let c  = a + b
    @assert(c == 15.0)
}
