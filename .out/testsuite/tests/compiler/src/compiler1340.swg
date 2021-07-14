
#global testerror

struct TOTO
{
	print: s32
}

struct TITI
{
    print:  bool
    print1: f32
}

#[Swag.PrintBc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
}