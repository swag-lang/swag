using swag

#[safety("of", true)]
//#[printbc]
#test
{
    {
        var x: u8 = swag.MaxU8
        #runerror x <<= 1
    }

    {
        var x: u8 = 0b01000000
        #runerror x <<= 4
    }

    {
        var x: u16 = swag.MaxU16
        #runerror x <<= 1
    }

    {
        var x: u32 = swag.MaxU32
        #runerror x <<= 1
    }

    {
        var x: u64 = swag.MaxU64
        #runerror x <<= 1
        x = 1'u64 << 63
        #runerror x <<= 1
    }
}