using Swag

#[ConstExpr]
func main () -> u32 { return 1; }

#assert 1 << main() == 2;
#assert 8 >> main() == 4;
