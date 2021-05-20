//#[Swag.printbc]
#test
{
    {
        x := 0
        var y: @typeof(x) = 6
        @assert(@typeof(x) == @typeof(y))
        @assert(@typeof(y).name == "s32")

        var aa = @[1, 2]
        ptr := @dataof(aa)
        @assert(@typeof(ptr).name == "*s32")
        @assert(@typeof(ptr) == @typeof(*s32))

        var ptr1: const @typeof(ptr)
        @assert(@typeof(ptr1).name == "const *s32")
        @assert(@typeof(ptr1) == @typeof(const *s32))
    }
}