#global skip // no more relative pointers (18 july 2021)
func(N: s32) toto()
{
    var x: *~N u8
}

#test
{
    toto'32()
}