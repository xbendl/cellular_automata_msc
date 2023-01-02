# cellular_automata_msc
This repository contains a software created for my master thesis "Intruction-controlled cellular automata". Text of my thesis can be found at:
https://www.fit.vut.cz/study/thesis/11121/.en?year=0&stud=bendl (czech language)

![GUI](https://github.com/xbendl/cellular_automata_msc/blob/main/fig.png)

Abstract:
==========
The thesis focuses on a new concept of cellular automata control based on instructions. The instruction can be understood as a rule that checks the states of cells in pre-defined areas in the cellular neighbourhood. If a given condition is satisfied, the state of the central cell is changed according to the definition of the instruction. Because it’s possible to perform more instructions in one computational step, their sequence can be understood as a form of a short program. This concept can be extended with simple operations applied to the instruction’s prescription during interpretation of the instructions – an example of such operation can be row shift or column shift. An advantage of the instruction-based approach lies in the search space reduction. In comparison with the table-based approach, it isn’t necessary to search all the possible configurations of the cellular neighbouhood, but only several areas determined by the instructions. While the groups of the inspected cells in the cellular neighbourhood are designed manually on the basis of the analysis of the solved task, their sequence in the chromosome is optimized by genetic algorithm. The capability of the proposed method of cellular automata control is studied on these benchmark tasks - majority, synchronization, self-organization and the design of combinational circuits.

Installation:
============
Run `install.sh` with root permission (because it needs to install Qwt)

How to run it:
=============
```Synopsis: ./automaton [-u usage]```

-u Optional parameters specifying the program use, i.e. 1 = evolution of CA rules, 2 = analysis of CA rules, 3 = CA rules designer, 4 = CA task designer

User interface:
===============
Software contains three main sections (tabs) for work with cellular automata (CA):
- Evolution of CA rules: To run evolution of instruction sets for self-organization problems
- Analysis of CA instructions: To validate the evolved CA design by checking CA behaviour in stepwise manner
- Create CA instructions: Design of instruction sets
- Create CA jobs: Specify initial CA configuration and final CA configurations for training of self-organization skills 
