#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ncurses.h>
using namespace std;
#define __DEBUG
#endif /* __PROGTEST__ */

class MyString {
public:
    explicit MyString(const char *src);
    MyString(const MyString &orig);
    ~MyString();

    int length() const;
    const char* getCStr() const;

    bool operator==(const MyString &other)const;
    bool operator<(const MyString &other)const;
    bool operator>(const MyString &other)const;
    bool operator==(const char *other)const;
    bool operator<(const char *other)const;
    bool operator>(const char *other)const;

    friend ostream& operator <<(ostream & os, const MyString &toPrint);

    int references;
private:
    char *data;
};

class CResidencyRecord {
public:
    CResidencyRecord(const char * adate, const char * astreet, const char * acity);
    ~CResidencyRecord();

    bool operator==(const char *other)const;
    bool operator<(const char *other)const;
    bool operator>(const char *other)const;

    friend ostream& operator <<(ostream &os, const CResidencyRecord &toPrint);

    int references;
private:
    MyString *date, *street, *city;
};

class CPerson {
public:
    CPerson(const char * id, const char * name, const char * surname,
            const char * date, const char * street, const char * city);
    ~CPerson();

    CPerson(const CPerson &other);

    void addResidency(const char *date, const char *street, const char *city, int pos);
    bool findResidency(const char *ID, int *pos) const;

    bool operator<(const char *other);
    bool operator>(const char *other);

    CPerson& operator=(const CPerson &other);

    friend ostream& operator <<(ostream &os, const CPerson &toPrint);

    int references;
    MyString *id, *name, *surname;
private:

    CResidencyRecord **residencies;
    int residenciesCount;
    int residenciesSize;
    void insertResidency(CResidencyRecord *toAdd, int pos);

    void incSize();
};

class CRefPersonList {
public:
    CRefPersonList();
    CRefPersonList(const CRefPersonList &other);
    ~CRefPersonList();

    bool findPerson(MyString ID, int *pos) const;
    bool findPerson(const char *ID, int *pos) const;

    void insertPerson(CPerson *toAdd, int index);

    int references;
    CPerson ** people;
private:
    //    CPerson ** people;
    int peopleCount;
    int peopleSize;

    void incSize();
};

class CRegister {
public:
    CRegister();
    ~CRegister();

    CRegister(const CRegister &other);

    CRegister& operator=(const CRegister &other);

    bool Add(const char * id,
            const char * name,
            const char * surname,
            const char * date,
            const char * street,
            const char * city);
    bool Resettle(const char * id,
            const char * date,
            const char * street,
            const char * city);
    bool Print(ostream & os,
            const char * id) const;

    static bool findString(const char *ID, int *pos);
    static void insertString(MyString *toAdd, int pos);
    static void removeString(int pos);

    static MyString* getString(int pos);
    static void printStrings();

    //    int references;
private:

    //    bool findPerson(MyString ID, int *pos) const;
    //    bool findPerson(const char *ID, int *pos) const;
    //
    //    void insertPerson(CPerson *toAdd, int index);
    //
    //    void incSize();

    //    CPerson ** people;
    CRefPersonList * peopleList;

    /* de-duplication */
    static MyString ** strings;
    static int stringsCount;
    static int stringsSize;
    static void incStringsSize();
    static void checkDelStrings();
};

/******************************************************************************/

/***********              String                      *************************/

MyString::MyString(const char *src) {
    references = 1;
    data = new char[strlen(src) + 1];
    memcpy(data, src, strlen(src) + 1);
}

MyString::MyString(const MyString &orig) {
    data = orig.data;
    references = orig.references;
}

MyString::~MyString() {
    int pos;
    if (CRegister::findString(data, &pos)) {
        CRegister::removeString(pos);
    }
    delete [] data;
}

int MyString::length() const {
    return strlen(data);
}

bool MyString::operator ==(const MyString &other)const {
    return strcmp(data, other.data) == 0;
}

bool MyString::operator<(const MyString &other)const {
    return strcmp(data, other.data) < 0;
}

bool MyString::operator>(const MyString &other)const {
    return strcmp(data, other.data) > 0;
}

bool MyString::operator==(const char *other)const {
    return strcmp(data, other) == 0;
}

bool MyString::operator<(const char *other)const {
    return strcmp(data, other) < 0;
}

bool MyString::operator>(const char *other)const {
    return strcmp(data, other) > 0;
}

ostream& operator <<(ostream & os, const MyString &toPrint) {
    //    os << toPrint.data << " (refs " << toPrint.references << ", " << &toPrint << ") ";
    os << toPrint.data;
    return os;
}

const char* MyString::getCStr() const {
    return data;
}

/***********              CResidencyRecord                      ***************/

CResidencyRecord::CResidencyRecord(const char * adate,
        const char * astreet, const char * acity) {

    int pos;

    if (CRegister::findString(adate, &pos)) {
        date = CRegister::getString(pos);
        date->references++;
    } else {
        date = new MyString(adate);
        CRegister::insertString(date, pos);
    }

    if (CRegister::findString(astreet, &pos)) {
        street = CRegister::getString(pos);
        street->references++;
    } else {
        street = new MyString(astreet);
        CRegister::insertString(street, pos);
    }

    if (CRegister::findString(acity, &pos)) {
        city = CRegister::getString(pos);
        city->references++;
    } else {
        city = new MyString(acity);
        CRegister::insertString(city, pos);
    }

    references = 1;
}

CResidencyRecord::~CResidencyRecord() {
    if (date->references == 1) {
        delete date;
    } else {
        date->references--;
    }

    if (street->references == 1) {
        delete street;
    } else {
        street->references--;
    }

    if (city->references == 1) {
        delete city;
    } else {
        city->references--;
    }
}

bool CResidencyRecord::operator==(const char *other)const {
    return *date == other;
}

bool CResidencyRecord::operator<(const char *other)const {
    return *date < other;
}

bool CResidencyRecord::operator>(const char *other)const {
    return *date > other;
}

ostream& operator <<(ostream &os, const CResidencyRecord &toPrint) {
    os << *toPrint.date << " " << *toPrint.street << " " << *toPrint.city;
    return os;
}

/***********              CPerson                               ***************/

CPerson::CPerson(const char * aid, const char * aname, const char * asurname,
        const char * adate, const char * astreet, const char * acity) {

    int pos;
    if (CRegister::findString(aid, &pos)) {
        id = CRegister::getString(pos);
        id->references++;
    } else {
        id = new MyString(aid);
        CRegister::insertString(id, pos);
    }

    if (CRegister::findString(aname, &pos)) {
        name = CRegister::getString(pos);
        name->references++;
    } else {
        name = new MyString(aname);
        CRegister::insertString(name, pos);
    }

    if (CRegister::findString(asurname, &pos)) {
        surname = CRegister::getString(pos);
        surname->references++;
    } else {
        surname = new MyString(asurname);
        CRegister::insertString(surname, pos);
    }

    references = 1;
    residenciesCount = 0;
    residenciesSize = 0;

    addResidency(adate, astreet, acity, 0);
}

CPerson::~CPerson() {

    //    cout << "[DELETE] person: " << *id << endl;
    //    cout << "  residenciesCount: " << residenciesCount << endl;
    //    cout << "  residenciesSize: " << residenciesSize << endl;

    for (int i = 0; i < residenciesCount; i++) {

        if (residencies[i]->references == 1) {
            delete residencies[i];
        } else {
            residencies[i]->references--;
        }
    }

    delete [] residencies;

    if (id->references == 1) {
        delete id;
    } else {
        id->references--;
    }

    if (name->references == 1) delete name;
    else name->references--;

    if (surname->references == 1) delete surname;
    else surname->references--;
}

CPerson::CPerson(const CPerson &other) {
    references = 1;

    id = other.id;
    id->references++;

    name = other.name;
    name->references++;

    surname = other.surname;
    surname->references++;

    residenciesCount = other.residenciesCount;
    residenciesSize = other.residenciesSize;
    residencies = new CResidencyRecord*[residenciesSize];

    memcpy(residencies, other.residencies,
            residenciesCount * sizeof (CResidencyRecord*));

    for (int i = 0; i < residenciesCount; i++) {
        //        residencies[i] = other.residencies[i];
        residencies[i]->references++;
    }
}

CPerson& CPerson::operator=(const CPerson &other) {
    if (this == &other) return *this;

    for (int i = 0; i < residenciesCount; i++) {
        if (residencies[i]->references == 1) {
            delete residencies[i];
        } else {
            residencies[i]->references--;
        }
    }

    delete [] residencies;

    residenciesCount = other.residenciesCount;
    residencies = new CResidencyRecord*[residenciesCount];

    memcpy(residencies, other.residencies,
            residenciesCount * sizeof (CResidencyRecord*));

    for (int i = 0; i < residenciesCount; i++) {
        //        residencies[i] = other.residencies[i];
        residencies[i]->references++;
    }

    if (id->references == 1) delete id;
    else id->references--;

    if (name->references == 1) delete name;
    else name->references--;

    if (surname->references == 1) delete surname;
    else surname->references--;

    id = other.id;
    name = other.name;
    surname = other.surname;

    id ->references++;
    name->references++;
    surname->references++;

    return *this;
}

bool CPerson::operator<(const char * other) {
    return *id < other;
}

bool CPerson::operator>(const char * other) {
    return *id > other;
}

void CPerson::addResidency(const char *date, const char *street, const char *city,
        int pos) {

    CResidencyRecord *tmp = new CResidencyRecord(date, street, city);
    insertResidency(tmp, pos);
}

bool CPerson::findResidency(const char *date, int *pos) const {

    if (residenciesCount == 0) {
        *pos = 0;
        return false;
    }

    int min = 0;
    int max = residenciesCount - 1;


    while (max > min && max >= 0) {
        int mid = (max + min) / 2;

        if (*(residencies[mid]) < date) {
            min = mid + 1;
        } else if (*(residencies[mid]) > date) {
            max = mid - 1;
        } else {
            *pos = mid;
            return true;
        }
    }


    //     Klic nebyl nalezen -> urceni, kam patri
    if (*(residencies[min]) < date) {
        *pos = min + 1;
        return false;
    } else if (*(residencies[min]) > date) {
        *pos = min;
        return false;
    } else {
        *pos = min;
        return true;
    }
}

void CPerson::insertResidency(CResidencyRecord *toAdd, int index) {
    //    cout << "[CALL] CPerson::insertRes. ResSize: "<<residenciesSize<<
    //            ", resC: "<<residenciesCount<<endl;

    incSize();

    if (residenciesCount != 0) {
        memmove(&(residencies[index + 1]), &(residencies[index]),
                (residenciesCount - index) * sizeof (CPerson*));
    }

    residencies[index] = toAdd;

    residenciesCount++;
}

void CPerson::incSize() {
    if (residenciesCount == 0) {
        residenciesSize = 4;
        residencies = new CResidencyRecord*[residenciesSize];
    } else if (residenciesSize == residenciesCount) {

        residenciesSize *= 2;

        CResidencyRecord **tmp = new CResidencyRecord*[residenciesSize];
        memcpy(tmp, residencies, residenciesCount * sizeof (CResidencyRecord*));

        delete [] residencies;

        residencies = tmp;
    }

    //    cout << "  [CALL] CPerson::incSize. residenciesSize=" << residenciesSize <<
    //            " residenciesCount=" << residenciesCount << endl;
}

ostream& operator <<(ostream &os, const CPerson &toPrint) {
    os << *toPrint.id << " " << *toPrint.name << " " << *toPrint.surname << endl;

    for (int i = 0; i < toPrint.residenciesCount; i++) {
        os << *(toPrint.residencies[i]) << endl;
    }

    return os;
}

/***********              CRefPersonList                        ***************/

CRefPersonList::CRefPersonList() {
    peopleCount = 0;
    peopleSize = 0;
    people = NULL;
    references = 1;
}

CRefPersonList::CRefPersonList(const CRefPersonList& other) {

//    cout << "[COPYCONST] RefList: other.count=" << other.peopleCount << ", other.size:" <<
//            other.peopleSize << endl;

    peopleCount = other.peopleCount;
    peopleSize = other.peopleSize;
    references = 1;

    people = new CPerson*[peopleSize];

    memcpy(people, other.people, peopleCount * sizeof (CResidencyRecord*));
    for (int i = 0; i < peopleCount; i++) {
        //        people[i] = new CPerson(*(other.people[i]));
        other.people[i]->references++;
    }
}

CRefPersonList::~CRefPersonList() {
    for (int i = 0; i < peopleCount; i++) {
        if (people[i]->references == 1) {
            delete people[i];
        } else {
            people[i]->references--;
        }
    }

    delete [] people;
}

bool CRefPersonList::findPerson(MyString ID, int *pos) const {
    return findPerson(ID.getCStr(), pos);
}

bool CRefPersonList::findPerson(const char *ID, int *pos) const {

    if (peopleCount == 0) {
        *pos = 0;
        return false;
    }

    int min = 0;
    int max = peopleCount - 1;


    while (max > min && max >= 0) {
        int mid = (max + min) / 2;

        if (*(people[mid]) < ID) {
            min = mid + 1;
        } else if (*(people[mid]) > ID) {
            max = mid - 1;
        } else {
            *pos = mid;
            return true;
        }
    }


    //     Klic nebyl nalezen -> urceni, kam patri
    if (*(people[min]) < ID) {
        *pos = min + 1;
        return false;
    } else if (*(people[min]) > ID) {
        *pos = min;
        return false;
    } else {
        *pos = min;
        return true;
    }
}

void CRefPersonList::insertPerson(CPerson *toAdd, int index) {
    incSize();

    if (peopleCount != 0) {
        memmove(&(people[index + 1]), &(people[index]),
                (peopleCount - index) * sizeof (CPerson*));
    }

    people[index] = toAdd;

    peopleCount++;
}

void CRefPersonList::incSize() {
    if (peopleCount == 0) {
        peopleSize = 4;
        people = new CPerson*[peopleSize];
    } else if (peopleCount == peopleSize) {
        peopleSize *= 2;

        CPerson **tmp = new CPerson*[peopleSize];
        memcpy(tmp, people, peopleCount * sizeof (CPerson*));

        delete [] people;

        people = tmp;
    }
}

/***********              CRegister                             ***************/

CRegister::CRegister() {
    peopleList = new CRefPersonList();
}

CRegister::~CRegister() {

    if (peopleList->references == 1) {
        delete peopleList;
    } else {
        peopleList->references--;
    }
}

bool CRegister::Add(const char * id, const char * name, const char * surname,
        const char * date, const char * street, const char * city) {

//#ifdef __DEBUG 
//    cout << "[CALL] Add (" << id << ", " << name << ", " << surname <<
//            ", " << date << ", " << street << ", " << city << endl;
//#endif

    int pos;
    if (peopleList->findPerson(id, &pos)) { // Person was found, cannot add them
        return false;
    } else {
        if (peopleList->references > 1) {
            peopleList->references--;
            peopleList = new CRefPersonList(*peopleList);
        }

        CPerson *tmp = new CPerson(id, name, surname, date, street, city);
        peopleList->insertPerson(tmp, pos);
        return true;
    }
}

bool CRegister::Resettle(const char * id, const char * date, const char * street,
        const char * city) {

//#ifdef __DEBUG 
//    cout << "[CALL] Resettle(" << id << ", " << date << ", " << street << ", " << city << endl;
//#endif

    int pos;
    if (!peopleList->findPerson(id, &pos)) {
        return false;
    } else {
        int resPos;
        if (peopleList->people[pos]->findResidency(date, &resPos)) {
            return false;
        } else {
            if (peopleList->references > 1) {
//                cout << "  more references to this List. Cloning..." << endl;
                peopleList->references--;
                peopleList = new CRefPersonList(*peopleList);
            }

            if (peopleList->people[pos]->references > 1) {
//                cout << "  more references to this Person (" <<
//                        *(peopleList->people[pos]->name)<<" "<<
//                        *(peopleList->people[pos]->surname)<< ") cloning." << endl;
                peopleList->people[pos]->references--;
                peopleList->people[pos] = new CPerson(*(peopleList->people[pos]));
            }
            peopleList->people[pos]->addResidency(date, street, city, resPos);
            return true;
        }
    }

}

bool CRegister::Print(ostream & os, const char * id) const {
    int pos;
    if (!peopleList->findPerson(id, &pos)) {
        return false;
    } else {
        os << *(peopleList->people[pos]);
        return true;
    }
}

CRegister::CRegister(const CRegister &other) {
    peopleList = other.peopleList;
    peopleList->references++;
}

CRegister& CRegister::operator=(const CRegister &other) {

//#ifdef __DEBUG 
//    cout << "[CALL] operator =" << endl;
//#endif

    if (this == &other) return *this;

    if (peopleList->references == 1) {
        delete peopleList;
    } else {
        peopleList->references--;
    }

    peopleList = other.peopleList;
    peopleList->references++;

    return *this;
}



// REDUPLICATION
int CRegister::stringsCount = 0;
int CRegister::stringsSize = 0;
MyString** CRegister::strings = NULL;

bool CRegister::findString(const char* ID, int* pos) {

    if (stringsCount == 0) {
        *pos = 0;
        return false;
    }

    int min = 0;
    int max = stringsCount - 1;


    while (max > min && max >= 0) {
        int mid = (max + min) / 2;

        if (*(strings[mid]) < ID) {
            min = mid + 1;
        } else if (*(strings[mid]) > ID) {
            max = mid - 1;
        } else {
            *pos = mid;

            return true;
        }
    }


    //     Klic nebyl nalezen -> urceni, kam patri
    if (*(strings[min]) < ID) {
        *pos = min + 1;
        return false;
    } else if (*(strings[min]) > ID) {
        *pos = min;
        return false;
    } else {
        *pos = min;
        return true;
    }
}

void CRegister::insertString(MyString* toAdd, int pos) {
    incStringsSize();

    memmove(&(strings[pos + 1]), &(strings[pos]),
            (stringsCount - pos) * sizeof (MyString*));

    strings[pos] = toAdd;

    stringsCount++;

    //    printStrings();
}

void CRegister::removeString(int pos) {

    memmove(&(strings[pos]), &(strings[pos + 1]),
            (stringsCount - pos - 1) * sizeof (MyString*));

    //    decStringsSize();
    stringsCount--;

    checkDelStrings();

    //    printStrings();
}

void CRegister::incStringsSize() {
    if (stringsCount == 0) {
        stringsSize = 4;
        strings = new MyString*[stringsSize];
    } else if (stringsCount == stringsSize) {
        stringsSize *= 2;

        MyString **tmp = new MyString*[stringsSize];
        memcpy(tmp, strings, stringsCount * sizeof (MyString*));

        delete [] strings;

        strings = tmp;
    }
}

void CRegister::checkDelStrings() {
    if (stringsCount == 0) {
        delete [] strings;
        stringsSize = 0;
    }
}

MyString* CRegister::getString(int pos) {
    return strings[pos];
}

void CRegister::printStrings() {

    for (int i = 0; i < stringsCount; i++) {
        cout << "        (" << i << ") " << *(strings[i]) << " used: " <<
                strings[i]->references << endl;
    }

}