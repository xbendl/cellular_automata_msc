/**
* \file recombinationOperator.h
* \brief Spravce rekonfiguracnich operatoru (krizeni, mutace).
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef RECOMBINATION_OPERATOR_H_INCLUDED
#define RECOMBINATION_OPERATOR_H_INCLUDED
#include <iostream>
#include <vector>
#include <limits.h>
#include "config.h"
#include "automaton.h"

/// Vycet moznych variant rekombinacniho operatoru krizeni.
enum E_CROSSOVER
{
    CROSSOVER_PMX = 0,    ///< Krizeni typu PMX (partial matched crossover).
    CROSSOVER_OX = 1,     ///< Krizeni typu OX (order crossover).
    CROSSOVER_AP = 2,     ///< Krizeni typu AP (alternativ-position crossover).
    CROSSOVER_MPX = 3,    ///< Krizeni typu MPX (maximal preservative crossover).
    CROSSOVER_COUNT = 4   ///< Pocet variant krizeni.
};

/// Vycet moznych variant rekombinacniho operatoru mutace.
enum E_MUTATION
{
    MUTATION_SWAP = 0,
    MUTATION_IVM = 1,
    MUTATION_DM = 2,
    MUTATION_SM = 3,
    MUTATION_ISM = 4,
    MUTATION_COUNT = 5   ///< Pocet variant mutaci.
};

/// Spravce rekonfiguracnich operatoru (krizeni, mutace).
class RecombinationOperator
{
    public:
    /**
     * Provede krizeni dvou chromozomu nahodne vybranou metodou.
     * @param parent1 Prvni z krizenych chromozomu.
     * @param parent2 Druhy z krizenych chromozomu.
     * @param childPop Uloziste chromozomu vzniklych krizenim.
     */
    static void RandomCrossover(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::ChromosomeSet &childPop);

    /**
     * Provede krizeni dvou chromozomu s instrukcemi metodou PMX (partial matched crossover).
     * @param parent1 Prvni z krizenych chromozomu.
     * @param parent2 Druhy z krizenych chromozomu.
     * @param child Chromozom vznikly po aplikaci operatoru krizeni.
     */
    static void CrossoverPMX(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child);

    /**
     * Provede krizeni dvou chromozomu s instrukcemi metodou OX (order crossover).
     * @param parent1 Prvni z krizenych chromozomu.
     * @param parent2 Druhy z krizenych chromozomu.
     * @param child Chromozom vznikly po aplikaci operatoru krizeni.
     */
    static void CrossoverOX(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child);

    /**
     * Provede krizeni dvou chromozomu s instrukcemi metodou AP (alternating position crossover).
     * @param parent1 Prvni z krizenych chromozomu.
     * @param parent2 Druhy z krizenych chromozomu.
     * @param child Chromozom vznikly po aplikaci operatoru krizeni.
     */
    static void CrossoverAP(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child);

    /**
     * Provede krizeni dvou chromozomu s instrukcemi metodou MPX (maximal preservative crossover).
     * @param parent1 Prvni z krizenych chromozomu.
     * @param parent2 Druhy z krizenych chromozomu.
     * @param child Chromozom vznikly po aplikaci operatoru krizeni.
     */
    static void CrossoverMPX(Automaton::Chromosome &parent1, Automaton::Chromosome &parent2, Automaton::Chromosome &child);

    /**
     * Provede mutaci chromozomu nahodne vybranou metodou.
     * @param instrList Mnozina dostupnych instrukci.
     * @param chromosome Vybrany jedinec populace urceny k mutaci.
     */
    static void RandomMutation(std::vector<Automaton::Instruction> &instrList, Automaton::Chromosome &chromosome);

    /**
     * Mutace typu jednoduchy swap (zamena dvou nahodne vybranych instrukci).
     * @param chromosome Vybrany jedinec populace urceny k mutaci.
     */
    static void MutationSwap(Automaton::Chromosome &chromosome);

    /**
     * Mutace typu Inversion mutation (IVM).
     * @param chromosome Vybrany jedinec populace urceny k mutaci.
     */
    static void MutationIvm(Automaton::Chromosome &chromosome);

    /**
     * Mutace typu Displacement mutation (DM).
     * @param chromosome Vybrany jedinec populace urceny k mutaci.
     */
    static void MutationDm(Automaton::Chromosome &chromosome);

    /**
     * Mutace typu Simple inversion mutation (SM).
     * @param chromosome Vybrany jedinec populace urceny k mutaci.
     */
    static void MutationSm(Automaton::Chromosome &chromosome);

    /**
     * Mutace typu (ISM).
     * @param instrList Mnozina dostupnych instrukci.
     * @param chromosome Vybrany jedinec populace urceny k mutaci.
     */
    static void MutationIsm(std::vector<Automaton::Instruction> &instrList, Automaton::Chromosome &chromosome);

    /**
     * Vybere nahodnou instrukci, ktera se jeste nenachazi v danem chromozomu.
     * @param instrList Mnozina dostupnych instrukci.
     * @param chromosome Chromozom, do ktereho bude nahodne vybrana instrukce umistena.
     * @return Nahodne vybrana instrukce.
     */
    static Automaton::Instruction* ChooseRandomInstruction(std::vector<Automaton::Instruction> &instrList, Automaton::Chromosome &chromosome);
};

#endif // RECOMBINATION_OPERATOR_H_INCLUDED
