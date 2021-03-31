#test
{
    #semerror x := 10'u32 >> 32
    #semerror x := 10'u64 >> 64
    #semerror x := 10'u32 << 32
    #semerror x := 10'u64 << 64
}

#test
{
    x := -1's64
    //x >>= 1
    @print(x)
}