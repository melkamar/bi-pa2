/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on April 7, 2013, 12:54 PM
 */

#ifndef __PROGTEST__
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
using namespace std;

class CTimeStamp {
public:
    CTimeStamp(int year,
            int month,
            int day,
            int hour,
            int minute,
            double sec);

    int Compare(const CTimeStamp & x) const;
    friend ostream & operator <<(ostream & os,
            const CTimeStamp & x);
private:
    int year;
    int month;
    int day;
    int hour;
    int minute;
    double sec;
};

class CMail {
public:
    CMail(const CTimeStamp & timeStamp,
            const string & from,
            const string & to);
    int CompareTimeStamp(const CTimeStamp & x) const;
    int CompareByTimeStamp(const CMail & x) const;
    const string & From(void) const;
    const string & To(void) const;
    const CTimeStamp & TimeStamp(void) const;
    friend ostream & operator <<(ostream & os,
            const CMail & x);
private:
    CTimeStamp m_timeStamp;
    string m_from;
    string m_to;
};

CTimeStamp::CTimeStamp(int year, int month, int day, int hour, int minute, double sec) {
    this -> year = year;
    this -> month = month;
    this -> day = day;
    this -> hour = hour;
    this -> minute = minute;
    this -> sec = sec;
}

int CTimeStamp::Compare(const CTimeStamp& x) const {
    int res;

    //    cout << "Comparing " << *this << " to " << x << endl;

    res = year - x.year;
    if (res != 0) {
        //        cout << "  Year: " << res << endl;
        return res;
    }

    res = month - x.month;
    if (res != 0) {
        //        cout << "  Month: " << res << endl;
        return res;
    }

    res = day - x.day;
    if (res != 0) {
        //        cout << "  Day: " << res << endl;
        return res;
    }

    res = hour - x.hour;
    if (res != 0) {
        //        cout << "  Hour: " << res << endl;
        return res;
    }

    res = minute - x.minute;
    if (res != 0) {
        //        cout << "  Minute: " << res << endl;
        return res;
    }

    return (sec > x.sec ? 1 : (sec == x.sec) ? 0 : -1);
}

ostream & operator <<(ostream & os, const CTimeStamp & x) {
    os << setfill('0');

    os << setw(4) << x.year;
    os << "-";

    os << setw(2) << x.month;
    os << "-";

    os << setw(2) << x.day;
    os << " ";

    os << setw(2) << x.hour;
    os << ":";

    os << setw(2) << x.minute;
    os << ":";

    //    os << setw(5) << setprecision(3) << x.sec;
    os.setf(ios::fixed);
    os.precision(3);
    os << setw(6) << x.sec;
    
    return os;
}

/*               CMAIL  *******************************************************/

CMail::CMail(const CTimeStamp & timeStamp, const string & from, const string & to) :
m_timeStamp(timeStamp),
m_from(from),
m_to(to) {

}

int CMail::CompareTimeStamp(const CTimeStamp & x) const {
    return m_timeStamp.Compare(x);
}

int CMail::CompareByTimeStamp(const CMail & x) const {
    return m_timeStamp.Compare(x.m_timeStamp);
}

const string & CMail::From(void) const {
    return m_from;
}

const string & CMail::To(void) const {
    return m_to;
}

const CTimeStamp & CMail::TimeStamp(void) const {
    return m_timeStamp;
}

ostream & operator <<(ostream & os, const CMail & x) {
    os << x.m_timeStamp << " " << x.m_from << " -> " << x.m_to;
    return os;
}





#endif /* __PROGTEST__ */   


#define TYPE_FAIL -1
#define TYPE_FROM 0
#define TYPE_TO 1

struct CLogRecord {

    CLogRecord(string awho, CTimeStamp & atimeStamp, string aID) :
    who(awho),
    timeStamp(atimeStamp),
    ID(aID) {
    }

    bool operator<(const CLogRecord & other) const {
        return ID < other.ID;
    }

    bool operator==(const CLogRecord & other) const {
        return ID == other.ID;
    }

    bool operator>(const CLogRecord & other) const {
        return ID > other.ID;
    }


    string who;
    CTimeStamp timeStamp;
    string ID;
};

class CMailLog {
public:
    CMailLog();
    ~CMailLog();
    int ParseLog(istream & in);
    list<CMail> ListMail(const CTimeStamp & from,
            const CTimeStamp & to) const;
    set<string> ActiveUsers(const CTimeStamp & from,
            const CTimeStamp & to) const;
private:
    list<CMail> mails;

    int monthStrToInt(const string &month) const;
    int getType(istream & in);
};

bool mailComparator(const CMail & a, const CMail & b) {
    return a.CompareByTimeStamp(b) < 0;
}

CMailLog::CMailLog() {

}

CMailLog::~CMailLog() {

}

int CMailLog::ParseLog(istream & in) {
    list<CLogRecord> from;
    list<CLogRecord> to;

    //    cout << "Parse log" << endl << endl;

    int linesParsed = 0;
    string lineStr;
    while (getline(in, lineStr), !in.eof()) {
        istringstream is(lineStr);
        int year;
        int month;
        string monthStr;
        int day;
        int hour;
        int minute;
        double second;
        string ID;
        int msgType;
        string name;
        char tmp;
        string tmpStr;

        //        cout << endl << endl << "  this line: [" << lineStr << "]" << endl;


        is >> monthStr;
        if ((month = monthStrToInt(monthStr)) == -1) continue;
        //        cout << "    month: " << month << endl;

        is >> day;
        //        cout << "      day: " << day << endl;
        is >> year;
        //        cout << "     year: " << year << endl;

        is >> hour;
        //        cout << "     hour: " << hour << endl;


        is >> tmp;
        if (tmp != ':') continue;


        is >> minute;
        //        cout << "      min: " << minute << endl;


        is >> tmp;
        if (tmp != ':') continue;

        is >> second;
        //        cout << "      sec: " << second << endl;

        is >> tmpStr;

        is >> ID;
        //        cout << "       ID: " << ID << endl;

        if ((msgType = getType(is)) == TYPE_FAIL) continue;

        //        cout << "     type: " << msgType << endl;

        is >> name;
        //        cout << "     name: " << name << endl;

        if (!is.good() && !is.eof()) continue;

        CTimeStamp newStamp(year, month, day, hour, minute, second);
        if (msgType == TYPE_FROM) {
            from . push_back(CLogRecord(name, newStamp, ID));
            //            cout << "  Adding FROM: " << name << "  --  " << newStamp << endl;
        } else {
            to . push_back(CLogRecord(name, newStamp, ID));
            //            cout << "  Adding TO: " << name << "  --  " << newStamp << endl;
        }

        linesParsed++;
    }

    //    cout << "Parsing done! Lines processed: " << linesParsed << endl;



    //    cout << endl << endl;
    //    cout << "Printout FROM:" << endl;
    //    for (list<CLogRecord>::const_iterator iter = from.begin(); iter != from.end(); iter++) {
    //        cout << "  " << (*iter).ID << " - " << (*iter).who << endl;
    //    }
    //
    //    cout << endl << endl;
    //    cout << "Printout TO:" << endl;
    //    for (list<CLogRecord>::const_iterator iter = to.begin(); iter != to.end(); iter++) {
    //        cout << "  " << (*iter).ID << " - " << (*iter).who << endl;
    //    }
    //
    //    cout << endl << endl;
    //    cout << endl << endl;
    //
    //    cout << " ......... SORTED ............." << endl;
    //
    //    from.sort();
    //    to.sort();
    //
    //    cout << endl << endl;
    //    cout << "Printout FROM:" << endl;
    //    for (list<CLogRecord>::const_iterator iter = from.begin(); iter != from.end(); iter++) {
    //        cout << "  " << (*iter).ID << " - " << (*iter).who << endl;
    //    }
    //
    //    cout << endl << endl;
    //    cout << "Printout TO:" << endl;
    //    for (list<CLogRecord>::const_iterator iter = to.begin(); iter != to.end(); iter++) {
    //        cout << "  " << (*iter).ID << " - " << (*iter).who << endl;
    //    }

    from . sort();
    to . sort();







    // Basically a merge sort
    list <CLogRecord>::const_iterator fromIter = from . begin();
    list <CLogRecord>::const_iterator toIter = to . begin();

    int parsedCount = 0;
    while (fromIter != from.end() && toIter != to.end()) {
        //        cout << "  while loop." << endl;
        //        cout << "    from: " << (*fromIter).ID << endl;
        //        cout << "      to: " << (*toIter).ID << endl;
        if (*fromIter == *toIter) {
            //            cout << "   match." << endl;
            CMail tmp((*toIter).timeStamp, (*fromIter).who, (*toIter).who);
            mails . push_back(tmp);
            toIter++;
            parsedCount++;
        } else {
            if (*fromIter < *toIter) {
                if (fromIter == from.end()) break;
                fromIter++;
                //                cout << "   fromIter++." << endl;
            } else {
                if (toIter == to.end()) break;
                toIter++;
                //                cout << "   toIter++." << endl;
            }
        }
    }

//    cout << endl << " ---------- MAILS ------------- " << endl;
//    for (list<CMail>::const_iterator iter = mails.begin(); iter != mails.end(); iter++) {
//        cout << "  " << *iter << endl;
//    }

    mails.sort(mailComparator);

//    cout << endl << " ---------- MAILS ------------- " << endl;
//    for (list<CMail>::const_iterator iter = mails.begin(); iter != mails.end(); iter++) {
//        cout << "  " << *iter << endl;
//    }

    return parsedCount;
}

list<CMail> CMailLog::ListMail(const CTimeStamp & from,
        const CTimeStamp & to) const {

    list<CMail> ::const_iterator start = mails.begin();
    while ((*start).TimeStamp() .Compare(from) < 0 && start != mails.end()) {
        start++;
    }

    list<CMail> res;
    while ((*start).TimeStamp() . Compare(to) < 0 && start != mails.end()) {
        //        cout << "Adding " << *start << endl;
        res . push_back(*start);
        start++;
    }


    return res;


}

set<string> CMailLog::ActiveUsers(const CTimeStamp & from,
        const CTimeStamp & to) const {

    set<string> users;


    list<CMail> ::const_iterator start = mails.begin();
    while ((*start).TimeStamp() .Compare(from) < 0 && start != mails.end()) {
        start++;
    }


    while ((*start).TimeStamp() . Compare(to) < 0 && start != mails.end()) {
        //        cout << "Adding " << *start << endl;
        //        res . push_back(*start);
        users.insert((*start).From());
        users.insert((*start).To());
        start++;
    }


    return users;

}

int CMailLog::getType(istream & in) {
    char tmp;
    ostringstream is;

    in >> skipws;
    while (!in.eof()) {
        in >> tmp;
        if (tmp != '=') {
            is << tmp;
        } else {
            break;
        }
    }

    //    cout << "  getType (" << is.str() << ") -> ";

    if (is.str().compare("from") == 0) {
        //        cout << " FROM " << endl;
        return TYPE_FROM;
    } else if (is.str().compare("to") == 0) {
        //        cout << " TO " << endl;
        return TYPE_TO;
    } else {
        //        cout << " FAIL " << endl;
        return TYPE_FAIL;
    }
}

int CMailLog::monthStrToInt(const string & month) const {
    if (month.compare("Jan") == 0) return 1;
    else if (month.compare("Feb") == 0) return 2;
    else if (month.compare("Mar") == 0) return 3;
    else if (month.compare("Apr") == 0) return 4;
    else if (month.compare("May") == 0) return 5;
    else if (month.compare("Jun") == 0) return 6;
    else if (month.compare("Jul") == 0) return 7;
    else if (month.compare("Aug") == 0) return 8;
    else if (month.compare("Sep") == 0) return 9;
    else if (month.compare("Oct") == 0) return 10;
    else if (month.compare("Nov") == 0) return 11;
    else if (month.compare("Dec") == 0) return 12;
    else return -1;
}



#ifndef __PROGTEST__

void showMail(const list<CMail> & l) {
    for (list<CMail>::const_iterator it = l . begin(); it != l . end(); ++it)
        cout << *it << endl;
}

void showUsers(const set<string> & s) {
    for (set<string>::const_iterator it = s . begin(); it != s . end(); ++it)
        cout << *it << endl;
}

int main() {
    CMailLog m;
    int cnt;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;

    iss . clear();
    iss . str(string(
            "Mar 29 2013 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
            "Mar 29 2013 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
            "Mar 29 2013 12:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
            "Mar 29 2013 13:48:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
            "Mar 29 2013 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
            "Mar 29 2013 14:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
            "Mar 29 2013 14:58:32.563 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
            "Mar 29 2013 15:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
            "Mar 29 2013 15:02:34.232 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
            "Mar 29 2013 15:04:18.345 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
            ));
    cnt = m . ParseLog(iss);
    cout << cnt << endl;
//     cnt = 5;
    mailList = m . ListMail(CTimeStamp(2013, 3, 28, 0, 0, 0),
            CTimeStamp(2013, 3, 29, 23, 59, 59));
//    showMail(mailList);
    /*
    ---8<---8<---8<---8<---8<---8<---
    2013-03-29 12:55:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz
    2013-03-29 13:48:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
    2013-03-29 14:58:32.563 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz
    2013-03-29 15:02:34.232 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz
    2013-03-29 15:04:18.345 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz
    ---8<---8<---8<---8<---8<---8<---
     */

//    cout << endl << endl << endl << endl;


    mailList = m . ListMail(CTimeStamp(2013, 3, 28, 0, 0, 0),
            CTimeStamp(2013, 3, 29, 14, 59, 59));
//    showMail(mailList);
    /*
    ---8<---8<---8<---8<---8<---8<---
    2013-03-29 12:55:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz
    2013-03-29 13:48:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
    2013-03-29 14:58:32.563 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz
    ---8<---8<---8<---8<---8<---8<---
     */



//    cout << endl << endl << endl << endl;



    mailList = m . ListMail(CTimeStamp(2013, 3, 30, 0, 0, 0),
            CTimeStamp(2013, 3, 30, 23, 59, 59));
//    showMail(mailList);
    /*
    ---8<---8<---8<---8<---8<---8<---
    ---8<---8<---8<---8<---8<---8<---
     */


//    cout << endl << endl << endl << endl;



        users = m . ActiveUsers(CTimeStamp(2013, 3, 28, 0, 0, 0),
                CTimeStamp(2013, 3, 29, 23, 59, 59));
//        showUsers(users);
        /*
        ---8<---8<---8<---8<---8<---8<---
        CEO@fit.cvut.cz
        CIO@fit.cvut.cz
        HR-department@fit.cvut.cz
        PR-department@fit.cvut.cz
        boss13@fit.cvut.cz
        office13@fit.cvut.cz
        person3@fit.cvut.cz
        user76@fit.cvut.cz
        ---8<---8<---8<---8<---8<---8<---
         */
        
        
        
//        cout << endl << endl << endl << endl;
        
        
        
        
        users = m . ActiveUsers(CTimeStamp(2013, 3, 28, 0, 0, 0),
                CTimeStamp(2013, 3, 29, 13, 59, 59));
//        showUsers(users);
        /*
        ---8<---8<---8<---8<---8<---8<---
        boss13@fit.cvut.cz
        office13@fit.cvut.cz
        person3@fit.cvut.cz
        user76@fit.cvut.cz
        ---8<---8<---8<---8<---8<---8<---
         */


    return 0;
}

#endif