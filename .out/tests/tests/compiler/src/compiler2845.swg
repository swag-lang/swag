//#[swag.safety("", true)]
#test
{
    {
        x := -1's32 >>% 320
        @assert(x == -1)
        x = x >>% 100
        @assert(x == -1)
        x <<= 1
        @assert(x == -2)
    }

    {
        x := 50'u32 >>% 320
        @assert(x == 0)
        x = x >>% 100
        @assert(x == 0)
        x <<= 1
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF'u32
        x >>%= 100
        //@assert(x == 0)
    }
}
