//#[Swag.PrintBc]
#run
{
    x := 0xFFFFFFFF'u64
    x %= 16'u32

    var m: u32 = 16
    x %= cast(u64) m
}