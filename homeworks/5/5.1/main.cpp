/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on April 6, 2013, 12:58 PM
 */

#ifndef __PROGTEST__
#include <cctype>
#include <iostream>
#include <iomanip>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */

template <typename _Type, typename _Comparator>
class TrieNode {
public:
    typedef typename _Type::value_type _ElmType;

    TrieNode(const _ElmType &val, TrieNode *aParent, const _Comparator &aCmp) :
    value(val),
    root(false),
    endID(-1),
    parent(aParent),
    fallNode(NULL),
    cmp(aCmp) {
    }

    TrieNode(const _ElmType &val, const _Comparator &aCmp) :
    value(val),
    root(true),
    endID(-1),
    parent(NULL),
    fallNode(NULL),
    cmp(aCmp) {
    }

    ~TrieNode() {
        for (unsigned int i = 0; i < children.size(); i++) {
            delete children[i];
        }
    }

    TrieNode* getChild(const typename _Type::value_type val) {
        for (unsigned int i = 0; i < children.size(); i++) {
            if (cmp(children[i]->value, val)) {
                return children[i];
            }
        }

        return NULL;
    }

    void addSequence(typename _Type::const_iterator startPos,
            typename _Type::const_iterator endPos, int endID) {

        if (startPos == endPos) {
            this -> endID = endID;
            return;
        }

        TrieNode * tmp = getChild(*startPos);

        if (tmp == NULL) { // This child not found

            tmp = new TrieNode(*startPos, this, cmp);
            children.push_back(tmp);
        }

        tmp -> addSequence(++startPos, endPos, endID);
    }

    bool contains(typename _Type::const_iterator startPos,
            typename _Type::const_iterator endPos) {

        if (startPos == endPos) {
            return endID > -1;
        }

        TrieNode * tmp = getChild(*startPos);
        if (tmp == NULL) {
            return false;
        } else {

            return tmp->contains(++startPos, endPos);
        }
    }

    vector<TrieNode *>& getChildren() {

        return children;
    }

    typename _Type::value_type getVal() {

        return value;
    }

    void print(int lvl) const {
        if (!root) {
            for (int i = 0; i < lvl; i++) {
                cout << " ";
            }

            cout << "level = " << lvl << " || " << value << " falls to ";
            if (fallNode != NULL) {
                if (fallNode -> isRoot()) cout << "ROOT";
                else cout << fallNode->getVal();
            }

            cout << endl;
        }

        for (int i = 0; i < children.size(); i++) {

            children[i] -> print(lvl + 1);
        }
    }

    void makeFallNodes() {
        if (!root) return;

        fallNode = this;
        queue <TrieNode*> fallQueue;
        fallQueue . push(this);

        while (!fallQueue.empty()) {
            TrieNode* tmp = fallQueue . front();
            fallQueue . pop();

            for (unsigned int i = 0; i < tmp -> children . size(); i++) {
                fallQueue . push(tmp -> children[i]);
            }

            if (tmp == this) continue;

            TrieNode * fall = tmp->parent->fallNode;
            while (fall -> getChild(tmp->value) == NULL && !fall->root) {
                fall = fall->fallNode;
            }
            tmp -> fallNode = fall->getChild(tmp->value);

            if (tmp -> fallNode == NULL) tmp -> fallNode = this;

            if (tmp -> fallNode == tmp) tmp->fallNode = this;
            //            if (tmp -> fallNode ->isRoot()) cout << "    " << tmp->value << "'s fall is now ROOT" << endl;
            //            else cout << "    " << tmp->value << "'s fall is now " << tmp->fallNode->value << endl;
        }
    }

    TrieNode *getFallNode() {

        return fallNode;
    }

    bool isRoot() {

        return root;
    }

    int getEndID() {

        return endID;
    }


private:
    _ElmType value;
    bool root;

    vector<TrieNode *> children;
    int endID;

    TrieNode * parent;
    TrieNode * fallNode;

    const _Comparator cmp;
};

template <typename _Type, typename _Comparator = std::equal_to<typename _Type::value_type> >
        class CSearch {
public:

    CSearch() : cmp(_Comparator()) {
        trie = NULL;
    }

    CSearch(const _Comparator &_cmp) : cmp(_cmp) {
        trie = NULL;
    }

    ~CSearch() {

        delete trie;
    }

    void Add(int id,
            const _Type & needle) {

        if (trie == NULL) {
            trie = new TrieNode<_Type, _Comparator > (*needle.begin(), cmp);
        }

                trie -> addSequence(needle . begin(), needle . end(), id);

                trie -> makeFallNodes();
    }

    set<int> Search(const _Type & hayHeap) const {
                typename _Type::const_iterator iter = hayHeap.begin();
                typename _Type::const_iterator end = hayHeap.end();
                TrieNode<_Type, _Comparator> *triePos = trie;
                TrieNode<_Type, _Comparator> *tmp;
        
                set<int> res;
        
                while (iter != end) {
        
                    while (triePos->getChild(*iter) == NULL && !triePos->isRoot()) {
                        //                cout << "  #1" << endl;
                        triePos = triePos->getFallNode();
                    }
        
                    if (triePos->isRoot()) {
                        triePos = triePos -> getChild(*iter);
                        if (triePos == NULL) triePos = trie;
                    } else {
                        triePos = triePos -> getChild(*iter);
                    }
        
                    tmp = triePos;
                    while (tmp != trie) {
                        if (tmp ->getEndID() > -1) {
        
                            res.insert(tmp -> getEndID());
                        }
                        tmp = tmp -> getFallNode();
                    }
        
                    iter++;
                }
        
                return res;

//        set<int> res;
//        res . insert(5);

        return res;

    }

private:
    const _Comparator &cmp;

    TrieNode<_Type, _Comparator> *trie;

    CSearch(const CSearch &other);
    CSearch &operator=(const CSearch &other);
};



#ifndef __PROGTEST__

class CharComparator {
public:

    CharComparator(bool caseSensitive = true)
    : m_CaseSensitive(caseSensitive) {
    }

    bool operator () (const char & a, const char & b) const {

        return m_CaseSensitive ? a == b : toupper(a) == toupper(b);
    }
private:
    bool m_CaseSensitive;
};

bool upperCaseCompare(const char & a, const char & b) {

    return toupper(a) == toupper(b);
}

void printSet(const set<int> & s) {

    for (set<int>::const_iterator it = s . begin(); it != s . end(); ++it)
        cout << (it == s . begin() ? "" : ", ") << *it;
    cout << endl;
}

template <typename _T, int _N>
vector<_T> makeVector(const _T(&data)[_N]) {

    return vector<_T > (data, data + _N);
}

class CTest {
public:

    CTest(int a) {
        cout << "CTest constructor" << endl;
    }
};

class CTestComparator {
public:

    CTestComparator(int a) {
    }

    bool operator () (const char & a, const char & b) const {

        return a == b;
    }
};

int main() {

    //    CSearch <vector<CTest>, CTestComparator > test0(CTestComparator());


        CSearch <string> test1;
        test1 . Add(0, "hello");
        test1 . Add(1, "world");
        test1 . Add(2, "rld");
        test1 . Add(3, "ell");
        test1 . Add(4, "hell");
        printSet(test1 . Search("hello world!"));
        // 0, 1, 2, 3, 4
        printSet(test1 . Search("hEllo world!"));
        // 1, 2
        cout << endl;
    
    
    
        CSearch <string, bool (*)(const char &, const char &) > test2(upperCaseCompare);
        test2 . Add(0, "hello");
        test2 . Add(1, "world");
        test2 . Add(2, "rld");
        test2 . Add(3, "ell");
        test2 . Add(4, "hell");
        printSet(test2 . Search("HeLlO WoRlD!"));
        // 0, 1, 2, 3, 4
        printSet(test2 . Search("hallo world!"));
        // 1, 2
        cout << endl;



    CSearch <string, CharComparator> test3;
    test3 . Add(0, "hello");
    test3 . Add(1, "world");
    test3 . Add(2, "rld");
    test3 . Add(3, "ell");
    test3 . Add(4, "hell");
    printSet(test3 . Search("heLLo world!"));
    // 0, 1, 2, 3, 4
    printSet(test3 . Search("Well, templates are hell"));
    // 3, 4
    cout << endl;
    //
        CSearch <vector<int> > test4;
        const int needleA [] = {1, 6, 1, 6, 9, 12};
        const int needleB [] = {9, 12, 7};
        const int hayHeap [] = {1, 6, 1, 6, 1, 6, 9, 12, 8};
    
    
        test4 . Add(0, makeVector(needleA));
        test4 . Add(1, makeVector(needleB));
        printSet(test4 . Search(makeVector(hayHeap)));
        //     0
        cout << endl;
    
    
        CSearch <vector<string> > test5;
        static const string needleX [] = {"Prague", "Bern", "Rome"};
        static const string needleY [] = {"London", "Prague", "Bern"};
        static const string needleZ [] = {"London", "Rome"};
        static const string cityHeap [] = {"Berlin", "London", "Prague", "Bern", "Rome", "Moscow"};
        test5 . Add(0, makeVector(needleX));
        test5 . Add(1, makeVector(needleY));
        test5 . Add(2, makeVector(needleZ));
        printSet(test5 . Search(makeVector(cityHeap)));
        // 0, 1
        cout << endl;


    return 0;
}
#endif