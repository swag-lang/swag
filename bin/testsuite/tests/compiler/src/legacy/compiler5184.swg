//#global testerror "Err0417"

enum BasicErrors0: f32
{
    FailedToLoad0 = 1.1
}

enum BasicErrors1: f32
{
    FailedToLoad1 = 1.1
}

enum BasicErrors2: #decltype(#typeof(BasicErrors1).rawType) { using BasicErrors0, using BasicErrors1 }
