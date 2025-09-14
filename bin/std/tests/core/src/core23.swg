using Core

#test
{
    let x: String = "coucou\n"
    Console.silent(true)
    Console.print(cast(string) x)
    Console.silent(false)
}
