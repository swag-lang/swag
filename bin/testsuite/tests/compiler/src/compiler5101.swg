//#global testerror "Err0262"

typealias B = f32
const A = 1'B
#assert @typeof(A) == f32
