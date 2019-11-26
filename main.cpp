#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>

namespace pt = boost::property_tree;

using namespace std;

int main()
{
    char buffer[128];
    pt::ptree parserTree;
    pt::read_xml("/home/lotesadmin/Документы/testBoostXMLParser/config.xml", parserTree);
    string outFile;
    string inputFile;
    int sizeFile;
    int countProcess;
    inputFile = parserTree.get<string>("config.fileIn");
    outFile = parserTree.get<string>("config.fileOut");
    sizeFile = parserTree.get("config.sizeFile", 100);
    countProcess = parserTree.get("config.countProcess", 5);
    for(int i = 0; i < countProcess; ++i){
        snprintf(buffer, sizeof(i)+sizeof(inputFile), "%s%d.txt", inputFile.c_str(), i);
        cout << buffer << endl;
    }
    return 0;
}
