#unittest module module185;
#unittest pass semantic;
using swag;

#[constExpr]
func main: () -> u32 { return 1; }

#assert 1 << main == 2;
#assert 8 >> main == 4;

