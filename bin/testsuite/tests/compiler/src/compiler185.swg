using Swag

#[ConstExpr]
func main () -> u32 { return 1; }

#assert 1 <<,over main() == 2;
#assert 8 >>,over main() == 4;
