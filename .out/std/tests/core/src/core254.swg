using core

struct W
{
}

impl ILogWriter for W
{
    func print(using self, text: string)
    {
        console.lock()
        console.setTextColor(console.Color.Magenta)
        console.println(text)
        console.resetColor()
        console.unlock()
    }
}

#test
{
    var x: W

    logger := Log.create()
    logger.writers.add(x)
    logger.writers.add(x)
    logger.print("toto")
}