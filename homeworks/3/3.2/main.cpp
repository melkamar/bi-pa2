/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on March 26, 2013, 8:52 PM
 */

#ifndef __PROGTEST__
#include <new>
#include <cstring>
#include <stdint.h>

#include <iostream>
#include <stdlib.h>

using namespace std;
#endif /* __PROGTEST__ */

typedef uint16_t mint;

class CDegChar {
public:
    CDegChar();
    CDegChar(const CDegChar &other);
    ~CDegChar();

    CDegChar& operator =(const CDegChar &other);
    CDegChar& operator +=(const mint charID);
    CDegChar& operator -=(const mint charID);
    bool operator ==(const CDegChar &other) const;
    bool operator [] (const mint &charID) const;

    int Size()const;

    void printLow(); // debug vypisy
    void printHigh();
private:
    int low[3]; // bitove pole pro znaky 32..127
    unsigned char lowCount; // pocet prvku v poli

    mint * high; // serazene dynamicke pole pro 128..65535
    mint highSize; // alokovana velikost pole
    //    mint highCount; // pocet prvku v poli

    static int OFFSETS[];
    static int BITS_IN_INT;


    void putLow(const mint &ID); // metody pro praci s prvky 0..127
    bool getLow(const mint &ID) const;
    void delLow(const mint &ID);


    bool findHigh(const mint ID, mint * index) const;
    /* ^ uses*/ bool findElmIter(const mint ID, mint * pos) const;

    void putHigh(const mint &ID); // Vlozeni prvku ID
    /* ^ uses*/ void insertHigh(const mint &ID, mint &pos);

    void delHigh(const mint &ID); // Smazani prvku ID
    /* ^ uses*/ void delHighAtIndex(mint &pos); // Smazani prvku z pozice pos


    void resizeHigh(int newSize); // Zvetseni pole na 1.5nasobek
    void shrinkHigh(int newSize); // Zmenseni pole 
};

int CDegChar::OFFSETS[] = {
    1 << (0),
    1 << (0 + 1),
    1 << (0 + 1 + 1),
    1 << (0 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
    1 << (0 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1
    + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1),
};

int CDegChar::BITS_IN_INT = (sizeof (int) *8);

CDegChar::CDegChar() {
    for (int i = 0; i < 3; i++)
        low[i] = 0;

    highSize = 0;
    //    highCount = 0;
    lowCount = 0;
}

CDegChar::CDegChar(const CDegChar& other) {

    //    highCount = other.highCount;
    highSize = other.highSize;
    lowCount = other.lowCount;

    if (highSize > 0) {
        high = new mint[highSize];

        memcpy(high, other.high, highSize * sizeof (mint));
    }

    memcpy(low, other.low, 3 * sizeof (int));
}

CDegChar::~CDegChar() {
    if (highSize > 0)
        delete[]high;
}

CDegChar& CDegChar::operator =(const CDegChar &other) {
    if (*this == other) return (*this);

    //    cout << "OPERATOR=: " << endl;
    //    cout << "other.highCount: " << other.highCount << endl;
    //    cout << "other.highSize:: " << other.highSize << endl;

    if (highSize > 0) delete[]high;

    if (other.highSize > 0) { // Other set does use high vals
        high = new mint[other.highSize];
        memcpy(high, other.high, other.highSize * sizeof (mint));
    }

    //    highCount = other.highCount;
    highSize = other.highSize;
    lowCount = other.lowCount;

    memcpy(low, other.low, 3 * sizeof (int));

    return (*this);
}

CDegChar& CDegChar::operator +=(const mint charID) {
//    cout << "+= " << charID << endl;
    if (charID >= 32 && charID < 128) {
        putLow(charID);
    } else {
        putHigh(charID);
    }
//    cout << "+= done."<<endl<<endl<<endl;

    return (*this);
}

CDegChar& CDegChar::operator -=(const mint charID) {
//    cout << "-= " << charID << endl;
    if (charID >= 32 && charID < 128) {
        delLow(charID);
    } else {
        delHigh(charID);
    }

    return (*this);
}

bool CDegChar::operator ==(const CDegChar &other) const {
    if (lowCount != other.lowCount || highSize != other.highSize) {
        return false;
    } else {
        if (lowCount == 0 && highSize == 0) {
            return true;
        }

    }

    // if sizes are the same
    if (memcmp(low, other.low, 3 * sizeof (int)))return false;
    if (memcmp(high, other.high, highSize * sizeof (mint))) return false;

    return true;
}

bool CDegChar::operator [] (const mint &charID) const {
    if (charID >= 32 && charID < 128) {
        return getLow(charID);
    } else {
        mint tmp;
        return findHigh(charID, &tmp);
    }
}

int CDegChar::Size() const {
    return lowCount + highSize;
}

void CDegChar::putLow(const mint& ID) {
//    cout << "    putLow"<<endl;
    bool res = getLow(ID);
    if (!res) {
//        cout << "      inside branch"<<endl;
        int arr = ID / BITS_IN_INT-1; // ktery prvek pole pouzit
        //low[arr] |= (1 << (ID % (sizeof (int) *8)));
        low[arr] |= OFFSETS[ID % BITS_IN_INT];
        lowCount++;
//        cout << "      ending branch"<<endl;
    }
    
//    cout << "    putLow done."<<endl;
}

bool CDegChar::getLow(const mint& ID) const {
//    cout <<"      getLow"<<endl;
    int arr = ID / BITS_IN_INT-1; // ktery prvek pole pouzit
    bool res = ((low[arr] & (OFFSETS[ID % BITS_IN_INT])) != 0);
//    cout <<"      getLow done"<<endl;
    return res;
}

void CDegChar::delLow(const mint & ID) {
    if (getLow(ID)) {
        int arr = ID / BITS_IN_INT-1; // ktery prvek pole pouzit
        //low[arr] &= (~(1 << (ID % (sizeof (int) *8))));
        low[arr] &= (~(OFFSETS[ID % BITS_IN_INT]));
        lowCount--;
    }
}

void CDegChar::printLow() { // pro debug vypis
    for (int i = 0; i < 128; i++) {
        if (i % (sizeof (int) *8) == 0) {

            cout << endl << "  ";
        }
        cout << getLow(i);
    }
    cout << endl;
}

void CDegChar::printHigh() { // pro debug vypis
    for (int i = 0; i < highSize; i++)
        cout << "  " << high[i] << endl;
}

bool CDegChar::findHigh(const mint ID, mint * index) const {
    if (highSize == 0) {
        *index = 0;
        return false;
    }

    return findElmIter(ID, index);
}

bool CDegChar::findElmIter(const mint ID, mint * pos) const {

    int min = 0;
    int max = highSize - 1;

    //    cout << "-------------- findElm, " << min << " -> " << max << endl;

    while (max > min && max >= 0) {
        int mid = (max + min) / 2;
        //        pos = mid;

        if (high[mid] < ID) {
            min = mid + 1;
        } else if (high[mid] > ID) {
            max = mid - 1;
        } else {
            *pos = mid;
            return true;
        }
    }


    //     Klic nebyl nalezen -> urceni, kam patri
    //    cout << "    --- high[min] = " << high[min] << endl;
    if (high[min] < ID) {
        *pos = min + 1;
        return false;
    } else if (high[min] > ID) {
        *pos = min;
        return false;
    } else {
        *pos = min;
        return true;
    }
}

void CDegChar::putHigh(const mint & ID) {
    mint pos;
    bool found = findHigh(ID, &pos);
    if (found) return;

    //    resizeHigh();

    insertHigh(ID, pos);
}

void CDegChar::insertHigh(const mint & ID, mint & pos) {

    //    cout << "insertHigh, cur highSize: " << highSize << endl;
    resizeHigh(highSize + 1);

    //    cout << "  resized. memmoving: pos=" << pos << ", count=" << highSize - pos << endl;
    memmove(&(high[pos + 1]), &(high[pos]), (highSize - pos) * sizeof (mint));
    high[pos] = ID;

    highSize++;
    //    highCount++;
}

void CDegChar::delHigh(const mint & ID) {
    //    cout << "delHigh (" << ID << ")" << endl;
    //    cout << "  -- highSize = " << highSize << endl;
    //    cout << "  -- high[0] == " << high[0] << endl;

    mint pos;
    bool found = findHigh(ID, &pos);

    if (!found) return;
    delHighAtIndex(pos);

    // Pokud je pole vic nez z pulky nevyuzite, zkratit
    //    if (highCount == 0 || (highSize / highCount >= 2)) shrinkHigh();
    //    shrinkHigh();
    shrinkHigh(highSize - 1);
    highSize--;
}

void CDegChar::delHighAtIndex(mint & pos) {
    //    cout << "  deleting high at index " << pos << " (" << high[pos] << ")" << endl << endl;
    memmove(&(high[pos]), &(high[pos + 1]), sizeof (mint)*(highSize - 1 - pos));
    //    cout << "  new item at index " << pos << " (" << high[pos] << ")" << endl << endl;
}

void CDegChar::resizeHigh(int newSize) {
    //    cout << "resizeHigh(" << newSize << ")" << endl;
    if (newSize == 1) {
        high = new mint[newSize];
    } else {
        //        cout << "  memcpying " << newSize - 1 << "items" << endl;
        mint * tmp = new mint[newSize];
        memcpy(tmp, high, (newSize - 1) * sizeof (mint));
        delete[]high;
        high = tmp;
    }

    //    cout << "done resizing." << endl;
}

void CDegChar::shrinkHigh(int newSize) {
    //    cout << "shrinkHigh (" << newSize << ")" << endl;
    if (newSize == 0) {
        //        high = new mint[newSize];
        delete [] high;
    } else {
        //        cout << "  memcpying " << newSize << "items" << endl;
        mint * tmp = new mint[newSize];
        memcpy(tmp, high, (newSize) * sizeof (mint));
        delete[]high;
        high = tmp;
    }

    //    cout << "done shrinking." << endl;
}

#ifndef __PROGTEST__

int main(int argc, char** argv) {

    CDegChar a;
    CDegChar b;
    b += 1503;

//    for (int p = 0; p < 1; p++) {
//        for (int y = 0; y < 300; y++) {
//            a += y;
//        }
//
//        for (int y = 0; y < 300; y++) {
//            a -= y;
//        }
//    }
//    return 0;

    //        bool status;

        for (int x = 0; x < 10000; x++) {
            CDegChar a1;
    
            for (int i = 0; i < 100; i++) {
                //            cout << "i = " << i << endl;
                random();
                int op = rand() % 4;
                //            int num = rand() % 65536;
                random();
                int num = rand() % 65536;
    
                switch (op) {
                    case 0:
                        a1 -= num;
                        break;
    
                    default:
                        a1 += num;
                        break;
                }
    
                //            cout << "_____________________ " << a1.Size() << endl;
            }
            //        cout << "DONE" << endl;
        }
    return 0;
}
#endif