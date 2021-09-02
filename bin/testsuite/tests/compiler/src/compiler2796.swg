#[Swag.EnumFlags]
enum RGB
{
    R
    G
    B
}

#test
{
    x := RGB.R
    x = .R
}