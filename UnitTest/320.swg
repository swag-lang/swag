#module module320;
#unittest pass semantic;

var a: [6] s32;
#assert @prop(a, sizeof) == 24;

var b: [6, 5] s32;
#assert @prop(b, sizeof) == 120;

var c: [6, 5] *s32;
#assert @prop(c, sizeof) == 240;

var d: [6, 5] **s32;
#assert @prop(d, sizeof) == 240;