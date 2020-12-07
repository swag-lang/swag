struct A
{
    myTruc: u32
    buffer: [2] u32
}

impl A
{
    #[swag.inline]
    func(T) titi(x, y: T) => 1'u32

    #[swag.macro]
    func(ptr: bool) toto(using self, stmt: code)
    {
        #assert ptr == true
        loop idx: titi's32(1, 1)
        {
            var shift: u32 = 0
            loop 1
            {
                #macro
                {
                    var @alias0 = cast(bool) (buffer[`idx] & `shift)
                    myTruc = `idx + 1
                    #mixin `stmt
                }
            }
        }
    }
}

//#[swag.printbc]
#test
{
    var x: A
    x.toto'true()
    {

    }

    @assert(x.myTruc == 1)
}