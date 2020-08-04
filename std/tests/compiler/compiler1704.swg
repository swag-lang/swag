#test
{
    var str = "string"
    var strSlice1 = @{@dataof(str), 2}
    @assert(@typeof(strSlice1.item0) == @typeof(const*u8))
    @assert(@typeof(strSlice1.item1) == @typeof(s32))
    //var strSlice2: [..] u8 = {@dataof(str), 2}
    //strSlice2[0] = 0
}