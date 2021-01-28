using swag

#[safety("of", true)]
//#[printbc, optim("bc", false)]
#test
{
    {
        x := swag.MaxU8
        #runerror x0 := cast(s8) x
    }

    {
        x := swag.MaxU16
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
    }

    {
        x := swag.MaxU32
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
    }

    {
        x := swag.MaxU64
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
    }
}