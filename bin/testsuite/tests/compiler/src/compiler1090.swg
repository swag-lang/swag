#global testerror "Err0596"

enum TITI
{
	F1
	F2
	F3
}

/*#[Swag.EnumFlags]
enum TOTO : u32
{
	F1 = 0x00000001
	F2 = 0x00000002
	F3 = F1 | F2
}*/

#test
{
	const x : TITI = TITI.F1 | TITI.F2
}
