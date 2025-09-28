#global testerror "Saf0001"

struct A {x: s32}

func toto() throw
{
    throw A{42}
}

#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#test
{
    catch toto()
    //@breakpoint()
    let err = cast(s32) @err
    //@print(err.x)
}