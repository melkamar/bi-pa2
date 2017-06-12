/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on March 18, 2013, 11:27 AM
 */

#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
using namespace std;
#endif /* __PROGTEST__ */

#define NOT_FOUND -1
#define ARRAY_STARTSIZE 1

#define ENTRY_LESSER -1
#define ENTRY_EQUALS 0
#define ENTRY_GREATER 1

struct Entry {
    string oName, oAddr;
    string cName, cAddr;

    /*
    bool operator<(const Entry & other) {
        if (oName < other.oName) return true;
        else if (oName == other.oName) {
            if (oAddr < other.oAddr) return true;
        }

        return false;
    }

    bool operator==(const Entry & other) {
        return (oName == other.oName && oAddr == other.oAddr);
    }
     * */

    int compareTo(string cName, string cAddr) {
        if (oName < cName) { // item in lower part
            return ENTRY_LESSER;
        } else if (oName > cName) {
            return ENTRY_GREATER;
        } else if (oAddr < cAddr) {
            return ENTRY_LESSER;
        } else if (oAddr > cAddr) {
            return ENTRY_GREATER;
        } else {
            //            cout << "ENTRY: " << oName << "==" << cName << endl;
            return ENTRY_EQUALS;
        };
    }
};

class CCompanyIndex {
public:
    CCompanyIndex(void);
    ~CCompanyIndex(void);
    bool Add(const string & oName,
            const string & oAddr,
            const string & cName,
            const string & cAddr);
    bool Del(const string & oName,
            const string & oAddr);
    bool Search(const string & oName,
            const string & oAddr,
            string & cName,
            string & cAddr) const;

    void print();
private:
    Entry * items;
    int count; // Number of records in db
    int size; // allocated array size; size==count -> realloc

    void resizeArray(); // resizes array to 1.5x previous size
    int binarySearch(const string &oName, const string &oAddr, int min, int max) const;
    int binaryGetIndex(const string &oName, const string &oAddr, int min, int max) const;
    int getAddIndex(const string &oName, const string &oAddr) const;
};

CCompanyIndex::CCompanyIndex() {
    count = 0;
    size = ARRAY_STARTSIZE;
    items = new Entry[size];
}

CCompanyIndex::~CCompanyIndex() {
    delete [] items;
}

bool CCompanyIndex::Add(const string& oName, const string& oAddr, const string& cName, const string& cAddr) {

    //    cout << "---- ADD " << oName << ", " << oAddr << endl;

//    cout << "Adding "<<oName<<"..."<<endl;
    // Check if entry already here
    if (binarySearch(oName, oAddr, 0, count - 1) != NOT_FOUND) {
        //        cout << "Item already here" << endl;
        return false;
    } else {
        int tmp;
        tmp = getAddIndex(oName, oAddr);
        //        cout << "ADD: [" << oName << ", " << oAddr << "] to be put in " << tmp << endl;
        if (count + 1 == size) {
            resizeArray();
        }

        for (int i = count - 1; i >= tmp; i--) {
            items[i + 1].oName = items[i].oName;
            items[i + 1].oAddr = items[i].oAddr;
            items[i + 1].cName = items[i].cName;
            items[i + 1].cAddr = items[i].cAddr;
        }

        items[tmp].oName = oName;
        items[tmp].oAddr = oAddr;
        items[tmp].cName = cName;
        items[tmp].cAddr = cAddr;

        count++;

        return true;
    }
}

bool CCompanyIndex::Del(const string& oName, const string & oAddr) {
    if (binarySearch(oName, oAddr, 0, count - 1) == NOT_FOUND) return false;

    int pos = getAddIndex(oName, oAddr);
    for (int i = pos; i < count - 2; i++)
        items[i] = items[i + 1];

    count--;

    return true;
}

bool CCompanyIndex::Search(const string& oName, const string& oAddr, string& cName, string & cAddr) const {
    //    cout << "Search!" << endl;
    //    cout << "  " << oName << ", " << oAddr << endl;
    int tmp = binarySearch(oName, oAddr, 0, count - 1);
    if (tmp != NOT_FOUND) {
        //        cout << "TMP==" << tmp << endl;
        cName = items[tmp].cName;
        cAddr = items[tmp].cAddr;
        return true;
    } else return false;
}

int CCompanyIndex::binarySearch(const string& oName, const string& oAddr, int min, int max) const {
    //    cout << "  binarySearch, " << min << "-" << max << endl;

    if (min > max) { // key not found
        //        cout << "  NOT_FOUND: min=" << min << " max=" << max << endl;
        return NOT_FOUND;
    }


    int mid = (max + min) / 2;
    //    cout << "mid == "<<mid<<endl;
    switch (items[mid].compareTo(oName, oAddr)) {
        case ENTRY_GREATER:
            //            cout << "  " << oName << "in " << min << " - " << mid - 1 << endl;
            return binarySearch(oName, oAddr, min, mid - 1);

        case ENTRY_EQUALS:
            //            cout << "equal on i=" << mid << endl;
            return mid;

        case ENTRY_LESSER:
            //            cout << "  " << oName << "in " << mid + 1 << " - " << max << endl;
            return binarySearch(oName, oAddr, mid + 1, max);
    }

    return -1;
}

int CCompanyIndex::binaryGetIndex(const string& oName, const string& oAddr, int min, int max) const {

//    cout << "binaryGetIndex [" << oName << ", " << oAddr << "]" << endl;
//    cout << "  [" << min << " :: " << max << "]" << endl;

    if (min >= max || max < 0) {
        int ret;
        if (count>0 && items[min].compareTo(oName, oAddr)==ENTRY_LESSER)
            ret=min+1; else ret = min;
//        cout << "  ret1: "<<ret<<endl;
        return ret;
    }

    int mid = (max + min) / 2;
//        cout << "  mid == "<<mid<<endl;
    switch (items[mid].compareTo(oName, oAddr)) {
        case ENTRY_GREATER:
//            cout << "  " << oName << " in " << min << " :: " << mid - 1 << endl;
            return binaryGetIndex(oName, oAddr, min, mid - 1);

        case ENTRY_EQUALS:
            //            cout << "equal on i=" << mid << endl;
            return mid;

        case ENTRY_LESSER:
//            cout << "  " << oName <<  " in " << mid + 1 << " :: " << max << endl;
            return binaryGetIndex(oName, oAddr, mid + 1, max);
    }

    return -10;
}

void CCompanyIndex::resizeArray() {

    int newSize = (size * 2);
    Entry * tmp = new Entry[newSize];
    size = newSize;

    for (int i = 0; i < count; i++) {
        tmp[i].oName = items[i].oName;
        tmp[i].oAddr = items[i].oAddr;
        tmp[i].cName = items[i].cName;
        tmp[i].cAddr = items[i].cAddr;
    }

    //memcpy(tmp, items, count * sizeof (Entry));

    //    cout << "RESIZING ARRAY TO " << newSize << endl;

    delete [] items;
    items = tmp;
}

void CCompanyIndex::print() {
    cout << "Printing..." << endl;
    for (int i = 0; i < count; i++)
        cout << "    (" << i << ") " << items[i].oName << ", " << items[i].oAddr << " -- " << items[i].cName << ", " << items[i].cAddr << endl;
}

int CCompanyIndex::getAddIndex(const string& oName, const string& oAddr) const {
    
    return binaryGetIndex(oName, oAddr, 0, count-1);
}

/*
 * *******************************************************************
 */

#ifndef __PROGTEST__

int main() {

    bool status;
    string cName, cAddress;
    CCompanyIndex b1;


//    cout << "Add1: "<<endl;
//    status = b1 . Add("Smith", "Oak road", "ACME, Ltd.", "One ACME road");
//    cout << status << endl;
//    // status = true
//
//    cout << "Add2: "<<endl;
//    status = b1 . Add("Brown", "Second street", "MakroHard, Inc.", "Soft street");
//    // status = true
//    cout << status << endl;
//    //
//    cout << "Add3: "<<endl;
//    status = b1 . Add("Hacker", "5-th avenue", "Forks and Knives, Ltd.", "Cutlery avenue");
//    // status = true
//    cout << status << endl;
//
//    cout << "Add4: "<<endl;
//    status = b1 . Add("Hacker", "7-th avenue", "Child toys, Inc.", "Red light district");
//    // status = true
//    cout << status << endl;
//
//
//    cout << endl << endl << "Search 1: ";
//    status = b1 . Search("Brown", "Second street", cName, cAddress);
//    //    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//    // status = true, cName = "MakroHard, Inc.", cAddress="Soft street"
//    b1.print();
//
//    cout << endl << endl << "Search 2: ";
//    status = b1 . Search("Hacker", "Oak road", cName, cAddress);
//    //    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//    // status = false
//    b1.print();
//
//    cout << endl << endl << "Search 3: ";
//    status = b1 . Search("Smith", "Oak road", cName, cAddress);
//    //    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//    // status = true, cName = "ACME, Ltd.", cAddress="One ACME road"
//    b1.print();
//
//    cout << endl << endl << "Del 1: ";
//    status = b1 . Del("Smith", "Oak roads");
//    //    cout << status << endl;
//    //    // status = true
//    b1.print();
//
//    cout << endl << endl << "Search 4: ";
//    status = b1 . Search("Smith", "Oak road", cName, cAddress);
//    //    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//    //    // status = false
//    b1.print();
//
//
//
//
//
//
//
//    //    
//    CCompanyIndex b2;
//
//    cout << boolalpha;
//    cout << "Add 1: ";
//    status = b2 . Add("Smith", "Michigan avenue", "ACME, Ltd.", "One ACME road");
//    // status = true
//    cout << status << endl;
//
//    cout << "Search 1: ";
//    status = b2 . Search("Smith", "Michigan avenue", cName, cAddress);
//    // status = true, cName = "ACME, Ltd.", cAddress="One ACME road"
//    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//
//
//    cout << "Del 1: ";
//    status = b2 . Del("Smith", "Michigan avenue");
//    // status = true
//    cout << status << endl;
//
//
//    cout << "Search 2: ";
//    status = b2 . Search("Smith", "Michigan avenue", cName, cAddress);
//    // status = false
//    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//
//    cout << "Del 2: ";
//    status = b2 . Del("Smith", "Michigan avenue");
//    // status = false
//    cout << status << endl;
//
//
//    cout << "Add 2: ";
//    status = b2 . Add("Smith", "Michigan avenue", "Forks and Knives, Ltd.", "Cutlery avenue");
//    // status = true
//    cout << status << endl;
//
//
//    cout << "Search 3: ";
//    status = b2 . Search("Smith", "Michigan avenue", cName, cAddress);
//    // status = true, cName = "Forks and Knives, Ltd.", cAddress="Cutlery avenue"
//    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
//
//    cout << "Add 3: ";
//    status = b2 . Add("Smith", "Michigan avenue", "Child toys, Inc.", "Red light district");
//    // status = false
//    cout << status << endl;
//
//
//    cout << "Add 4: ";
//    status = b2 . Add("Smith", "Michigan avenue", "MakroHard, Inc.", "Soft street");
//    // status = false
//    cout << status << endl;
//
//
//    cout << "Del 3: ";
//    status = b2 . Del("Smith", "Michigan avenue");
//    // status = true
//    cout << status << endl;
//
//
//    cout << "Search 4: ";
//    status = b2 . Search("Smith", "Michigan avenue", cName, cAddress);
//    // status = false
//    (status ? cout << true << " \"" << cName << "\", \"" << cAddress << "\"" << endl : cout << false);
    
    b1.Add("D", "XX", "XX", "XX");
    b1.print();
    b1.Add("B", "XX", "XX", "XX");
    b1.print();
    b1.Add("E", "XX", "XX", "XX");
    b1.print();
    b1.Add("S", "XX", "XX", "XX");
    b1.print();
    b1.Add("A", "XX", "XX", "XX");
    b1.print();
    b1.Add("G", "XX", "XX", "XX");
    b1.print();
    b1.Add("HG", "XX", "XX", "XX");
    b1.print();
    b1.Add("M", "XX", "XX", "XX");
    b1.print();
    b1.Add("X", "XX", "XX", "XX");
    b1.print();
    b1.Add("Y", "XX", "XX", "XX");
    b1.print();
    b1.Add("W", "XX", "XX", "XX");
    b1.print();
    b1.Add("MH", "XX", "XX", "XX");
    b1.print();
    b1.Add("PO", "XX", "XX", "XX");
    b1.print();
    b1.Add("L", "XX", "XX", "XX");
    b1.print();
    b1.Add("J", "XX", "XX", "XX");
    b1.print();
    b1.Add("IK", "XX", "XX", "XX");
    b1.print();
    b1.Add("AB", "XX", "XX", "XX");
    b1.print();
    b1.Add("ER", "XX", "XX", "XX");
    b1.print();

}

#endif /* __PROGTEST__ */