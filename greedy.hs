data Path = Path
  deriving Show

greedy :: Eq a => [(a,Int)] -> [[a]] -> [a] -> Path
greedy _ds _is _o = Path

-- 1.) look for any hadamard products
-- 2.) greedily select what shrinks size of difference of the sum of the size of the input and the size of the output
-- 3.) compute pairwise outer products that remain choosing the pair that minimizes input_sizes

main = do
  print $ greedy 
     [('x',35),('y',37),('f',59),('t',51),('p',51),('r',27)]
     ["xyf","xtf","ytpf","fr"] "tpr" 

-- we're doing this at compile time, so we can afford to be a bit closer to optimal than greedy.
-- which gives something like a 16x slowdown in this example.
