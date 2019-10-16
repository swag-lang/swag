;
;

var a: [6] s32;
#assert @sizeof(a) == 24;

var b: [6, 5] s32;
#assert @sizeof(b) == 120;

var c: [6, 5] *s32;
#assert @sizeof(c) == 240;

var d: [6, 5] **s32;
#assert @sizeof(d) == 240;
