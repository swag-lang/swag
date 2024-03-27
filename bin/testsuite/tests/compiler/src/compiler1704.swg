#test
{
    let str       = "string"
    let strSlice1 = {@dataof(str), 2}
    @assert(@typeof(strSlice1.item0) == @typeof(const ^u8))
    @assert(@typeof(strSlice1.item1) == @typeof(s32))
}
