#[Swag.Strict]
alias Handle = u32

#test
{
    var x, y: Handle
    x = cast(Handle) y
    var z: u32 = cast(u32) x
}