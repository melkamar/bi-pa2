/* 
 * File:   main.cpp
 * Author: magicmaster
 *
 * Created on March 24, 2013, 9:50 AM
 */
#ifndef __PROGTEST__
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

/*
 * 
 */
#endif // PROGTEST

//#define CLINE_INVALID_INDEX -1

class CSizeException {
public:
    CSizeException(int heightA, int widthA, char operation, int heightB, int widthB);
    friend ostream& operator<<(ostream &out, const CSizeException &exc);
private:
    int heightA;
    int widthA;
    char operation;
    int heightB;
    int widthB;
};

class CIndexException {
public:
    CIndexException(int index0, int index1);
    friend ostream& operator<<(ostream &out, const CIndexException &exc);
private:
    int index0, index1;
};

class COutLine {
public:
    COutLine(double *data, int len, int firstIndex, bool firstOK);
    ~COutLine();

    double& operator[](const int index);
    const double operator[](const int index) const;
private:
    int len;
    double * line;
    int firstIndex;
    bool firstOK;
};

class CMatrix {
public:
    CMatrix(int height, int width);
    CMatrix(const CMatrix &orig);
    ~CMatrix();

    CMatrix& operator=(const CMatrix &other);
    CMatrix operator-() const;
    CMatrix operator+(const CMatrix &b) const;
    CMatrix operator-(const CMatrix &b) const;
    CMatrix operator*(const CMatrix &b) const;
//    CMatrix operator*(const CMatrix &b) const;
    CMatrix operator*(const double x) const;
    //    friend CMatrix operator+(const CMatrix &a, const CMatrix &b);
    //    friend CMatrix operator-(const CMatrix &a, const CMatrix &b);
    //    friend CMatrix operator*(const CMatrix &a, const CMatrix &b);
    //    friend CMatrix operator*(const CMatrix &a, const double x);
    //    friend CMatrix operator*(const double x, const CMatrix &a);

    COutLine operator[](const int index);
    const COutLine operator[](const int index) const;

    friend ostream& operator<<(ostream &out, const CMatrix& a);
    friend istream& operator>>(istream &in, CMatrix& a);
private:
    void deleteData();
    void allocData(int height, int width);

    int rows, cols;
    double** data;
};

/******************************************************************************/

CIndexException::CIndexException(int index0, int index1) {
    this->index0 = index0;
    this->index1 = index1;
}

CSizeException::CSizeException(int heightA, int widthA, char operation, int heightB, int widthB) {
    this->heightA = heightA;
    this->widthA = widthA;
    this->operation = operation;
    this->heightB = heightB;
    this->widthB = widthB;
}

/******************************************************************************/

ostream& operator<<(ostream &out, const CSizeException &exc) {
    out << "Invalid matrix size ";
    out << exc.heightA;
    out << "x";
    out << exc.widthA;
    out << " ";
    out << exc.operation;
    out << " ";
    out << exc.heightB;
    out << "x";
    out << exc.widthB;
    return out;
}

ostream& operator<<(ostream &out, const CIndexException &exc) {
    out << "Invalid index [" << exc.index0 << "][" << exc.index1 << "]";
    return out;
}

/******************************************************************************/

CMatrix::CMatrix(int height, int width) {
    allocData(height, width);
}

CMatrix::CMatrix(const CMatrix& orig) {
    //deleteData();
    allocData(orig.rows, orig.cols);
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            //            data[j][i] = orig.data[j][i];
            (*this)[j][i] = orig[j][i];
        }
    }
}

CMatrix::~CMatrix() {
    deleteData();
}

CMatrix& CMatrix::operator=(const CMatrix &other) {
    if (&other == this) return *this;
    if (this->rows != other.rows ||
            this->cols != other.cols) {
        deleteData();
        allocData(other.rows, other.cols);
    }

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            (*this)[j][i] = other[j][i];
        }

    }

    return *this;
}

CMatrix CMatrix::operator-() const {
    CMatrix res(rows, cols);
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            res[j][i] = -((*this)[j][i]);
        }

    }

    return res;
}

COutLine CMatrix::operator[](const int index) {
    if (index < 0 || index > rows - 1) {
        return COutLine(data[index], this->cols, index, false);
    } else return COutLine(data[index], this->cols, index, true);
}

const COutLine CMatrix::operator[](const int index) const {
    if (index < 0 || index > rows - 1) {
        return COutLine(data[0], this->cols, index, false);
    } else return COutLine(data[index], this->cols, index, true);
}

CMatrix CMatrix::operator+(const CMatrix &b) const {

    if (rows == b.rows && cols == b.cols) {
        CMatrix res = *this;
        for (int j = 0; j < res.rows; j++) {
            for (int i = 0; i < res.cols; i++) {
                res[j][i] += b[j][i];
            }
        }
        return res;
    } else {
        throw CSizeException(rows, cols, '+', b.rows, b.cols);
    }
}

CMatrix CMatrix::operator-(const CMatrix &b) const {
    if (rows == b.rows && cols == b.cols) {
        CMatrix res = *this;
        for (int j = 0; j < res.rows; j++) {
            for (int i = 0; i < res.cols; i++) {
                res[j][i] -= b[j][i];
            }
        }
        return res;
    } else {
        throw CSizeException(rows, cols, '-', b.rows, b.cols);
    }
}

CMatrix CMatrix::operator*(const CMatrix &b) const{
    if (cols == b.rows) {
        CMatrix res(rows, b.cols);

        for (int j = 0; j < rows; j++) {
            for (int i = 0; i < b.cols; i++) {
                double val = 0;
                for (int k = 0; k < cols; k++) {
                    val += data[j][k] * b[k][i];
                }
                res[j][i] = val;
            }
        }

        return res;
    } else {
        throw CSizeException(rows, cols, '*', b.rows, b.cols);
    }
}

CMatrix CMatrix::operator*(const double x) const {
    CMatrix res = *this;
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            res[j][i] *= x;
        }
    }

    return res;
}

//CMatrix CMatrix::operator*(const double x, const CMatrix &a) {
//    CMatrix res(a.rows, a.cols);
//    for (int j = 0; j < a.rows; j++) {
//        for (int i = 0; i < a.cols; i++) {
//            res[j][i] *= x;
//        }
//    }
//
//    return res;
//}

ostream& operator<<(ostream &out, const CMatrix& a) {
    out << "{" << endl;
    for (int j = 0; j < a.rows; j++) {
        out << "  {";
        for (int i = 0; i < a.cols; i++) {
            out << a[j][i];
            if (i < a.cols - 1) out << ", ";
        }
        out << "}";
        if (j < a.rows - 1)out << ",";
        out << endl;
    }
    out << "}" << endl;
    return out;
}

double* reallocLine(double *data, int oldLen, int newLen) {
    //    cout << "reallocLine, old:" << oldLen << ", new:" << newLen << endl;
    double *res = new double[newLen];
    //    cout << "  new double[]" << endl;
    for (int i = 0; i < oldLen; i++) {
        //        cout << "  copying res[" << i << "] = data[" << i << "]. (" << data[i] << endl;
        res[i] = data[i];
    }

    //    cout << "  deleting data..."<<endl;
    delete [] data;
    //    cout << "  deleted."<<endl;

    return res;
}

double** reallocCol(double **data, int oldLen, int newLen, int colCnt) {
//    cout << "reallocCol, old:" << oldLen << ", new:" << newLen << endl;
    double **res = new double*[newLen];
    for (int i = 0; i < oldLen; i++) {
        res[i] = data[i];
    }

    for (int i = oldLen; i < newLen; i++) {
        res[i] = new double[colCnt];
    }

    delete [] data;

    return res;
}

void delDoubleArr(double ** data, int sizeY) {
    for (int i = 0; i < sizeY; i++) {
        delete [] data[i];
    }
    delete [] data;
}

istream& operator>>(istream &in, CMatrix& a) {
    string str;
    double num;
    char chr;
    bool ok = true;

    double **data;
    int sizeX = 5;
    int sizeY = 5;

    data = new double*[sizeY];
    for (int i = 0; i < sizeY; i++) {
        data[i] = new double[sizeX];
    }

    int cols = 0;
    int rows = 0;
    int thisCol = 0;


    in >> skipws;

    in >> chr;
    if (chr != '{') { // first opening {
        in.setstate(ios::failbit);
        delDoubleArr(data, sizeY);
        return in;
    }

    while (ok) {

        thisCol = 0;

        in >> chr;
        if (chr != '{') { // line's opening {
            in.setstate(ios::failbit);
            delDoubleArr(data, sizeY);
            return in;
        }

        while (ok) {

            in >> num;
            if (!in.good()) { // if reading a number failed, return fail
                delDoubleArr(data, sizeY);
                return in;
            }

            //            cout << "data[" << rows << "][" << thisCol << "] = " << num << endl;
            data[rows][thisCol++] = num;

            //            cout << "  thisCol: " << thisCol << ", sizeX: " << sizeX << endl;
            if (thisCol > sizeX - 1) {
                int oldSize = sizeX;
                sizeX += sizeX / 2;
                for (int i = 0; i < sizeY; i++) {
                    double *tmp = reallocLine(data[i], oldSize, sizeX);
                    data[i] = tmp;
                }
            }

            in >> chr; // read char; it as to be , or }

            if (chr != '}' && chr != ',') {
                in.setstate(ios::failbit);
                return in;
            } else if (chr == '}') {
                break;
            }
        }

        if (cols == 0) { // if first line
            cols = thisCol; // set matrix width
            rows++;
        } else if (thisCol != cols) {
            in.setstate(ios::failbit);
            ok = false;
            break;
        } else {
            rows++;
            //            cout << "__________ rows: " << rows << ", sizeY: " << sizeY << endl;
            if (rows >= sizeY - 1) {
                double **tmp = reallocCol(data, sizeY, sizeY + sizeY / 2, cols);
                sizeY += sizeY / 2;
                data = tmp;
            }
        }

        in >> chr;
        if (chr != '}' && chr != ',') {
            in.setstate(ios::failbit);
            delDoubleArr(data, sizeY);
            return in;
        } else if (chr == '}') {
            break;
        }
    }

    //    cout << "OK: " << ok << " rows: " << rows << ", cols: " << cols << endl;

    if (ok) {
        a.deleteData();
        a.allocData(rows, cols);

        cout << endl;

        for (int j = 0; j < rows; j++) {
            for (int i = 0; i < cols; i++) {
                //                cout << "a[" << j << "][" << i << "] = " << data[j][i] << endl;
                a[j][i] = data[j][i];
            }
        }
    } //else cout << "not ok." << endl;

    delDoubleArr(data, sizeY);

    return in;
}

void CMatrix::allocData(int height, int width) {
    this->rows = height;
    this->cols = width;

    //    cout << "new data(" << rows << "])" << endl;
    data = new double*[rows];
    for (int i = 0; i < rows; i++) {
        //        cout << "  new data[" << i << "] (" << cols << ")" << endl;
        data[i] = new double[cols];
        for (int j = 0; j < cols; j++)
            data[i][j] = 0;
    }
}

void CMatrix::deleteData() {
    for (int i = 0; i < rows; i++) {
        //        cout << "deleting data[" << i << "]" << endl;
        delete [] data[i];
    }
    //    cout << "deleting [] data" << endl;
    delete [] data;
}

/******************************************************************************/

COutLine::COutLine(double *data, int len, int firstIndex, bool firstOK) {
    this->len = len;
    line = data;
    this->firstIndex = firstIndex;
    this->firstOK = firstOK;
}

COutLine::~COutLine() {
}

double& COutLine::operator[](const int index) {
    if (index < 0 || index > len - 1 || !firstOK) {
        throw CIndexException(firstIndex, index);
//        cout << "wrong index." << endl;
    } else return line[index];
}

const double COutLine::operator[](const int index) const {
    if (index < 0 || index > len - 1 || !firstOK) {
        throw CIndexException(firstIndex, index);
//        cout << "wrong index." << endl;
    } else return line[index];
}

/******************************************************************************/