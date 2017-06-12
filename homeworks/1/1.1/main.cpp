/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on March 13, 2013, 9:51 PM
 */

#ifndef  __PROGTEST__ 
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <stdint.h>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

#define FIELD_FAIL -1
#define FIELD_INT 0
#define FIELD_STR 1

#define LENGTH_HEADER_FILE 13
#define LENGTH_HEADER_RECORD 5

bool verbose = false;

bool openFileOut(ofstream &file, const char* FN) {
    file.open(FN, ios::out | ios::binary);

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

void cleanup(const char* FN) {

    remove(FN);
    //  todo
}

bool readFileHeader(int &fileSize, int &rows, int &cols, ifstream &in) {

    if (verbose) cout << "Reading file header." << endl;

    char tmp;
    in.read((char*) &tmp, 1);
    if (verbose) cout << "  CHAR: " << tmp << endl;
    if (tmp != 'H') return false;

    in.read((char*) &fileSize, 4);
    in.read((char*) &rows, 4);
    in.read((char*) &cols, 4);

    if (verbose) cout << "  fileSize=" << fileSize << endl;
    if (verbose) cout << "      rows=" << rows << endl;
    if (verbose) cout << "      cols=" << cols << endl;

    return in.good();
}

bool readRecordHeader(int &recLength, int &bytesRead, ifstream &in) {

    if (verbose) cout << "Reading record header." << endl;

    char tmp;
    in.read((char*) &tmp, 1);
    if (verbose) cout << "  CHAR: " << tmp << endl;
    if (tmp != 'R') return false;

    in.read((char*) &recLength, 4);
    if (verbose) cout << "  recLength=" << recLength << endl;
    bytesRead += 5;

    return in.good();
}

int readField(int &num, string &str, int &bytesRead, int &recBytes, ifstream &in) {

    if (verbose) cout << "Reading field." << endl;

    char tmp;
    in.read((char*) &tmp, 1);

    if (verbose) cout << "  CHAR: " << tmp << endl;

    if (tmp == 'I') {
        in.read((char*) &num, 4);
        bytesRead += 5;
        recBytes += 5;

        if (verbose) cout << "  Number=" << num << endl;

        if (in.good()) return FIELD_INT;
        else return FIELD_FAIL;

    } else if (tmp == 'S') {
        int len;
        in.read((char*) &len, 4);

        bytesRead += (1 + 4 + len);
        recBytes += (1 + 4 + len);

        stringstream buf;

        for (int i = 0; i < len; i++) {
            in.read((char*) &tmp, 1);
            buf << tmp;
            if (tmp == '"') buf << "\"";
        }

        if (verbose) cout << "String len=" << len << ": [" << buf.str() << "]" << endl;

        str = buf.str();

        if (in.good()) return FIELD_STR;
        else return FIELD_FAIL;

    } else return FIELD_FAIL;
}

bool fieldNeedsQuotes(const string str) {
    char tmp;
    for (int unsigned i = 0; i < str.length(); i++) {
        tmp = str[i];
        if (tmp == '"' || tmp == ';' || tmp == '\n') return true;
    }

    return false;
}

bool BINtoCSV(const char * inFileName, const char * outFileName) {

    ifstream in;
    ofstream out;

    if (!openFileIn(in, inFileName)) {
        return false;
    }

    if (!openFileOut(out, outFileName)) {
        return false;
    }

    int totalSize, rows, cols;

    if (!readFileHeader(totalSize, rows, cols, in)) {
        cleanup(outFileName);
        return false;
    }

    int bytesRead = 0;
    for (int rec = 0; rec < rows; rec++) { // Loop for each record
        stringstream recBuf;
        int recLen;
        int recBytes = 0;
        if (verbose) cout << endl << "rec = " << rec << endl;
        if (!readRecordHeader(recLen, bytesRead, in)) {
            cleanup(outFileName);
            return false;
        }

        for (int field = 0; field < cols; field++) { // Loop for each field in record
            int num;
            string str;
            int res = readField(num, str, bytesRead, recBytes, in);
            switch (res) {
                case FIELD_FAIL:
                    cleanup(outFileName);
                    return false;

                case FIELD_INT:
                    recBuf << num;
                    break;

                case FIELD_STR:
                    if (fieldNeedsQuotes(str)) {
                        //                        if (verbose) cout << "  Field [" << str << "] needs quotes." << endl;
                        recBuf << "\"" << str << "\"";
                    } else {
                        //                        if (verbose) cout << "  Field [" << str << "] does not need quotes." << endl;
                        recBuf << str;
                    }
                    break;
            }
            if (field != cols - 1) {
                recBuf << ";";
            } else recBuf << "\n";
        }

        if (verbose) cout << "Record bytes expected: " << recLen << endl;
        if (verbose) cout << "Record bytes     read: " << recBytes << endl;
        if (recLen != recBytes) {
            cleanup(outFileName);
            return false;
        }

        out << recBuf.str();
    }

    if (verbose) cout << "Bytes read: " << bytesRead << endl;
    if (verbose) cout << "Bytes expected: " << totalSize << endl;

    return (bytesRead == totalSize);
}

bool isNumber(string str) {
    if (str.length() == 0) return false;
    for (unsigned int i = 0; i < str.length(); i++) {
        char c = str.at(i);
        if (!isdigit(c) && c != '-' && c != '+')
            return false;
    }
    return true;
}

/*
 * @param number/text; only one of these will be used, depending on result
 * @param buf; text to parse
 * @return indicates whether to use number or text
 */
int convertField(int& number, string& text, string buf) {
    if (isNumber(buf)) {
        number = atoi(buf.c_str());
        return FIELD_INT;
    } else {
        text = buf;
        return FIELD_STR;
    }
}

bool writeInt(int number, ofstream& out, int& bytesWritten, int &totalBytes) {
    out.write("I", 1);
    out.write((char*) &number, 4);

    bytesWritten += (4 + 1);
    totalBytes += (4 + 1);

    return out.good();
}

bool writeStr(string str, ofstream& out, int& bytesWritten, int& totalBytes) {
    out.write("S", 1);
    int ln = str.size();
    out.write((char*) &ln, 4);
    out.write(str.c_str(), ln);

    bytesWritten += (1 + 4 + ln);
    totalBytes += (1 + 4 + ln);

    return out.good();
}

bool writeRecordHeader(int &recLength, ofstream& out, int &totalBytes, int &pos) {

    //if (verbose) cout << "    Writing record header to pos " << pos << endl;

    out.seekp(pos);

    out.write("R", 1);
    out.write((char*) &recLength, 4);

    totalBytes += (1 + 4);

    out.seekp(0, ios_base::end);
    pos = out.tellp();

    recLength = 0;

    return out.good();
}

bool writeDummyData(int length, ofstream &out) {
    stringstream st;
    st << setw(length) << setfill('X') << "X";
    //if (verbose) cout << "Dummy writing @" << out.tellp() << "  len=" << length << ": " << st.str() << endl;

    out.write(st.str().c_str(), length);

    return out.good();
}

bool writeField(stringstream &buf, ofstream& out, int& bytesWritten, int& totalBytes) {

    //if (verbose) cout << "    Writing field @" << out.tellp() << "  [" << buf.str() << "]" << endl;

    bool res;

    int num;
    string str;
    int type = convertField(num, str, buf.str());

    if (type == FIELD_INT) {
        res = writeInt(num, out, bytesWritten, totalBytes);
    } else {
        res = writeStr(str, out, bytesWritten, totalBytes);
    }

    buf.str("");
    buf.clear();

    return res;
}

bool writeFileHeader(int totalBytes, int cols, int rows, ofstream& out) {
    //if (verbose) cout << "    Writing header:" << endl;
    //if (verbose) cout << "      totalBytes = " << totalBytes << endl;
    //if (verbose) cout << "            cols = " << cols << endl;
    //if (verbose) cout << "            rows = " << rows << endl;

    out.seekp(0);

    out.write("H", 1);
    out.write((char*) &totalBytes, 4);
    out.write((char*) &rows, 4);
    out.write((char*) &cols, 4);

    return out.good();
}

bool checkCols(int &cols, int &thisCols) {
    if (cols == -1) cols = thisCols;

    if (cols != thisCols) {
        //if (verbose) cout << "ERROR!" << endl << "   Number of columns does not match. (cols=" << cols << ", thisCols=" << thisCols << ")" << endl;
        return false;
    }

    return true;

}

bool CSVtoBIN(const char * inFileName, const char * outFileName) {
    ifstream in;
    ofstream out;

    if (!openFileIn(in, inFileName)) {
        return false;
    }

    if (!openFileOut(out, outFileName)) {
        return false;
    }

    stringstream buffer;
    bool insideQuotes = false;


    int cols = -1; // Columns in file
    int rows = 0;
    int thisCols = 0; // Columns in a processed record

    int pos = 0; // position in file
    int bytesWritten = 0; // number of bytes written by this record
    int totalBytes = 0;

    bool newRecord = true;

    writeDummyData(LENGTH_HEADER_FILE, out);
    pos = out.tellp();

    char tmp;
    while (tmp = in.get(), in.good()) {

        if (newRecord) {
            writeDummyData(LENGTH_HEADER_RECORD, out);
            newRecord = false;
        }

        //string cstr;
        // if (tmp == '\n') cstr = "/n";
        //else cstr = tmp;
        //int cn = tmp;
        //if (verbose) cout << "  New char [" << cstr << "]" << "  (" << cn << ")" << endl;

        switch (tmp) {
            case -1:
                //if (verbose) cout << "-1 has arrived!" << endl;
                break;

            case '"': // Need to decide if special char, or "" for normal char
                if (in.peek() == '"') { // first quote of ""
                    if (!insideQuotes) { // if not inside quoteblock, not valid
                        cleanup(outFileName);
                        return false;
                    } else { // is inside quoteblock, print "
                        buffer << "\"";
                        in.get(); // remove second "
                    }
                } else { // only single quote, toggle quoteblock
                    insideQuotes = !insideQuotes;
                }
                break;

            case ';':
                if (insideQuotes) { // if inside quoteblock, only print ;
                    buffer << ";";
                } else { // else write field to file
                    if (!writeField(buffer, out, bytesWritten, totalBytes)) {
                        cleanup(outFileName);
                        return false;
                    }

                    thisCols++;
                }

                break;

            case '\n':
                if (insideQuotes) {
                    buffer << "\n";
                } else { // end of record!
                    if (!writeField(buffer, out, bytesWritten, totalBytes)) {
                        cleanup(outFileName);
                        return false;
                    }
                    thisCols++;

                    writeRecordHeader(bytesWritten, out, totalBytes, pos);

                    newRecord = true;

                    if (!checkCols(cols, thisCols)) {
                        cleanup(outFileName);
                        return false;
                    }

                    rows++;

                    thisCols = 0;
                }
                break;
            default: buffer << tmp;
        }
    }

    if (in.eof()) {

        if (!buffer.str().empty()) {
            if (!writeField(buffer, out, bytesWritten, totalBytes)) {
                cleanup(outFileName);
                return false;
            }
            thisCols++;

            writeRecordHeader(bytesWritten, out, totalBytes, pos);

            checkCols(cols, thisCols);

            rows++;
        }

        writeFileHeader(totalBytes, cols, rows, out);
    } else {
        cleanup(outFileName);
        return false;
    }



    return true;
}

#ifndef __PROGTEST__

int main() {

        if (CSVtoBIN("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_0.csv",
                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/mout_0.bin"))
             cout << "RETURN TRUE" << endl;
        else  cout << "RETURN FALSE" << endl;
    
        if (CSVtoBIN("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_1.csv",
                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/mout_1.bin"))
             cout << "RETURN TRUE" << endl;
        else  cout << "RETURN FALSE" << endl;
    
        if (CSVtoBIN("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_2.csv",
                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/mout_2.bin"))
             cout << "RETURN TRUE" << endl;
        else  cout << "RETURN FALSE" << endl;
    
        if (CSVtoBIN("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_3.csv",
                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/mout_3.bin"))
             cout << "RETURN TRUE" << endl;
        else  cout << "RETURN FALSE" << endl;
    
        if (!BINtoCSV("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_4.bin",
                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/mout_4.csv"))
             cout << "RETURNING FALSE!" << endl;
        else  cout << "RETURNING TRUE!" << endl;
        
        if (!BINtoCSV("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_5.bin",
                "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/mout_5.csv"))
             cout << "RETURNING FALSE!" << endl;
        else  cout << "RETURNING TRUE!" << endl;

    //    if (CSVtoBIN("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/hint0.csv",
    //            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/hint0_out.bin"))
    //        if (verbose) cout << "RETURN TRUE" << endl;
    //    else if (verbose) cout << "RETURN FALSE" << endl;

//    if (!BINtoCSV("/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/in_Nespravny_vstup_(bin)_01.bin",
//            "/home/magicmaster/skola/PA2/Domácí-úkoly/1-1-BinToCSV/dist/Debug/GNU-Linux-x86/sample/hint0_fin.csv")) {
//         cout << "RETURNING FALSE!" << endl;
//    } else  cout << "RETURNING TRUE!" << endl;

    return 0;
}

#endif