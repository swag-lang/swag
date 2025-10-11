var x: s32
var y: const *#decltype(x)
#assert(#typeof(y) == const *s32)

var x1: *s32
var y1: const #decltype(x1)
#assert(#typeof(y1) == const *s32)
