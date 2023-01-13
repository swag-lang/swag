using Swag

#[ConstExpr]
func main () -> u32 { return 1; }

#assert 1 <<,safe main() == 2;
#assert 8 >>,safe main() == 4;
