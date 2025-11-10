//#[Swag.ConstExpr]
struct A { x: s32 }
func toto()->A { return A{1}; }

const C = #run toto()
#assert(C.x == 1)
