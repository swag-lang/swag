/*
--- Day 10: Balance Bots ---
You come upon a factory in which many robots are zooming around handing small microchips to each other.

Upon closer examination, you notice that each bot only proceeds when it has two microchips, and once it does, it gives each one to a different bot or puts it in a marked "output" bin. Sometimes, bots take microchips from "input" bins, too.

Inspecting one of the microchips, it seems like they each contain a single number; the bots must use some logic to decide what to do with each chip. You access the local control computer and download the bots' instructions (your puzzle input).

Some of the instructions specify that a specific-valued microchip should be given to a specific bot; the rest of the instructions indicate what a given bot should do with its lower-value or higher-value chip.

For example, consider the following instructions:

value 5 goes to bot 2
bot 2 gives low to bot 1 and high to bot 0
value 3 goes to bot 1
bot 1 gives low to output 1 and high to bot 0
bot 0 gives low to output 2 and high to output 0
value 2 goes to bot 2
Initially, bot 1 starts with a value-3 chip, and bot 2 starts with a value-2 chip and a value-5 chip.
Because bot 2 has two microchips, it gives its lower one (2) to bot 1 and its higher one (5) to bot 0.
Then, bot 1 has two microchips; it puts the value-2 chip in output 1 and gives the value-3 chip to bot 0.
Finally, bot 0 has two microchips; it puts the 3 in output 2 and the 5 in output 0.
In the end, output bin 0 contains a value-5 microchip, output bin 1 contains a value-2 microchip, and output bin 2 contains a value-3 microchip. In this configuration, bot number 2 is responsible for comparing value-5 microchips with value-2 microchips.

Based on your instructions, what is the number of the bot that is responsible for comparing value-61 microchips with value-17 microchips?
*/

using Core

private struct Bot
{
    id:          s32
    count:       s32
    val:         [2] s32 = Swag.S32.Max
    out:         s32
    pending:     Array'String
}

private var result = 0
private var bots: Array'Bot

private impl Bot
{
    mtd add(v: s32)
    {
        val[count] = v
        orderMinMax(&val[0], &val[1])
        count += 1

        if count == 2 and val[0] == 17 and val[1] == 61
        {
            result = id
            @assert(id == 47)
        }
    }

    mtd eatLow()->s32
    {
        let v = val[0]
        val[0] = val[1]
        count -= 1
        return v
    }

    mtd eatHigh()->s32
    {
        let v = count == 1 ? val[0] : val[1]
        count -= 1
        return v
    }

    mtd execute()
    {
        if count == 2 and pending.count
        {
            let cpy = pending
            pending.clear()
            execute(cpy)
        }
    }
}

private func getBot(x: s32)->&Bot
{
    bots.growResize(x + 1)
    bots[x].id = x
    return bots.ref1(x)
}

private func execute(lines: Array'String) assume
{
    var r: Parser.RegExp
    visit l: lines
    {
        switch
        {
        case r.match(#"value (\d+) goes to bot (\d+)"#, l):
            let bot = ref getBot(StrConv.toS32(r[2]))
            bot.add(StrConv.toS32(r[1]))
            bot.execute()

        case r.match(#"bot (\d+) gives low to (\w+) (\d+) and high to (\w+) (\d+)"#, l):
            let bot = ref getBot(StrConv.toS32(r[1]))
            if bot.count != 2
            {
                bot.pending.add(l)
                break
            }

            let low  = ref getBot(StrConv.toS32(r[3]))
            let high = ref getBot(StrConv.toS32(r[5]))

            if r[2] == "bot" do
                low.add(bot.eatLow())
            else do
                low.out = bot.eatLow()

            if r[4] == "bot" do
                high.add(bot.eatHigh())
            else do
                high.out = bot.eatHigh()

            low.execute()
            high.execute()
        }
    }
}

func day10AB() assume
{
    Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "10.txt")
    var lines    = File.readAllLines(dataPath)
    execute(lines)

    @assert(result == 47)
    @assert(bots[0].out * bots[1].out * bots[2].out == 2666)
}
