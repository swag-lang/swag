var x: s32
var y: const *@typeof(x)
#assert @typeof(y) == const *s32

var x1: *s32
var y1: const @typeof(x1)
#assert @typeof(y1) == const *s32