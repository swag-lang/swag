#test
{
    var str = "string"
    var strSlice1 = {@dataof(str), 2}
    var strSlice2: [..] u8 = {@dataof(str), 2}
}