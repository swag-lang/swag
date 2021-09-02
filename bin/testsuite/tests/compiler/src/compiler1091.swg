#[Swag.EnumFlags]
enum TITI
{
	F1
	F2
	F3
}

#test
{
	var x : TITI = TITI.F1 | TITI.F2
}
