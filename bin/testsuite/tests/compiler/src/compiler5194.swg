#global testerror "Saf0002"

struct A {x: s32}

func toto() throw
{
    throw A{42}
}

#[Swag.Safety("", true)]
#test
{
    catch toto()
    let err = cast(s32) @err()
    //@print(err.x)
}