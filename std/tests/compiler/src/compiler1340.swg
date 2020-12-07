#module "compiler1340"
#unittest error

struct TOTO
{
	print: s32
}

struct TITI
{
    print:  bool
    print1: f32
}

#[swag.printbc]
#test
{
    var x: TOTO
    var y: TITI
    x = y
}