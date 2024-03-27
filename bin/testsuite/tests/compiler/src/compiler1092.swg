#global testerror "Err0351"

#[Swag.EnumFlags]
enum TITI
{
	F1
	F2
	F3
}

#test
{
	const x : TITI = TITI.F1 + TITI.F2
}
