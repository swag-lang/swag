interface ErrorId
{
    toString: func(self)->string
}

struct ErrorString
{
    err: string
}

impl ErrorId for ErrorString
{
    func toString(using self)->string
    {
        return err
    }
}
/*
func toto()->ErrorId
{

}
*/

#test
{
}