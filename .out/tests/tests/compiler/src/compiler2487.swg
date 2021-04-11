using swag

#[safety("of", true)]
//#[printbc]
#test
{
    {
        var x: s8 = 1
        x >>%= 1
        #assert 1 >>% 1 == 0
        #assert swag.U32.Max <<% 1 == 0xFFFFFFFE
    }
}