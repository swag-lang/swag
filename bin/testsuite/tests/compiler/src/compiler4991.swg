#global testerror "Err0561"

struct(A, B) Foozle
{
    a: A
    b: B
}

func yes_auto2(x: Foozle, y: Foozle)
{
}

#test
{
    var fooz1: Foozle'(s32, f32)
    var fooz2: Foozle'(bool, #decltype(fooz1))
    yes_auto2(fooz1, fooz2)
}