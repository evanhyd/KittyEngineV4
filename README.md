# KittyEngineV4

A magic bitboard based chess engine written in C++.  
Partially support Universal Chess Interface.  
This project has been succeeded by [KittyEngineV5](https://github.com/evanhyd/KittyEngineV5).  

# Perft

## Initial Position
perft 6 Node: 119060324 Time: 3310 ms Speed: 35969 knode/s

## Kiwipete
perft 5 Node: 193690690 Time: 5299 ms Speed: 36552 knode/s  

# Decision Tree  
Negamax + PV search with alhpa-beta pruning  

# Pruning Technique  
Move Ordering  
PV Table  
Null Move Pruning  
Late Move Reduction  
Quiescence Search  
Transposition Table with Zobrist Hashing  
Respiratory Window  

# To-Do List:  
Book Opening  
Better pruning technique  
Multithreading Searching  

# Reference  
A bitboard-based chess engine guided by Code Monkey King:<br />
https://www.youtube.com/channel/UClA-jNuyJKqN-xCm7KPG_XA<br />
https://www.youtube.com/channel/UCB9-prLkPwgvlKKqDgXhsMQ<br />


Neural Network Architecture from David Miller: 
David Miller: http://www.millermattson.com/dave/<br />

