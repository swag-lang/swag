enum ContextFlags: u64
{
    None     = 0x00000000_00000000
    Test     = 0x00000000_00000001
    DevMode  = 0x00000000_00000002
    ByteCode = 0x00000000_00000004
}

struct A
{
    x =  ContextFlags.None
}

var G: A
