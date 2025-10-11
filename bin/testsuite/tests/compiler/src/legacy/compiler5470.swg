//#global skip
#[Swag.Inline]
func(X: s32) toto()->{ v: s32 }
{
    return {0}
}

#test
{
    let s  = toto'(4)()
    let s1 = toto'(5)()
    //@assert(#typeof(s.v) == [4] s32)
    //let s1 = toto'(5)()
    //@assert(#typeof(s1.v) == [5] s32)
}
