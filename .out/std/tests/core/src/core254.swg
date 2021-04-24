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

        console.lock()
        console.silent(true)
        //console.setTextColor(console.Color.Magenta)
        console.println(text)
        //console.resetColor()
        console.silent(false)
        console.unlock()
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