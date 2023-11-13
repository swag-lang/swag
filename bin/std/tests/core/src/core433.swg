
using Core

//#[Swag.PrintBc]
#test
{
    var stream = catch File.openRead("tolklkjlkjk")
    @assert(@err() == Errors.FileNotFoundError)

    catch Directory.setCurrent("fkdfjfkjfkdj")
    @assert(@err() == Errors.FileNotFoundError)

    catch Directory.setCurrent("fkdfjfk?..??/jfkdj")
    @assert(@err() == Errors.InvalidFileNameError)    
}
