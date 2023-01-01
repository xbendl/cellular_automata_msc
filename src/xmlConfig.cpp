#include "xmlConfig.h"

using namespace std;

XmlConfig::XmlConfig()
{
    configDoc = NULL;
}

XmlConfig::~XmlConfig()
{
    if(configDoc != NULL)
        delete configDoc;
}

void XmlConfig::LoadInstructions(std::string configPath, Automaton::InstructionSetVector &instructionList, int &lastId)
{
    // Otevreni XML souboru
    std::string path = QApplication::applicationDirPath().toStdString() + "/" + configPath;
    configDoc = new TiXmlDocument(path);
    if(!configDoc->LoadFile())
    {
        cerr << "CHYBA! Odkazovany XML soubor pro analyzu chromozomu s instrukcemi nebyl nalezen.";
        exit(EXIT_FAILURE);
    }

    // Pruchod XML souborem
    int tempVar;
    int actOutput;
    TiXmlNode *taskXmlNode = NULL;
    TiXmlNode *neighbourhoodXmlNode = NULL;
    TiXmlNode *instrXmlNode = NULL;
    TiXmlNode *rootXmlNode = configDoc->FirstChild()->NextSibling();
    Automaton::InstructionSet instrSet;
    Automaton::Instruction instr;
    std::vector<Automaton::Instruction> instrVector;

    TiXmlNode *lastIdXmlNode = rootXmlNode->FirstChild();
    lastIdXmlNode->ToElement()->QueryIntAttribute("lastId", &(lastId));

    // Pruchod pres ulohy
    for(taskXmlNode = lastIdXmlNode->NextSibling(); taskXmlNode; taskXmlNode = taskXmlNode->NextSibling())
    {
        instrSet.clear();

        // Pruchod pres typy okoli
        for(neighbourhoodXmlNode = taskXmlNode->FirstChild(); neighbourhoodXmlNode; neighbourhoodXmlNode = neighbourhoodXmlNode->NextSibling())
        {
            instrVector.clear();

            // Pruchod pres instrukce
            for(instrXmlNode = neighbourhoodXmlNode->FirstChild(); instrXmlNode; instrXmlNode = instrXmlNode->NextSibling())
            {
                string config;
                instrXmlNode->ToElement()->QueryIntAttribute("id", &(instr.id));
                instrXmlNode->ToElement()->QueryValueAttribute("config", &config);
                instrXmlNode->ToElement()->QueryValueAttribute("name", &(instr.name));
                instrXmlNode->ToElement()->QueryIntAttribute("output", &(actOutput));
                instrXmlNode->ToElement()->QueryIntAttribute("active", &tempVar);
                instr.output = (Automaton::CellType)actOutput;
                instr.active = (bool)tempVar;

                for(int i = 0; i < DIR_COUNT; i++)
                    instr.config[i] = (Automaton::CellType)config[i];

                instrVector.push_back(instr);
            }
            instrSet.push_back(instrVector);
        }
        instructionList.push_back(instrSet);
    }
}

bool XmlConfig::SaveInstructions(std::string configPath, Automaton::InstructionSetVector &instructionList, int lastId)
{
    configDoc = new TiXmlDocument();

    TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");    // Deklarace
    TiXmlElement *rootElement = new TiXmlElement("INSTRUCTION_LIST");       // Korenovy uzel
    TiXmlElement *lastIdElement = new TiXmlElement("ID_GENERATOR");         // Element s poslednim identifikatorem (citac)
    lastIdElement->SetAttribute("lastId", lastId);
    rootElement->LinkEndChild(lastIdElement);

    // Vygenerovani XML s instrukcemi
    string xmlString = "";
    for(int i = 0; i < (int)instructionList.size(); i++)
    {   // Element <TASK id="x">
        TiXmlElement *newTaskElement = new TiXmlElement("TASK");
        newTaskElement->SetAttribute("id", i);

        for(int j = 0; j < (int)instructionList[i].size(); j++)
        {   // Element <NEIGHBOURHOOD type="x">
            TiXmlElement *newNeighbourhoodElement = new TiXmlElement("NEIGHBOURHOOD");
            newNeighbourhoodElement->SetAttribute("type", j);

            vector<Automaton::Instruction>::iterator it;
            for(it = (instructionList[i][j]).begin(); it != (instructionList[i][j]).end(); it++)
            {   // Element <INSTRUCTION id="x" configuration="x" output="x" name="x">
                string config;
                for(int k = 0; k < DIR_COUNT; k++)
                {
                    stringstream ss;
                    ss << ((*it).config)[k];
                    config += ss.str();
                }
                TiXmlElement *newInstructionElement = new TiXmlElement("INSTRUCTION");
                newInstructionElement->SetAttribute("id", ((*it).id));
                newInstructionElement->SetAttribute("config", config);
                newInstructionElement->SetAttribute("output", ((*it).output));
                newInstructionElement->SetAttribute("name", ((*it).name));
                newInstructionElement->SetAttribute("active", ((*it).active));

                newNeighbourhoodElement->LinkEndChild(newInstructionElement);
            }
            newTaskElement->LinkEndChild(newNeighbourhoodElement);
        }
        rootElement->LinkEndChild(newTaskElement);
    }

    configDoc->LinkEndChild(declaration);
    configDoc->LinkEndChild(rootElement);

    if(!configDoc->SaveFile(configPath))  // Ulozeni vytvorene struktury
        return false;

    return true;
}
