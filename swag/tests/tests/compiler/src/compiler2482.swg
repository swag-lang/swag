using swag

#[safety("of", true)]
//#[printbc, optim("bc", false)]
#test
{
    {
        x := swag.MinS32
        #runerror x0 := cast(s8) x
    }

    {
        x := swag.MaxS32
        #runerror x0 := cast(s8) x
    }
}