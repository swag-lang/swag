func plus(xx, yy: s32) => xx + yy
func minus(xx, yy: s32) => xx - yy

const A = plus(4, 5) + minus(5, 4)
#assert A == 10

func plus2(xx, yy: s32) => plus(xx, yy)
const B = plus2(10, 5)
#assert B == 15