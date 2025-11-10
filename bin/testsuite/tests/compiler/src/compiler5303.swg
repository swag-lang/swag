#test
{
    var a = [0, 1, 2, 3]

    #[Swag.Mixin]
    func add(offset: u64)->&s32
    {
        return @dataof(a) + offset
    }

    let c = true ? add(1) : add(2)
    @assert(c == 1)
    let d = false ? add(1) : add(2)
    @assert(d == 2)
}
