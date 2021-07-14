using Swag

#[safety("overflow", true)]
//#[PrintBc]
#test
{
    {
        var x: s8 = 1
        x >>=,safe 1
        #assert 1 >>,safe 1 == 0
        #assert Swag.U32.Max <<,safe 1 == 0xFFFFFFFE
    }
}