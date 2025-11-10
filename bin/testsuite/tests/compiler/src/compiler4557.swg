struct A {}
impl A
{
    mtd opCount() => 2'u64
    mtd opSlice(l, h: u64)->string
    {
        @assert(l == 0 and h == 1)
        return ""
    }
}

#test
{
    var x: A
    let y  = x[0 to ]
    @assert(y == "")
}
