//#[Swag.PrintBc]
#test
{
    {
        var x = 0
        var y: @decltype(x) = 6
        @assert(@typeof(x) == @typeof(y))
        @assert(@typeof(y).fullname == "s32")

        var aa = [1, 2]
        var ptr = @dataof(aa)
        @assert(@typeof(ptr).fullname == "^s32")
        @assert(@typeof(ptr) == @typeof(^s32))

        var ptr1: const @decltype(ptr)
        @assert(@typeof(ptr1).fullname == "const ^s32")
        @assert(@typeof(ptr1) == @typeof(const ^s32))
    }
}