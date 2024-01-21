#global testerror "Err0067"

#[Swag.EnumFlags]
enum BasicErrors0: u32
{
    FailedToLoad
}

#[Swag.EnumFlags]
enum BasicErrors1: u32
{
    FailedToLoad
}

#[Swag.NoDuplicate]
enum BasicErrors2: @decltype(@kindof(BasicErrors1))
{
    using BasicErrors0
    using BasicErrors1
}
