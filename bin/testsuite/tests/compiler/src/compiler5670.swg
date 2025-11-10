//#global skip

#[Swag.ConstExpr]
func square(n: s32)->s32
{
    return n * n
}

//const Squares = square(2)//[square(2), square(3)]

const Squares = [square(2), square(3)]
#assert(Squares[0] == 4)
#assert(Squares[1] == 9)
