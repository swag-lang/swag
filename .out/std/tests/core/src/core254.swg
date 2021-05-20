using core

struct W
{
}

var G = 0

impl ILogWriter for W
{
    func print(using self, text: string)
    {
        @assert(text == "titi toto")
        G += 1

        Console.lock()
        Console.silent(true)
        //Console.setTextColor(Console.Color.Magenta)
        Console.println(text)
        //Console.resetColor()
        Console.silent(false)
        Console.unlock()
    }
}

#test
{
    var x: W

    logger := Log.create()
    logger.setFlags(LogFlags.Prefix)
    logger.setPrefix("titi")
    logger.addWriter(x)
    logger.addWriter(x)
    logger.print("toto")
    @assert(G == 2)
}