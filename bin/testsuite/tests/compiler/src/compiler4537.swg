#global testerror "Err0692"

#[Swag.Strict]
typealias C = func(s32)

#test
{
    var y: C = func(a) { return a; }
}
