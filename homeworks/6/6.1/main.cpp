/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on April 16, 2013, 9:43 AM
 */

#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

class Partition {
public:

    Partition(int aSize, int aID, string aName) {
        size = aSize;
        ID = aID;
        name = aName;
    }

    void print(ostream &os, string prefix) const {
        os << prefix << "[" << ID << "]: " << size << " GiB, " << name << endl;
    }


    int size;
    list<Partition>::size_type ID;
    string name;
};

class CComponent {
public:

    virtual ~CComponent() {
        //        cout << "CComponent destructor" << endl;
    }

    virtual void print(ostream &os, string prefix) const {
        os << "CComponent print!" << endl;
    }

    virtual void printCaption(ostream &os, string prefix) const {

    }

    virtual CComponent* clone() const {
        cout << "CComponent clone." << endl;
        return NULL;
    }
protected:
};

class CComputer {
public:
    CComputer(string aName);
    ~CComputer();
    CComputer(const CComputer &orig);
    CComputer& operator=(const CComputer &orig);

    CComputer& AddComponent(const CComponent &newComponent);
    CComputer& AddAddress(string aAddr);
    const string &getName() const;

    void print(ostream &os, string prefix) const;
    void printCaption(ostream &os, string prefix) const;

    friend ostream& operator<<(ostream & os, const CComputer & toPrint);
protected:
    string name;
    list<string> addrs;
    list<const CComponent*> components;
};

class CNetwork {
public:
    CNetwork(string aName);

    CNetwork& AddComputer(CComputer & toAdd);
    CComputer* FindComputer(string ID);
    void print(ostream &os, string prefix)const;
    void printCaption(ostream &os, string prefix) const;

    friend ostream& operator<<(ostream &os, const CNetwork &toPrint);
protected:
    string name;
    list<CComputer> computers;
};

class CCPU : public CComponent {
public:
    CCPU(int aCores, int aFreq);
    virtual ~CCPU();
    virtual CComponent* clone() const;
    virtual void print(ostream &os, string prefix)const;
    virtual void printCaption(ostream &os, string prefix) const;
protected:
    int cores;
    int freq;
};

class CMemory : public CComponent {
public:
    CMemory(int aSize);
    virtual ~CMemory();
    virtual CComponent* clone() const;
    virtual void print(ostream &os, string prefix)const;
    virtual void printCaption(ostream &os, string prefix) const;
protected:
    int size;
};

class CDisk : public CComponent {
public:
    static const int MAGNETIC;
    static const int SSD;

    CDisk(char aType, int aSize);
    virtual ~CDisk();

    virtual void print(ostream &os, string prefix)const;
    virtual void printCaption(ostream &os, string prefix) const;

    virtual CComponent* clone() const;
    CDisk& AddPartition(int aSize, string ID);
protected:
    char type;
    int size;

    list<Partition> partitions;
};

const int CDisk::MAGNETIC = 0;
const int CDisk::SSD = 1;

/****************** CNetwork      *********************************************/
CNetwork::CNetwork(string aName) : name(aName) {

}

CNetwork& CNetwork::AddComputer(CComputer & toAdd) {
    computers.push_back(toAdd);

    return *this;
}

CComputer* CNetwork::FindComputer(string ID) {
    for (list<CComputer>::iterator iter = computers.begin();
            iter != computers.end(); iter++) {
        if ((*iter).getName().compare(ID) == 0) return &(*iter);
    }

    return NULL;
}

void CNetwork::print(ostream &os, string prefix) const {

    list<CComputer>::size_type printed = 0;
    for (list<CComputer>::const_iterator iter = computers.begin();
            iter != computers.end();
            iter++) {
        if (printed == computers.size() - 1) {
            (*iter).printCaption(os, prefix + "\\-");
            (*iter).print(os, prefix + "  ");
            printed++;
        } else {
            (*iter).printCaption(os, prefix + "+-");
            (*iter).print(os, prefix + "| ");
            printed++;
        }
    }
}

void CNetwork::printCaption(ostream &os, string prefix) const {
    os << prefix << "Network: " << name << endl;
}

ostream& operator<<(ostream &os, const CNetwork &toPrint) {
    toPrint.printCaption(os, "");
    toPrint.print(os, "");

    return os;
}

/****************** CComputer     *********************************************/
CComputer::CComputer(string aName) : name(aName) {
    //    cout << "[CComputer] " << aName << endl;
}

CComputer::~CComputer() {
    for (list<const CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {
        delete (*iter);
    }
}

CComputer::CComputer(const CComputer &orig) : name(orig.name), addrs(orig.addrs) {

    //    cout << "    CComputer copy constructor" << endl;

    for (list<const CComponent*>::const_iterator iter = orig.components.begin();
            iter != orig.components.end();
            iter++) {

        components . push_back((*iter) -> clone());
    }
}

CComputer& CComputer::operator=(const CComputer &orig) {

    if (&orig == this) {
        return *this;
    }

    for (list<const CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {
        delete (*iter);
    }
    components.clear();
    
//    while (components.size() > 0) {
//        delete *(components.end());
//        components.pop_back();
//    }

    for (list<const CComponent*>::const_iterator iter = orig.components.begin();
            iter != orig.components.end();
            iter++) {
        components . push_back((*iter) -> clone());
    }

    name = orig.name;
    addrs = orig.addrs;

    return *this;

}

CComputer& CComputer::AddComponent(const CComponent &newComponent) {
    //    cout << "Adding component"<<endl;
    components.push_back(newComponent.clone());

    return *this;
}

CComputer& CComputer::AddAddress(string addr) {
    addrs.push_back(addr);

    return *this;
}

const string& CComputer::getName() const {
    return name;
}

void CComputer::print(ostream &os, string prefix)const {

    for (list<string>::const_iterator iter = addrs.begin();
            iter != addrs.end();
            iter++) {
        os << prefix << "+-" << *iter << endl;
    }

    //    os << prefix << " components size: " << components.size() << endl;
    list<const CComponent*>::size_type printed = 0;
    for (list<const CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {

        if (printed == components.size() - 1) {
            (*iter)->printCaption(os, prefix + "\\-");
            (*iter)->print(os, prefix + "  ");
            printed++;
        } else {
            (*iter)->printCaption(os, prefix + "+-");
            (*iter)->print(os, prefix + "| ");
            printed++;
        }
    }
}

void CComputer::printCaption(ostream &os, string prefix)const {
    os << prefix << "Host: " << name << endl;
}

ostream& operator<<(ostream & os, const CComputer & toPrint) {
    toPrint.printCaption(os, "");
    toPrint.print(os, "");

    return os;
}

/****************** CCPU          *********************************************/
CCPU::CCPU(int aCores, int aFreq) : cores(aCores), freq(aFreq) {
    //    cout << "CCPU " << aCores << " @" << freq << endl;
}

CCPU::~CCPU() {

}

void CCPU::print(ostream &os, string prefix) const {

}

void CCPU::printCaption(ostream &os, string prefix) const {
    os << prefix << "CPU, " << cores << " cores @ " << freq << "MHz" << endl;
}

CComponent* CCPU::clone() const {
    return new CCPU(cores, freq);
}

/****************** CMemory       *********************************************/
CMemory::CMemory(int aSize) : size(aSize) {
    //    cout << "CMemory " << size << endl;
}

CMemory::~CMemory() {

}

CComponent* CMemory::clone() const {
    return new CMemory(size);
}

void CMemory::print(ostream &os, string prefix) const {

}

void CMemory::printCaption(ostream &os, string prefix) const {
    os << prefix << "Memory, " << size << " MiB" << endl;
}

/****************** CDisk         *********************************************/
CDisk::CDisk(char aType, int aSize) : type(aType), size(aSize) {

}

CDisk::~CDisk() {

}

void CDisk::print(ostream &os, string prefix) const {
    list<Partition>::size_type printed = 0;
    for (list<Partition>::const_iterator iter = partitions.begin();
            iter != partitions.end();
            iter++) {
        if (printed == partitions.size() - 1) {
            (*iter).print(os, prefix + "\\-");
        } else {
            (*iter).print(os, prefix + "+-");
        }

        printed++;
    }

}

void CDisk::printCaption(ostream &os, string prefix) const {
    if (type == MAGNETIC) {
        os << prefix << "HDD, " << size << " GiB" << endl;
    } else {
        os << prefix << "SSD, " << size << " GiB" << endl;
    }
}

CDisk& CDisk::AddPartition(int size, string ID) {
    //    cout << "Adding partition " << ID << ", " << size << endl;
    partitions.push_back(Partition(size, partitions.size(), ID));

    return *this;
}

CComponent* CDisk::clone() const {
    CDisk * res = new CDisk(type, size);

    for (list<Partition>::const_iterator iter = partitions.begin();
            iter != partitions.end(); iter++) {
        res -> AddPartition((*iter).size, (*iter).name);
    }

    return res;
}



/******************************************************************************/

/******************************************************************************/

#ifndef __PROGTEST__

int main() {
    
    CComputer tmp("04511264.btlkem.cz");
    tmp . AddAddress("5.10.93.212");
    tmp . AddAddress("5.9.159.95");
    
    cout << tmp << endl;
//+-CPU, 5 cores @ 1518MHz
//+-CPU, 15 cores @ 1801MHz
//+-Memory, 5624 MiB
//+-CPU, 8 cores @ 927MHz
//\-Memory, 2555 MiB

    
    
    
    
    
    

    //    CNetwork tmp("TEMP");
    //    
    //    return 0;


//    CNetwork n("FIT network");
//    n . AddComputer(
//            CComputer("progtest.fit.cvut.cz") .
//            AddAddress("147.32.232.142") .
//            AddComponent(CCPU(8, 2400)) .
//            AddComponent(CCPU(8, 1200)) .
//            AddComponent(CDisk(CDisk::MAGNETIC, 1500) .
//            AddPartition(50, "/") .
//            AddPartition(5, "/boot").
//            AddPartition(1000, "/var")) .
//            AddComponent(CDisk(CDisk::SSD, 60) .
//            AddPartition(60, "/data")) .
//            AddComponent(CMemory(2000)).
//            AddComponent(CMemory(2000))) .
//            AddComputer(
//            CComputer("edux.fit.cvut.cz") .
//            AddAddress("147.32.232.158") .
//            AddComponent(CCPU(4, 1600)) .
//            AddComponent(CMemory(4000)).
//            AddComponent(CDisk(CDisk::MAGNETIC, 2000) .
//            AddPartition(100, "/") .
//            AddPartition(1900, "/data"))) .
//            AddComputer(
//            CComputer("imap.fit.cvut.cz") .
//            AddAddress("147.32.232.238") .
//            AddComponent(CCPU(4, 2500)) .
//            AddAddress("2001:718:2:2901::238") .
//            AddComponent(CMemory(8000)));
//
//    cout << n;
//    cout << endl << endl << endl;
//
//    CNetwork cop("Copied");
//    cop . AddComputer(
//            CComputer("progtest.fit.cvut.cz") .
//            AddAddress("147.32.232.142") .
//            AddComponent(CCPU(8, 2400)) .
//            AddComponent(CCPU(8, 1200)) .
//            AddComponent(CDisk(CDisk::MAGNETIC, 1500) .
//            AddPartition(50, "/") .
//            AddPartition(5, "/boot").
//            AddPartition(1000, "/var")));
//    
//    cout << endl << endl << endl << "CALLING = OPERATOR" << endl << endl;
//    cop = n;
//    cout << endl << endl << "AFTER  = OPERATOR" << endl << endl;
//    
//
//    cout << cop;
//    return 0;
//
//    //    cout << n;
//    //    return 0;
//
//
//    /*
//    --8<----8<----8<----8<----8<----8<--
//    Network: FIT network
//    +-Host: progtest.fit.cvut.cz
//    | +-147.32.232.142
//    | +-CPU, 8 cores @ 2400MHz
//    | +-CPU, 8 cores @ 1200MHz
//    | +-HDD, 1500 GiB
//    | | +-[0]: 50 GiB, /
//    | | +-[1]: 5 GiB, /boot
//    | | \-[2]: 1000 GiB, /var
//    | +-SSD, 60 GiB
//    | | \-[0]: 60 GiB, /data
//    | +-Memory, 2000 MiB
//    | \-Memory, 2000 MiB
//    +-Host: edux.fit.cvut.cz
//    | +-147.32.232.158
//    | +-CPU, 4 cores @ 1600MHz
//    | +-Memory, 4000 MiB
//    | \-HDD, 2000 GiB
//    |   +-[0]: 100 GiB, /
//    |   \-[1]: 1900 GiB, /data
//    \-Host: imap.fit.cvut.cz
//      +-147.32.232.238
//      +-2001:718:2:2901::238
//      +-CPU, 4 cores @ 2500MHz
//      \-Memory, 8000 MiB
//    --8<----8<----8<----8<----8<----8<--
//     */
//    CNetwork x("name");
//    x = n;
//    CComputer * c = x . FindComputer("imap.fit.cvut.cz");
//
//    //    cout << *c;
//    //    return 0;
//    //    
//    /*
//    --8<----8<----8<----8<----8<----8<--
//    Host: imap.fit.cvut.cz
//    +-147.32.232.238
//    +-2001:718:2:2901::238
//    +-CPU, 4 cores @ 2500MHz
//    \-Memory, 8000 MiB
//    --8<----8<----8<----8<----8<----8<--
//     */
//    c -> AddComponent(CDisk(CDisk::MAGNETIC, 1000) .
//            AddPartition(100, "system") .
//            AddPartition(200, "WWW") .
//            AddPartition(700, "mail"));
//
//
//    cout << x;
//    //    return 0;
//
//    /*
//    --8<----8<----8<----8<----8<----8<--
//    Network: FIT network
//    +-Host: progtest.fit.cvut.cz
//    | +-147.32.232.142
//    | +-CPU, 8 cores @ 2400MHz
//    | +-CPU, 8 cores @ 1200MHz
//    | +-HDD, 1500 GiB
//    | | +-[0]: 50 GiB, /
//    | | +-[1]: 5 GiB, /boot
//    | | \-[2]: 1000 GiB, /var
//    | +-SSD, 60 GiB
//    | | \-[0]: 60 GiB, /data
//    | +-Memory, 2000 MiB
//    | \-Memory, 2000 MiB
//    +-Host: edux.fit.cvut.cz
//    | +-147.32.232.158
//    | +-CPU, 4 cores @ 1600MHz
//    | +-Memory, 4000 MiB
//    | \-HDD, 2000 GiB
//    |   +-[0]: 100 GiB, /
//    |   \-[1]: 1900 GiB, /data
//    \-Host: imap.fit.cvut.cz
//      +-147.32.232.238
//      +-2001:718:2:2901::238
//      +-CPU, 4 cores @ 2500MHz
//      +-Memory, 8000 MiB
//      \-HDD, 1000 GiB
//        +-[0]: 100 GiB, system
//        +-[1]: 200 GiB, WWW
//        \-[2]: 700 GiB, mail
//    --8<----8<----8<----8<----8<----8<--
//     */
//
//
//    cout << n;
//
//    /*
//    --8<----8<----8<----8<----8<----8<--
//    Network: FIT network
//    +-Host: progtest.fit.cvut.cz
//    | +-147.32.232.142
//    | +-CPU, 8 cores @ 2400MHz
//    | +-CPU, 8 cores @ 1200MHz
//    | +-HDD, 1500 GiB
//    | | +-[0]: 50 GiB, /
//    | | +-[1]: 5 GiB, /boot
//    | | \-[2]: 1000 GiB, /var
//    | +-SSD, 60 GiB
//    | | \-[0]: 60 GiB, /data
//    | +-Memory, 2000 MiB
//    | \-Memory, 2000 MiB
//    +-Host: edux.fit.cvut.cz
//    | +-147.32.232.158
//    | +-CPU, 4 cores @ 1600MHz
//    | +-Memory, 4000 MiB
//    | \-HDD, 2000 GiB
//    |   +-[0]: 100 GiB, /
//    |   \-[1]: 1900 GiB, /data
//    \-Host: imap.fit.cvut.cz
//      +-147.32.232.238
//      +-2001:718:2:2901::238
//      +-CPU, 4 cores @ 2500MHz
//      \-Memory, 8000 MiB
//    --8<----8<----8<----8<----8<----8<--
//     */

}
#endif