;
;
using Swag;

#assert 1 | 2 == 3;
#assert 1 & 2 == 0;
#assert 3 & 1 == 1;
#assert 0xFF000000 | 0xFF == 0xFF0000FF;

#[ConstExpr]
func mask () -> s32 { return 1; }

#assert mask() | 2 == 3;