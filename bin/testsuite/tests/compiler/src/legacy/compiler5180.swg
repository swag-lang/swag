#global testerror "Err0010"

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
enum BasicErrors2: #decltype(#typeof(BasicErrors1).rawType)
{
    using BasicErrors0
    using BasicErrors1
}
