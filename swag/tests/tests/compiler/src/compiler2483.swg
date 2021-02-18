/*using swag

#[safety("of", true)]
//#[printbc]
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
        #runerror x2 := cast(s32) x
    }

    {
        x := swag.MaxU64
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
        #runerror x2 := cast(s32) x
        #runerror x3 := cast(s64) x
    }
}*/