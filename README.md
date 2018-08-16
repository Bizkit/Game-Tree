# Game-Tree

Implementation of "Tic-Tac-Toe" game tree, made in C, using multi way trees.
Minimax and Alpha Beta Pruning AI algorithms were used in creating the game tree.

Compiling:  
- use Makefile  
or  
- gcc -g source.c -o minimax  

For running:  
./minimax -c1 file_input file_output -> game tree  
./minimax -c2 file_input file_output -> AND / OR tree  
./minimax -c3 file_input file_output -> minimax tree  
./minimax -b file_input file_output -> alpha beta pruning tree  
