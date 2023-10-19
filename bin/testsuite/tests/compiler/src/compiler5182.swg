#global testerror "Err0684"

enum BasicErrors0: f32
{
    FailedToLoad0 = 1.1
}

enum BasicErrors1: f32
{
    FailedToLoad1 = 1.1
}

#[Swag.NoDuplicate]
enum BasicErrors2: @decltype(@kindof(BasicErrors1))
{
    using BasicErrors0
    using BasicErrors1
}
