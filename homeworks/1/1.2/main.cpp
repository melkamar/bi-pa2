/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on March 9, 2013, 1:45 PM
 */

#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

#define MEMORY_LIMIT 200000

#define CHUNK_FAIL 0 // function failed, return false
#define CHUNK_EOF 1 // end of file reached, file has been parsed, continue with another task
#define CHUNK_MEMORY 2 // not enough memory, start a new file

#define RECORD_DEFAULT_NAME "<N/A>"
#define RECORD_DEFAULT_GRADE -1

struct Record {
    string name;
    int grade;

    Record() {
        name = RECORD_DEFAULT_NAME;
        grade = RECORD_DEFAULT_GRADE;
    }

    Record(string n, int g) : name(n), grade(g) {
    }

    bool operator<(const Record & other) const {
        return name < other.name;
    }
};

bool openFileOut(ofstream &file, const char* FN) {
    file.open(FN, ios::out);

    if (!file.good()) {
        return false;
    }
    return true;
}

bool openFileIn(ifstream &file, const char* FN) {
    file.open(FN, ios::in);

    if (!file.good()) {
        return false;
    }
    return true;
}

string getFN(const char* orig, int ID) {
    ostringstream outFN;
    outFN << orig << "_" << setfill('0') << setw(4) << ID;
    return outFN.str();
}

bool isNumber(const string &str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        if (!isdigit(str.c_str()[i])) return false;
    }
    return true;
}

bool parseLine(string line, Record*& rec) {
    string name, numstr, fin;
    int num;
    name = RECORD_DEFAULT_NAME;
    num = RECORD_DEFAULT_GRADE;
    istringstream lineStream(line);

    rec = new Record();
    if (line.length() == 0) {
        rec->grade = RECORD_DEFAULT_GRADE;
        rec->name = RECORD_DEFAULT_NAME;
        return true;
    }

    lineStream >> name;
    lineStream >> numstr;
    lineStream >> fin;

    if (numstr.length() == 0 || !isNumber(numstr)) {
        return false;
    } else num = atoi(numstr.c_str());

    if (fin.length() != 0) {
        return false;
    }

    //    

    if ((!lineStream.good() && !lineStream.eof())) {
        return false;
    }
    rec->name = name;
    rec->grade = num;

    return true;
}

int processChunk(ifstream & file, const char * FN, int fileID) {
    int bytesWritten = 0;
    string line, newName;
    ofstream out;

    vector<Record> list;
    newName = getFN(FN, fileID);

    if (!openFileOut(out, newName.c_str())) {
        return CHUNK_FAIL;
    }


    while (bytesWritten < MEMORY_LIMIT && (getline(file, line), (file.good()) || !line.empty())) {

        Record* rec;
        if (!parseLine(line, rec)) {
            delete rec;
            out.close();
            return CHUNK_FAIL;
        } else if (rec->name == RECORD_DEFAULT_NAME) {
            out.close();
            delete rec;
            return CHUNK_EOF;
        } else {
            list.push_back(*rec);
            bytesWritten += (rec->name.size()) * sizeof (char) + sizeof (int);
            rec->name = RECORD_DEFAULT_NAME;
            rec->grade = RECORD_DEFAULT_GRADE;
            delete rec;
        }
        line = "";
    }

    sort(list.begin(), list.end());
    for (int unsigned i = 0; i < list.size(); i++) {
        out << list[i].name << " " << list[i].grade << endl;
    }

    if (bytesWritten >= MEMORY_LIMIT) {
        out.close();
        return CHUNK_MEMORY;
    } else if (file.eof()) {
        out.close();
        return CHUNK_EOF;
    } else {
        out.close();
        return CHUNK_FAIL;
    }
}

int findMin(Record **items, int size) {
    int minIndex = -1;
    Record *min;

    for (int i = 0; i < size; i++) {
        if ((minIndex == -1 || *(items[i])<*min) && items[i]->name.compare(RECORD_DEFAULT_NAME) != 0) {
            minIndex = i;
            min = items[i];
        }
    }

    return minIndex;
}

bool filesEmpty(ifstream *files, int size) {
    for (int i = 0; i < size; i++) {
        if (!files[i].eof()) {
            return false;
        }
    }
    return true;
}

bool mergeFiles(ifstream *files, int size, const char *outFN) {
    Record ** items = new Record*[size];

    string line;
    for (int i = 0; i < size; i++) {
        if (!getline(files[i], line)) {
        }

        if (!parseLine(line, items[i])) {
            for (int k = 0; k < size; k++)
                delete items[k];
            delete [] items;
            return false;
        }
    }

    ofstream out;
    if (!openFileOut(out, outFN)) {
        delete [] items;
        return false;
    }

    while (!filesEmpty(files, size)) {
        int min = findMin(items, size);
        out << items[min]->name << " " << items[min]->grade << endl;
        items[min]->name = RECORD_DEFAULT_NAME;
        items[min]->grade = RECORD_DEFAULT_GRADE;

        if (!getline(files[min], line)) {
            if (files[min].eof()) continue;
            else {
                for (int k = 0; k < size; k++)
                    if (k != min) delete items[k];
                delete [] items;
                return false;
            }
        }

        delete items[min];
        if (!parseLine(line, items[min])) {
            for (int k = 0; k < size; k++)
                if (k != min) delete items[k];
            delete [] items;
            return false;
        }
    }

    for (int k = 0; k < size; k++)
        delete items[k];
    delete[]items;

    out.close();
    return true;

}

bool combineFiles(const char *FN) {
    Record ** items = new Record*[2];
    ifstream files[2];

    for (int i = 0; i < 2; i++) {
        if (!openFileIn(files[i], getFN(FN, i).c_str())) {
            files[i].close();
            return false;
        }
    }
    string line;
    for (int i = 0; i < 2; i++) {
        if (!getline(files[i], line)) {
        }

        if (!parseLine(line, items[i])) {
            delete items[0];
            delete items[1];
            delete [] items;
            return false;
        }
    }

    ofstream out;
    if (!openFileOut(out, FN)) return false;

    Record * prevItems[2];
    prevItems[0] = new Record();
    prevItems[1] = new Record();

    while (!filesEmpty(files, 2)) {
        if (items[0]->name.compare(items[1]->name) == 0) { // Records match, add val
            out << items[0]->name << " " << items[0]->grade << " " << items[1]->grade << endl;

            for (int i = 0; i < 2; i++) {
                delete prevItems[i];
                prevItems[i] = items[i];

                if (!getline(files[i], line)) {
                }

                if (!parseLine(line, items[i])) {
                    delete prevItems[0];
                    delete prevItems[1];
                    delete items[0];
                    delete items[1];
                    delete[]items;
                    return false;
                }

                if (prevItems[i]->name.compare(items[i]->name) == 0) {
                    delete prevItems[0];
                    delete prevItems[1];
                    delete items[0];
                    delete items[1];
                    delete[]items;
                    return false;
                }
            }
        } else {

            int min = findMin(items, 2);

            if (min == 0) {
                out << items[0]->name << " " << items[0]->grade << " 0" << endl;
            } else {
                out << items[1]->name << " 0 " << items[1]->grade << endl;
            }

            delete prevItems[min];
            prevItems[min] = items[min];

            if (!getline(files[min], line)) {
            }

            if (!parseLine(line, items[min])) {
                delete prevItems[0];
                delete prevItems[1];
                delete items[0];
                delete items[1];
                delete [] items;
                return false;
            }

            if (prevItems[min]->name.compare(items[min]->name) == 0) {
                delete prevItems[0];
                delete prevItems[1];
                delete items[0];
                delete items[1];
                delete [] items;
                return false;
            }
        }
    }

    delete prevItems[0];
    delete prevItems[1];
    delete items[0];
    delete items[1];
    delete [] items;

    out.close();
    return true;
}

void cleanFiles(const char **fileNames, int *filesCounts, bool evenRes) {

    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < filesCounts[k]; i++) {
            remove(getFN(fileNames[k], i).c_str());
        }
        remove(getFN(fileNames[2], k).c_str());
    }

    if (evenRes) remove(fileNames[2]);
}

bool joinFiles(const char * inFile1,
        const char * inFile2,
        const char * outFile) {

    const char * fileNames [3];
    fileNames[0] = inFile1;
    fileNames[1] = inFile2;
    fileNames[2] = outFile;

    int fileCounts[2];
    fileCounts[0] = 0;
    fileCounts[1] = 0;



    for (int n = 0; n < 2; n++) {
        // Open input file
        ifstream file;
        if (!openFileIn(file, fileNames[n])) {
            file.close();
            return false;
        }

        /* Split input file to chunks and sort them
         * to use merge sort later.
         * 
         * CHUNK_MEMORY indicates there was not enough memory and a new file has been created
         *       which means the input file needs to be kept processed.
         * 
         * CHUNK_EOF indicates end of input file, which means it has been processed
         * 
         * CHUNK_FAIL indicates a non-fixable error with the filestream, input data etc (return FALSE)
         * */

        int res = CHUNK_MEMORY;
        int files = 0; // number of created files
        while (res == CHUNK_MEMORY) {
            res = processChunk(file, fileNames[n], files++);
        }



        fileCounts[n] = files;
        if (res == CHUNK_FAIL) {
            cleanFiles(fileNames, fileCounts, true);
            return false;
        }

        /* MergeSort part, files created above will have fstreams opened (toMerge[])
         * and then added in one file: <outFile>_0000 and <outFile>_0001
         */

        ifstream *toMerge = new ifstream[files];

        for (int i = 0; i < files; i++) {
            if (!openFileIn(toMerge[i], getFN(fileNames[n], i).c_str())) {
                toMerge[i].close();
                delete[] toMerge;
                cleanFiles(fileNames, fileCounts, true);
                //                cout << "openfile fault - toMerge - filename: \"" << getFN(fileNames[n], i).c_str() << "\"" << endl;
                return false;
            }
        }

        if (!mergeFiles(toMerge, files, getFN(outFile, n).c_str())) {
            delete[] toMerge;
            cleanFiles(fileNames, fileCounts, true);
            //            cout << "mergeFiles fault" << endl;
            return false;
        }

        delete[] toMerge;
    }


    if (!combineFiles(outFile)) {
        cleanFiles(fileNames, fileCounts, true);
        return false;
    }

    cleanFiles(fileNames, fileCounts, false);
    return true;
}

#ifndef __PROGTEST__


int main(int argc, char * argv []) {


    //    cout << "Starting up!" << endl;
    //    cout << "Memory limit is " << MEMORY_LIMIT << " B" << endl << endl;

    bool res;

    //    res = joinFiles("/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inA_0.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inB_0.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/mout_0.txt");
    //    cout << "____________________________________ RES0 = " << boolalpha << res << endl << endl;
    //
    //    res = joinFiles("/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inA_1.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inB_1.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/mout_1.txt");
    //    cout << "____________________________________ RES1 = " << boolalpha << res << endl << endl;
    //
    //        res = joinFiles("/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inA_2.txt",
    //                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inB_2.txt",
    //                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/mout_2.txt");
    //        cout << "____________________________________ RES2 = " << boolalpha << res << endl << endl;
    //
    //    res = joinFiles("/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inA_3.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inB_3.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/mout_3.txt");
    //    cout << "____________________________________ RES3 = " << boolalpha << res << endl << endl;
    //
    //    res = joinFiles("/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inA_4.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/inB_4.txt",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/mout_4.txt");
    //    cout << "____________________________________ RES4 = " << boolalpha << res << endl << endl;

    res = joinFiles("/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/A.txt",
            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/B.txt",
            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-2-spojovani/dist/Debug/GNU-Linux-x86/sample/mOUT.txt");
    cout << "____________________________________ RES0 = " << boolalpha << res << endl << endl;



    if (!res) {
        //        cout << endl << endl << "FALSE!" << endl;
        return 1;
    } else {
        //        cout << endl << endl << "TRUE!" << endl;
        return 0;
    }
}
#endif /* __PROGTEST__ */
