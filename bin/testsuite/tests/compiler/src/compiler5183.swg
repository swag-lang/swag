#global testerror "Err0586"

enum BasicErrors0: const [4] s32
{
    FailedToLoad = [0, 1, 2, 3]
}

enum BasicErrors1: const [4] s32
{
    FailedToLoad = [0, 1, 2, 3]
}

#[Swag.NoDuplicate]
enum BasicErrors2: #decltype(#typeof(BasicErrors1).rawType)
{
    using BasicErrors0
    using BasicErrors1
}
