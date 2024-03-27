#global testerror "Saf0023"

#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    const x = 0xFFFFFFFF_FFFFFFFF'u64
    let y   = cast(s64) x
}
