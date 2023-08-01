//#global skip
namespace A
{
    typealias B = s32
}

//#global testerror
var G: {a: A.B, b: s32 = 1}
var F: func(a: A.B, c: s32)