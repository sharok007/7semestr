#include <iostream>
#include <fstream>
#include <ctime>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <string>
#include <iterator>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace pt = boost::property_tree;

using namespace std;

typedef struct{
    int countProcess; //количество процессов
    size_t sizeFile; //размер файла
    string wordFileName; //название файла с рандомными словами
    string outFileName; //название файла с результатами выполнения программы
} config;

void generateFileNames(int countProcess, string fileName, char fileNames[][64]);
int generateRandomWord(size_t sizeFile, int countProcess, char fileNames[][64]);
void parsConfig(string configFileName, config &configFile);
int writeWords(char wordFileName[], string str);
int writeResult(char resultFileName[], clock_t time, int count, int pid, int ppid);
int countTheWords(char wordFileName[], int &nCount, clock_t &time);

int main()
{
    srand(time(NULL));

    config configFile; //структура конфига
    clock_t time; //время подсчёта слов в фале
    int count = 0; //количество слов в файле

    //парсим конфиг
    parsConfig("config.xml", configFile);

    //генерируем имена файлов с рандомными словами
    char filesWords[configFile.countProcess][64];
    generateFileNames(configFile.countProcess, configFile.wordFileName, filesWords);

    //генерируем имена файлов с результатами
    char filesResult[configFile.countProcess][64];
    generateFileNames(configFile.countProcess, configFile.outFileName, filesResult);

    //генерируем и записываем в файлы рандомные слова
    generateRandomWord(configFile.sizeFile, configFile.countProcess, filesWords);

    for(int i = 0; i < configFile.countProcess; ++i){
        pid_t pid = fork();
        if(pid < 0){
            cout << "Ошибка при создании процесса" << endl;
            exit(-1);
        }
        else if(pid == 0){
            countTheWords(filesWords[i], count, time);
            writeResult(filesResult[i], time, count, getpid(), getppid());
            exit(0);
        }
    }

    for(int proc = 0; proc < configFile.countProcess; ++proc){
        int status;
        wait(&status);
    }

    return 0;
}

void generateFileNames(int countProcess, string fileName, char fileNames[][64]){
    for(int i = 0; i < countProcess; ++i){
        snprintf(fileNames[i], 63, "%s%d.txt", fileName.c_str(), i);
    }

}

int generateRandomWord(size_t sizeFile, int countProcess, char fileNames[][64]){
    size_t length = rand() % sizeFile + 1;
    char str[length];
    str[length - 1] = '\0';
    int symbol;
    for(int j = 0; j < countProcess; ++j){
        for(size_t i = 0; i < length-1; ++i) {
            for(;;) {
                symbol = rand() % 57 + 65;
                if((symbol >= 65 && symbol <= 90) || (symbol >= 97 && symbol <= 122)) {
                    str[i] = (char)symbol;
                    break;
                }
                else{
                    str[i] = ' ';
                    break;
                }
            }
        }
       writeWords(fileNames[j], string(str));
    }
    return 0;
}

void parsConfig(string configFileName, config &configFile){
    pt::ptree parserTree;
    pt::read_xml(configFileName, parserTree);
    configFile.wordFileName = parserTree.get<string>("config.wordFile");
    configFile.outFileName = parserTree.get<string>("config.fileOut");
    configFile.sizeFile = parserTree.get<size_t>("config.sizeFile", 100);
    configFile.countProcess = parserTree.get("config.countProcess", 5);
}

int countTheWords(char wordFileName[], int &nCount, clock_t &time){
    clock_t startTime = clock();
    ifstream fileWords(wordFileName, ios::out);
    if(!fileWords.is_open()){
        cout << "Файл не открыт" << endl;
        return -1;
    }
    nCount = distance(istream_iterator<string>(fileWords),
                      istream_iterator<string>());
    fileWords.close();
    clock_t endTime = clock();
    time = endTime - startTime;
    return 0;
}

int writeWords(char wordFileName[], string str){
        ofstream fileWords(wordFileName, ios::out);
        if(!fileWords.is_open()){
            cout << "Файл " << wordFileName << " не открыт" << endl;
            return -1;
        }
        fileWords << str;
        fileWords.close();
    return 0;
}

int writeResult(char resultFileName[], clock_t time, int count, int pid, int ppid){
    ofstream fileWords(resultFileName, ios::out);
    if(!fileWords.is_open()){
        cout << "Файл " << resultFileName << " не открыт" << endl;
        return -1;
    }
    fileWords << "TIME: " << (float)time/CLOCKS_PER_SEC << " sec" << "\n";
    fileWords << "COUNT: " << count << "\n";
    fileWords << "PID: " << pid << "\n";
    fileWords << "PPID: " << ppid << "\n";
    fileWords.close();
    return 0;
}
