using swag

#[safety("of", true)]
//#[printbc, optim("bc", false)]
#test
{
    {
        x := swag.MinS16
        #runerror x0 := cast(s8) x
    }

    {
        x := swag.MaxS16
        #runerror x0 := cast(s8) x
    }

    {
        x := swag.MinS32
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
    }

    {
        x := swag.MaxS32
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
    }

    {
        x := swag.MinS64
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
        #runerror x2 := cast(s32) x
    }

    {
        x := swag.MaxS64
        #runerror x0 := cast(s8) x
        #runerror x1 := cast(s16) x
        #runerror x2 := cast(s32) x
    }
}