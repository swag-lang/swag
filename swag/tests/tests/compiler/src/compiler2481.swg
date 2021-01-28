using swag

#[safety("of", true)]
//#[printbc, optim("bc", false)]
#test
{
    {
        x := -1's8
        #runerror x0 := cast(u8) x
        #runerror x1 := cast(u16) x
        #runerror x2 := cast(u32) x
        #runerror x3 := cast(u64) x
    }

    {
        x := -1's16
        #runerror x0 := cast(u8) x
        #runerror x1 := cast(u16) x
        #runerror x2 := cast(u32) x
        #runerror x3 := cast(u64) x
    }

    {
        x := 256's16
        #runerror x0 := cast(u8) x
    }

    {
        x := 256's32
        #runerror x0 := cast(u8) x
    }

    {
        x := 256's64
        #runerror x0 := cast(u8) x
    }
}