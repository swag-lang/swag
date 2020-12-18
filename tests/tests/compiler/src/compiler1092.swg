
#[swag.enumflags]
enum TITI
{
	F1
	F2
	F3
}

#unittest error
#test
{
	let x : TITI = TITI.F1 + TITI.F2
}
