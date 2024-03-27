#global testerror "Err0623"

#[Swag.Strict]
typealias C = func(s32)

#test
{
    var y: C = func(a) { return a; }
}
