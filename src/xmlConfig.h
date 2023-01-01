/**
* \file xmlConfig.h
* \brief Komunikacni rozhrani pro praci s konfiguracnim XML souborem.
* \author Bendl Jaroslav (xbendl00)
*/

#ifndef XMLCONFIG_H_INCLUDED
#define XMLCONFIG_H_INCLUDED
#include <stdlib.h>
#include <vector>
#include <qapplication.h>
#include "automaton.h"
#include "tinyxml/tinyxml.h"

/// Spravce konfiguracniho nastaveni XML.
class XmlConfig
{
    private:
    TiXmlDocument *configDoc;

    public:
    XmlConfig();
    ~XmlConfig();

    /**
     * Vytvori novy XML dokument z vektoru instrukci.
     * @param configPath Cesta k ulozeni noveho XML dokumentu.
     * @param instructionList Vektor s posloupnostmi instrukci.
     * @param lastId Posledni pouzity identifikator instrukce v XML (dulezite pro generovani noveho identifikatoru).
     * @return True, pokud se vytvoreni dokumentu podarilo, jinak false.
     */
    bool SaveInstructions(std::string configPath, Automaton::InstructionSetVector &instructionList, int lastId);

    /**
     * Ziska instrukce z XML dokumentu.
     * @param configPath Cesta k nacteni pozadovaneho XML dokumentu.
     * @param instructionList Vektor s posloupnostmi instrukci.
     * @param lastId Posledni pouzity identifikator instrukce v XML (dulezite pro generovani noveho identifikatoru).
     * @return True, pokud se nacteni instrukci, podarilo, jinak false.
     */
    void LoadInstructions(std::string configPath, Automaton::InstructionSetVector &instructionList, int &lastId);
};

#endif // XMLCONFIG_H_INCLUDED
