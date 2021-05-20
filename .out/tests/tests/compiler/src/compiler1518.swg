struct A
{
    myTruc: uint
    buffer: [2] u32
}

impl A
{
    #[Swag.inline]
    func(T) titi(x, y: T) => 1'u32

    #[Swag.macro]
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
                    var @alias0 = cast(bool) (`self.buffer[`idx] & `shift)
                    `self.myTruc = `idx + 1
                    #mixin `stmt
                }
            }
        }
    }
}

//#[Swag.printbc]
#test
{
    var x: A
    x.toto'true()
    {

    }

    @assert(x.myTruc == 1)
}