//#global testerror "Err0243"

typealias B = f32
const A = 1'B
#assert @typeof(A) == f32
