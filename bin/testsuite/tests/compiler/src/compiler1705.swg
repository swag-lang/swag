#global testerror "Err0597"

#test
{
    var str = "string"
    var strSlice2: [..] u8 = {@dataof(str), 2}
    //strSlice2[0] = 0
}