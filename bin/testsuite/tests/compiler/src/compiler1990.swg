//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    {
        var x1 = 10 <=> 20
        @assert(x1 == -1)
        var x2 = 20 <=> 10
        @assert(x2 == 1)
        var x3 = 20 <=> 20
        @assert(x3 == 0)
    }

    {
        var x1 = 10'f32 <=> 20'f32
        @assert(x1 == -1)
        var x2 = 20'f32 <=> 10'f32
        @assert(x2 == 1)
        var x3 = 20'f32 <=> 20'f32
        @assert(x3 == 0)
    }
}