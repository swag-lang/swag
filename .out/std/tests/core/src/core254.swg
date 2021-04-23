using core

struct W
{
}

impl ILogWriter for W
{
    func print(using self, text: string)
    {
        console.println(text)
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