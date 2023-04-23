using Swag

#[Safety("overflow", true)]
//#[PrintBc]
#test
{
    {
        var x: s8 = 1
        x >>=,over 1
        #assert 1 >>,over 1 == 0
        #assert Swag.U32.Max <<,over 1 == 0xFFFFFFFE
    }
}