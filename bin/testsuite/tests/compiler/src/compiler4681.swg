#[Swag.ConstExpr]
struct A { x: s32 = 66 }

const s: const [..] A = [{66}]
const p  = @dataof(s)
const v  = dref p
#assert(v.x == 66)
