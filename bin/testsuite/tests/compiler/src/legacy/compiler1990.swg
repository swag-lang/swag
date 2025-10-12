//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    {
        let x1 = 10 <=> 20
        @assert(x1 == -1)
        let x2 = 20 <=> 10
        @assert(x2 == 1)
        let x3 = 20 <=> 20
        @assert(x3 == 0)
    }

    {
        let x1 = 10'f32 <=> 20'f32
        @assert(x1 == -1)
        let x2 = 20'f32 <=> 10'f32
        @assert(x2 == 1)
        let x3 = 20'f32 <=> 20'f32
        @assert(x3 == 0)
    }
}
