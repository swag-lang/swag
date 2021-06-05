//#[Swag.printbc]
#[Swag.optim("", false)]
#test
{
    {
        x1 := 10 <=> 20
        @assert(x1 == -1)
        x2 := 20 <=> 10
        @assert(x2 == 1)
        x3 := 20 <=> 20
        @assert(x3 == 0)
    }

    {
        x1 := 10'f32 <=> 20'f32
        @assert(x1 == -1)
        x2 := 20'f32 <=> 10'f32
        @assert(x2 == 1)
        x3 := 20'f32 <=> 20'f32
        @assert(x3 == 0)
    }
}