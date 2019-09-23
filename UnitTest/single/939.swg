#module module939

var x: [] any = [6]

//#[swag.printbc]
#test
{
	t := x[0]
	type := @kindof(t)
	@print(type.name)
}
