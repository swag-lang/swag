using Core

struct Generation
{
    all: Array'(*Solution)
}

const NumPop = 10'u32

var g_Rng:      Random.DefaultRng
var g_NumGenes: u32
var g_Gen:      [2] Generation
var g_CurGen:   u32

struct Solution
{
    genes:  Array'u32
    score:  f32
}

func genInit()
{
    g_NumGenes = g_Image0.width * g_Image0.height

    // Generate initial population
    var ranks: Array'u32
    loop indiv: NumPop
    {
        ranks.resize(cast(uint) g_NumGenes)
        loop i: g_NumGenes
            ranks[i] = i

        soluce := Memory.new'Solution()
        soluce.genes.resize(cast(uint) g_NumGenes)
        g_Gen[0].all.add(soluce)

        cpt := 0'uint
        while ranks.count
        {
            idx  := g_Rng.nextU32(0, cast(u32) ranks.count)
            idxP := ranks[idx]
            ranks.removeAt(cast(uint) idx)
            soluce.genes[cpt] = idxP
            cpt += 1
        }

        evaluateSolution(soluce)
    }

    // Placeholder for the next generation
    loop indiv: NumPop
    {
        soluce := Memory.new'Solution()
        soluce.genes.resize(cast(uint) g_NumGenes)
        g_Gen[1].all.add(soluce)
    }
}

func evaluateSolution(soluce: *Solution)
{
    soluce.score = 0

    visit idx: soluce.genes
    {
        rs := cast(f32) g_Image0.pixels[idx*3]
        gs := cast(f32) g_Image0.pixels[(idx*3)+1]
        bs := cast(f32) g_Image0.pixels[(idx*3)+2]

        rd := cast(f32) g_Image1.pixels[idx*3]
        gd := cast(f32) g_Image1.pixels[(idx*3)+1]
        bd := cast(f32) g_Image1.pixels[(idx*3)+2]

        soluce.score += Math.abs((rs - rd) / 255)
        soluce.score += Math.abs((gs - gd) / 255)
        soluce.score += Math.abs((bs - bd) / 255)
    }
}

func genGo()->*Solution
{
    // Selection

    // Returns the best score
    var bestSoluce: *Solution
    var bestScore = Swag.F32.Max
    visit soluce: g_Gen[g_CurGen].all
    {
        if soluce.score < bestScore
        {
            bestScore = soluce.score
            bestSoluce = soluce
        }
    }

    return bestSoluce
}