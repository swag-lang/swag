#global testerror "Err0048"

//#[Swag.ConstExpr]
struct A { x: s32; }
#[Swag.ConstExpr]
func toto()->A { return A{1}; }

const C = toto()
#assert C.x == 1
