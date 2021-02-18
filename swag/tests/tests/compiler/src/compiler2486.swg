/*using swag

#[safety("of", true)]
//#[printbc]
#test
{
    {
        var x: u8 = 1
        #runerror x >>= 1
    }

    {
        var x: u16 = 1
        #runerror x >>= 1
    }

    {
        var x: u32 = 2
        #runerror x >>= 2
    }

    {
        var x: u64 = 4
        #runerror x >>= 4
    }
}*/