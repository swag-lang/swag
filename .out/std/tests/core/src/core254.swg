using core

struct W
{
}

var G = 0

impl ILogWriter for W
{
    func print(using self, text: string)
    {
        @assert(text == "toto")
        G += 1
        /*console.lock()
        console.setTextColor(console.Color.Magenta)
        console.println(text)
        console.resetColor()
        console.unlock()*/
    }
}

#test
{
    var x: W

    logger := Log.create()
    logger.setFlags(LogFlags.None)
    logger.addWriter(x)
    logger.addWriter(x)
    logger.print("toto")
    @assert(G == 2)
}