/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on March 19, 2013, 7:15 PM
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

#define ENTRY_LESSER -1
#define ENTRY_EQUALS 0
#define ENTRY_GREATER 1

#define ARRAY_STARTSIZE 2

class CRecord { // One person or company
public:

    string name;
    string addr;
    CRecord ** items;

    CRecord();
    CRecord(string iname, string iaddr);
    ~CRecord();
    bool addItem(CRecord *newItem, int &index);
    bool removeItem(string name, string addr);
    bool findItem(string name, string addr, int &index) const;
    int compareTo(string cName, string cAddr);

    int getCount() const;

    void print();
private:
    int size;
    int count;
    void resize();

    void insert(CRecord *newItem, int pos);
    void remove(int pos);
};

class CIterator {
public:
    CIterator(int count);
    ~CIterator();
    bool AtEnd(void) const;
    void Next(void);
    const string & Name(void) const;
    const string & Address(void) const;
    CRecord ** items;
private:
    int size;
    int pos;
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
            const string & oAddr,
            const string & cName,
            const string & cAddr);

    CIterator * SearchOwner(const string & oName,
            const string & oAddr) const;

    CIterator * SearchCompany(const string & cName,
            const string & cAddr) const;

    CRecord owners, companies;
private:

    //    int ownersSize, ownersCount;
    //    int companiesSize, companiesCount;
};

bool findElm(const string &oName, const string &oAddr, int min, int max, int &pos, CRecord ** items, int count) {

    //    cout << "        Finding " << oName << ", " << oAddr << " (" << min << "," << max << ") cnt=" << count << endl;

    if (min >= max || max < 0) {
//        bool res;
//        int ret;
        if (count > 0) {
            switch (items[min]->compareTo(oName, oAddr)) {
                case ENTRY_LESSER:
                    pos = min + 1;
                    return false;

                case ENTRY_EQUALS:
                    pos = min;
                    return true;

                case ENTRY_GREATER:
                    pos = min;
                    return false;
            }
        } else {
            pos = min;
            return false;
        }
    }

    int mid = (max + min) / 2;
    //        cout << "  mid == "<<mid<<endl;
    switch (items[mid]->compareTo(oName, oAddr)) {
        case ENTRY_GREATER:
            //            cout << "  " << oName << " in " << min << " :: " << mid - 1 << endl;
            return findElm(oName, oAddr, min, mid - 1, pos, items, count);

        case ENTRY_EQUALS:
            //            cout << "equal on i=" << mid << endl;
            pos = mid;
            return true;

        case ENTRY_LESSER:
            //            cout << "  " << oName <<  " in " << mid + 1 << " :: " << max << endl;
            return findElm(oName, oAddr, mid + 1, max, pos, items, count);
    }

    pos = -1;
    return false;
}

CRecord::CRecord() {
    count = 0;
    size = ARRAY_STARTSIZE;
    items = new CRecord*[size];
}

CRecord::CRecord(string iname, string iaddr) : name(iname), addr(iaddr) {
    //    cout << " CRecord constructor called. Name:[" << name << "]" << endl;
    count = 0;
    size = ARRAY_STARTSIZE;
    //    cout << "  Creating Crecord*["<<size<<"]"<<endl;
    items = new CRecord*[size];
}

CRecord::~CRecord() {
    //    cout << " CRecord destructor called. Name:[" << name << "]count=" << count << endl;
    //    for (int i = 0; i < count; i++) {
    //    }
    delete[]items;
    //    cout << "   deleting [] items"<<endl;
    //    delete [] items;
}

bool CRecord::addItem(CRecord *newItem, int &index) {
    int pos;
    if (findElm(newItem->name, newItem->addr, 0, count - 1, pos, items, count)) {
        //        cout << "addItem: [" << newItem->name << ", " << newItem->addr << "] already here." << endl;
        index = pos;
        return false;
    } else {
        //        cout << "addItem: [" << newItem->name << ", " << newItem->addr << "] not found. Adding to " << pos << endl;
        insert(newItem, pos);
        index = pos;
        return true;
    }
}

bool CRecord::findItem(string name, string addr, int& index) const {
    int pos;
    if (findElm(name, addr, 0, count - 1, pos, items, count)) {
        //        cout << "addItem: [" << newItem->name << ", " << newItem->addr << "] already here." << endl;
        index = pos;
        return true;
    } else {
        //        cout << "addItem: [" << newItem->name << ", " << newItem->addr << "] not found. Adding to " << pos << endl;
        return false;
    }
}

bool CRecord::removeItem(string name, string addr) {
    int pos;
    if (findElm(name, addr, 0, count - 1, pos, items, count)) {
        //        cout << "addItem: [" << newItem->name << ", " << newItem->addr << "] already here." << endl;
        remove(pos);
        return true;
    } else {
        //        cout << "addItem: [" << newItem->name << ", " << newItem->addr << "] not found. Adding to " << pos << endl;
        return false;
    }
}

void CRecord::insert(CRecord* newItem, int pos) {
    if (count + 1 >= size) {
        resize();
    }

    for (int i = count - 1; i >= pos; i--) {
        items[i + 1] = items[i];
    }

    items[pos] = newItem;

    count++;
}

void CRecord::remove(int pos) {
    //    cout << "      removing item at " << pos << endl;
    //delete items[pos];
    for (int i = pos; i < count - 1; i++) {
        //        cout << "          items[" << i << "] <- items[" << i + 1 << "]" << endl;
        items[i] = items[i + 1];
    }
    count--;
    //    cout << "      count: " << count + 1 << " -> " << count << endl;
}

void CRecord::resize() {
    int newSize = size * 2;
    CRecord ** tmp = new CRecord*[newSize];
    size = newSize;

    for (int i = 0; i < count; i++) {
        tmp[i] = items[i];
    }

    delete [] items;
    items = tmp;
}

int CRecord::getCount() const {
    return count;
}

int CRecord::compareTo(string cName, string cAddr) {
    if (name < cName) { // item in lower part
        return ENTRY_LESSER;
    } else if (name > cName) {
        return ENTRY_GREATER;
    } else if (addr < cAddr) {
        return ENTRY_LESSER;
    } else if (addr > cAddr) {
        return ENTRY_GREATER;
    } else {
        //            cout << "ENTRY: " << oName << "==" << cName << endl;

        return ENTRY_EQUALS;
    };
}

void CRecord::print() {
    cout << endl << endl;
    cout << "------- " << name << " | " << addr << endl;
    for (int i = 0; i < count; i++) {
        cout << "  " << items[i]->name << " | " << items[i]->addr << " contains: " << endl;
        for (int k = 0; k < items[i]->count; k++) {
            cout << "    " << items[i]->items[k]->name << " | " << items[i]->items[k]->addr << endl;
        }
        cout << endl;
    }

    cout << endl << endl;
}

/*
 ************************************************************************************************************
 */


CIterator::CIterator(int count) {
    size = count;
    items = new CRecord*[size];
    pos = 0;
}

CIterator::~CIterator() {
    delete [] items;
}

bool CIterator::AtEnd() const {
    if (pos == size) {
        //        cout << "  iterator atEnd." << endl;
        return true;
    } else {
        //        cout << "  iterator NOT atEnd." << endl;
        return false;
    }
}

void CIterator::Next() {
    if (pos < size) {
        pos++;
        //        cout << "  iterator moving " << pos - 1 << "->" << pos << endl;
    }
}

const string & CIterator::Name() const {
    return items[pos]->name;
}

const string & CIterator::Address() const {
    return items[pos]->addr;
}

CCompanyIndex::CCompanyIndex() {
    owners.name = "OWNERS";
    companies.name = "COMPANIES";
}

CCompanyIndex::~CCompanyIndex() {
    for (int i = 0; i < owners.getCount(); i++) {
        delete owners.items[i];
    }

    for (int i = 0; i < companies.getCount(); i++) {
        delete companies.items[i];
    }
}

bool CCompanyIndex::Add(const string& oName, const string& oAddr, const string& cName, const string& cAddr) {
    CRecord * owner = new CRecord(oName, oAddr);
    CRecord * company = new CRecord(cName, cAddr);

    /*
     * Try to find both owner and company. 
     * - If owner found, add company to his list
     * - If owner not found, create and add company
     * . same with company
     * 
     */

    int ownerPos;
    int companyPos;
    bool res;

    bool ownerExisted;
    bool companyExisted;
    cout << boolalpha;

    //    cout << "Adding owner [" << oName << "," << oAddr << "]..." << endl;

    ownerExisted = !owners.addItem(owner, ownerPos);
    companyExisted = !companies.addItem(company, companyPos);
    
    if (ownerExisted) delete owner;
    if (companyExisted) delete company;

    cout << "Adding [" << oName << ", " << oAddr << "]-[" << cName << ", " << cAddr << "]" << endl;
    cout << "  ownerExisted: "<<ownerExisted<<endl;
    cout << "  companyExisted: "<<companyExisted<<endl;

    int tmp;
    res = owners.items[ownerPos]->addItem(companies.items[companyPos], tmp);
    if (!res) return false;

    res = companies.items[companyPos]->addItem(owners.items[ownerPos], tmp);
    if (!res) return false;


    //    if (ownerExisted && companyExisted) {
    //        delete owner;
    //        delete company;
    //    } else if (ownerExisted && !companyExisted) {
    //        delete owner;
    //
    //        res = owners.items[ownerPos]->addItem(companies.items[companyPos]);
    //        if (!res) return false;
    //
    //        res = companies.items[companyPos]->addItem(owners.items[ownerPos]);
    //        if (!res) return false;
    //    }


    //    res = owners.items[pos]->addItem(company, pos);
    //    if (!res) return false;


    return true;
}

bool CCompanyIndex::Del(const string& oName, const string& oAddr, const string& cName, const string& cAddr) {
    int pos;
    bool res;

    cout << "Deleting owner's company [" << oName << "," << oAddr << "] -/- [" << cName << "," << cAddr << "]..." << endl;

    res = owners.findItem(oName, oAddr, pos);
    if (!res) return false; // owner not found
    cout << "  Owner found @" << pos << endl;

    res = owners.items[pos]->removeItem(cName, cAddr); //.removeItem(oName, oAddr);
    if (!res) return false; // owner's company not found
    cout << "  Owner's company removed." << endl << endl;

    //    cout << "Deleting company's owner [" << oName << "," << oAddr << "]..." << endl;
    res = companies.findItem(cName, cAddr, pos);
    if (!res) return false; // owner not found
    cout << "  Company found @" << pos << endl;

    res = companies.items[pos]->removeItem(oName, oAddr); //.removeItem(oName, oAddr);
    if (!res) return false; // owner's company not found
    cout << "  Company's owner removed." << endl;
    return true;
    //    cout << "RETURN TRUE" << endl << endl;
    //res = companies.removeItem()
}

CIterator * CCompanyIndex::SearchOwner(const string& oName, const string& oAddr) const {
    //    cout << "  SearchOwner [" << oName << ", " << oAddr << "]" << endl;
    int pos;
    bool res = owners.findItem(oName, oAddr, pos);
    if (!res) {
        //        cout << "    item not found." << endl;
        return NULL;
    }

    int cnt = owners.items[pos]->getCount();
    CIterator* tmp = new CIterator(cnt);
    //    cout << "    count=" << cnt << endl;
    for (int i = 0; i < cnt; i++) {
        tmp->items[i] = owners.items[pos]->items[i];
        //        cout << "      adding [" << tmp->items[i]->name << ", " << tmp->items[i]->addr << "]" << endl;
    }
    return tmp;
}

CIterator * CCompanyIndex::SearchCompany(const string& cName, const string& cAddr) const {
    //    cout << "  SearchCompany [" << cName << ", " << cAddr << "]" << endl;
    int pos;
    bool res = companies.findItem(cName, cAddr, pos);
    if (!res) {
        //        cout << "    item not found." << endl;
        return NULL;
    }

    int cnt = companies.items[pos]->getCount();
    CIterator* tmp = new CIterator(cnt);
    //    cout << "    count=" << cnt << endl;
    for (int i = 0; i < cnt; i++) {
        tmp->items[i] = companies.items[pos]->items[i];
        //        cout << "      adding [" << tmp->items[i]->name << ", " << tmp->items[i]->addr << "]" << endl;
    }
    return tmp;
}



#ifndef __PROGTEST__

void showResults(CIterator * it) {
    cout << "  showResults:" << endl;
    while (!it -> AtEnd()) {
        cout << "    " << it -> Name() << ",  " << it -> Address() << endl;
        it -> Next();
    }
}

int main() {

    CIterator * it;
    bool status;
    CCompanyIndex b1;

    //    status = b1.Add("E", "XXX", "cA", "cXXX");
    //    status = b1.Add("A", "XXX", "cC", "cXXX");
    //    status = b1.Add("E", "XXX", "cX", "cXXZ");
    //    status = b1.Add("C", "XXX", "cG", "cXXX");
    //    status = b1.Add("A", "XXX", "cC", "cXXX");
    //    status = b1.Add("F", "XXX", "cB", "cXXX");
    //    status = b1.Add("D", "XXX", "cE", "cXXX");
    //    status = b1.Add("D", "XXX", "cC", "cXXX");

    cout << boolalpha;

    status = b1 . Add("Smith", "Oak road", "ACME, Ltd.", "One ACME road");
    //status = true
    cout << status << endl << endl;


    status = b1 . Add("Brown", "Second street", "ACME, Ltd.", "Mountain road");
    // status = true
    cout << status << endl << endl;

    status = b1 . Add("Hacker", "5-th avenue", "Forks and Knives, Ltd.", "Cutlery avenue");
    // status = true
    cout << status << endl << endl;

    status = b1 . Add("Hacker", "7-th avenue", "Child toys, Inc.", "Red light district");
    // status = true
    cout << status << endl << endl;

    status = b1 . Add("Smith", "Oak road", "ACME, Ltd.", "Mountain road");
    // status = true
    cout << status << endl << endl;

    status = b1 . Add("Hacker", "5-th avenue", "ACME, Ltd.", "One ACME road");
    // status = true
    cout << status << endl << endl;

    status = b1 . Add("Hacker", "7-th avenue", "ACME, Ltd.", "Mountain road");
    //status = true
    cout << status << endl << endl;




    cout << endl << endl << endl;
    it = b1 . SearchOwner("Brown", "Second street");
    showResults(it);
    cout << endl << endl;
    /*
    ----8<----8<----8<----8<----
    ACME, Ltd., Mountain road
    ----8<----8<----8<----8<----
     */
    delete it;




    it = b1 . SearchOwner("Hacker", "Oak road");
    cout << (it == NULL ? "NULL!" : "NOT NULL!") << endl;
    cout << endl << endl;
    // it = NULL


    it = b1 . SearchOwner("Hacker", "7-th avenue");
    showResults(it);
    cout << endl << endl;
    /*
    ----8<----8<----8<----8<----
    Child toys, Inc., Red light district
    ACME, Ltd., Mountain road
    ----8<----8<----8<----8<----
     */
    delete it;


    it = b1 . SearchCompany("ACME, Ltd.", "Mountain road");
    showResults(it);
    cout << endl << endl;
    /*
    ----8<----8<----8<----8<----
    Hacker, 7-th avenue
    Brown, Second street
    Smith, Oak road
    ----8<----8<----8<----8<----
     */
    delete it;


    //    it = b1 . SearchCompany("Child toys, Inc.", "Mountain road");
    //    cout << (it == NULL ? "NULL!" : "NOT NULL!") << endl;
    // it = NULL



    b1.owners.print();
    b1.companies.print();

    status = b1 . Del("Smith", "Oak road", "Child toys, Inc.", "Red light district");
    // status = false
    cout << status << endl << endl;




    status = b1 . Del("Smith", "Oak road", "ACME, Ltd.", "Mountain road");
    // status = true
    cout << status << endl << endl;


    it = b1 . SearchOwner("Smith", "Oak road");
    showResults(it);
    /*
    ----8<----8<----8<----8<----
    ACME, Ltd., One ACME road
    ----8<----8<----8<----8<----
     */
    delete it;


    status = b1 . Add("Smith", "Oak road", "ACME, Ltd.", "One ACME road");
    //status = false
    cout << status << endl << endl;
}

#endif

// TODO CIterator, Searching, Valgrind.