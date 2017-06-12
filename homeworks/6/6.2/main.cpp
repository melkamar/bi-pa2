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

class CNetwork;
class CComputer;

class CComponent {
public:

    virtual ~CComponent() {
    }

    virtual void print(ostream &os, string prefix) const {
        os << "CComponent print!" << endl;
    }

    virtual void printCaption(ostream &os, string prefix) const {
    }

    virtual CNetwork* FindNetwork(string ID) {
        return NULL;
    };

    virtual CComputer* FindComputer(string ID) {
        return NULL;
    };

    virtual CComponent* clone() const {
        cout << "CComponent clone." << endl;
        return NULL;
    }

    virtual CComponent* clone(map<string, string> &remap) const {
        cout << "CComponent remap clone." << endl;
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

    CComputer Duplicate(map<string, string> remap);

    CComputer* clone() const;
    CComputer* clone(map<string, string> &remap) const;

    CNetwork* FindNetwork(string ID);
    CComputer* FindComputer(string ID);

    void print(ostream &os, string prefix) const;
    void printCaption(ostream &os, string prefix) const;

    friend ostream& operator<<(ostream & os, const CComputer & toPrint);
protected:
    string name;
    list<string> addrs;
    list<CComponent*> components;
};

class CNetwork : public CComponent {
public:
    CNetwork(string aName);
    ~CNetwork();
    CNetwork(const CNetwork &orig);

    CNetwork& AddComputer(const CComputer &toAdd);
    CComputer* FindComputer(string ID);

    CNetwork* FindNetwork(string ID);

    virtual CComponent* clone() const;
    virtual CComponent* clone(map<string, string> &remap) const;

    void print(ostream &os, string prefix)const;
    void printCaption(ostream &os, string prefix) const;

    CNetwork& operator=(const CNetwork &orig);

    friend ostream& operator<<(ostream &os, const CNetwork &toPrint);
protected:
    string name;
    list<CComputer*> computers;
};

class CCPU : public CComponent {
public:
    CCPU(int aCores, int aFreq);
    virtual ~CCPU();
    virtual CComponent* clone() const;
    virtual CComponent* clone(map<string, string> &remap) const;
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
    virtual CComponent* clone(map<string, string> &remap) const;
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
    virtual CComponent* clone(map<string, string> &remap) const;
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

CNetwork::~CNetwork() {
    for (list<CComputer*>::const_iterator iter = computers.begin();
            iter != computers.end();
            iter++) {
        delete (*iter);
    }
}

CNetwork::CNetwork(const CNetwork& orig) : name(orig.name) {
    for (list<CComputer*>::const_iterator iter = orig.computers.begin();
            iter != orig.computers.end();
            iter++) {

        computers . push_back((*iter) -> clone());
    }
}

CNetwork& CNetwork::operator=(const CNetwork &orig) {
    if (this == &orig) {
        return *this;
    }

    for (list<CComputer*>::const_iterator iter = computers.begin();
            iter != computers.end();
            iter++) {
        delete (*iter);
    }
    computers.clear();


    for (list<CComputer*>::const_iterator iter = orig.computers.begin();
            iter != orig.computers.end();
            iter++) {

        computers . push_back((*iter) -> clone());
    }

    name = orig.name;

    return *this;
}

CComponent* CNetwork::clone() const {
    CNetwork * res = new CNetwork(name);

    for (list<CComputer*>::const_iterator iter = computers.begin();
            iter != computers.end();
            iter++) {

        res -> AddComputer(**iter);
    }


    return res;
}

CComponent* CNetwork::clone(map<string, string> &remap) const {

    map<string, string>::const_iterator iter = remap.find(name);

    CNetwork *res;
    if (iter != remap.end()) {
        res = new CNetwork(iter -> second);
    } else {
        res = new CNetwork(name);
    }

    for (list<CComputer*>::const_iterator iter = computers.begin();
            iter != computers.end();
            iter++) {

        res -> computers.push_back((**iter).clone(remap));
    }


    return res;
}

CNetwork& CNetwork::AddComputer(const CComputer &toAdd) {
    computers.push_back(toAdd.clone());

    return *this;
}

CComputer* CNetwork::FindComputer(string ID) {
    CComputer * res = NULL;
    for (list<CComputer*>::iterator iter = computers.begin();
            iter != computers.end(); iter++) {
        res = (*iter) -> FindComputer(ID);
        if (res != NULL) return res;
        //        if ((*iter)->getName().compare(ID) == 0) return (*iter);
    }

    return NULL;
}

CNetwork* CNetwork::FindNetwork(string ID) {

    if (ID.compare(name) == 0) return this;

    CNetwork * res = NULL;
    for (list<CComputer*>::const_iterator iter = computers.begin();
            iter != computers.end(); iter++) {
        res = (*iter) -> FindNetwork(ID);
        if (res != NULL) return res;
    }

    return NULL;
}

void CNetwork::print(ostream &os, string prefix) const {

    list<CComputer*>::size_type printed = 0;
    for (list<CComputer*>::const_iterator iter = computers.begin();
            iter != computers.end();
            iter++) {
        if (printed == computers.size() - 1) {
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
}

CComputer::~CComputer() {
    for (list<CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {
        delete (*iter);
    }
}

CComputer::CComputer(const CComputer &orig) : name(orig.name), addrs(orig.addrs) {
    for (list<CComponent*>::const_iterator iter = orig.components.begin();
            iter != orig.components.end();
            iter++) {

        components . push_back((*iter) -> clone());
    }
}

CComputer* CComputer::clone() const {
    CComputer * res = new CComputer(name);

    res -> addrs = addrs;

    for (list<CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {

        res -> AddComponent(**iter);
    }

    return res;
}

CComputer* CComputer::clone(map<string, string> &remap) const {
    //    cout << "Cloning CComputer" << endl;
    //    cout << "name = " << flush << name << endl;

    map<string, string>::const_iterator iter;
    iter = remap.find(name);
    //    cout << "Found iter" << endl;

    string newName;
    if (iter != remap.end()) {
        //        cout << "Iter not end, ==" << iter->second << endl;
        newName = iter -> second;
    } else {
        newName = name;
    }
    
    CComputer * res = new CComputer(newName);

    res -> addrs = addrs;

    for (list<CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {

        res -> components.push_back((**iter).clone(remap));
    }

    return res;
}

CComputer CComputer::Duplicate(map<string, string> remap) {
    map<string, string>::const_iterator iter;
    iter = remap.find(name);
    //    cout << "Found iter" << endl;

    string newName;
    if (iter != remap.end()) {
        //        cout << "Iter not end, ==" << iter->second << endl;
        newName = iter -> second;
    } else {
        newName = name;
    }
    
    CComputer res(newName);

    res . addrs = addrs;

    for (list<CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {

        res . components.push_back((**iter).clone(remap));
    }

    return res;
}

CComputer& CComputer::operator=(const CComputer &orig) {
    if (&orig == this) {
        return *this;
    }

    for (list<CComponent*>::const_iterator iter = components.begin();
            iter != components.end();
            iter++) {
        delete (*iter);
    }
    components.clear();

    for (list<CComponent*>::const_iterator iter = orig.components.begin();
            iter != orig.components.end();
            iter++) {
        components . push_back((*iter) -> clone());
    }

    name = orig.name;
    addrs = orig.addrs;

    return *this;

}

CNetwork* CComputer::FindNetwork(string ID) {

    CNetwork * res = NULL;
    for (list<CComponent*>::iterator iter = components.begin();
            iter != components.end();
            iter++) {
        res = (*iter) -> FindNetwork(ID);
        if (res != NULL) return res;
    }

    return NULL;
}

CComputer* CComputer::FindComputer(string ID) {
    if (name == ID) return this;

    CComputer * res = NULL;
    for (list<CComponent*>::iterator iter = components.begin();
            iter != components.end();
            iter++) {
        res = (*iter) -> FindComputer(ID);
        if (res != NULL) return res;
    }

    return NULL;

}

CComputer& CComputer::AddComponent(const CComponent &newComponent) {
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
    list<CComponent*>::size_type printed = 0;
    for (list<CComponent*>::const_iterator iter = components.begin();
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

CComponent* CCPU::clone(map<string, string> &remap) const {
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

CComponent* CMemory::clone(map<string, string> &remap) const {
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

CComponent* CDisk::clone(map<string, string> &remap) const {
    CDisk * res = new CDisk(type, size);

    for (list<Partition>::const_iterator iter = partitions.begin();
            iter != partitions.end(); iter++) {
        res -> AddPartition((*iter).size, (*iter).name);
    }

    return res;
}


/******************************************************************************/

/******************************************************************************/

