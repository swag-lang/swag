//#global skip
namespace A
{
    alias B = s32
}

//#global testerror
var G: {A.B, b: s32 = 1}
var F: func(A.B, c: s32)