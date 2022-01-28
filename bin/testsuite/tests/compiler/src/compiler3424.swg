//#global skip

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a := 10
    b := 20
    var v: closure(s32)/*->s32*/ = @|a,b|(x: s32)/*->s32*/
    {
        @assert(a == 10)
        @assert(b == 20)
        @assert(x == 5)
        //return x + a + b
    }

    v(5)
    //x := v(5)
    //@print(x)
}
