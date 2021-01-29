using swag

#[safety("of", true)]
//#[printbc]
#test
{
    {
        var x: s8 = -1
        #runerror x >>= 1
    }
}