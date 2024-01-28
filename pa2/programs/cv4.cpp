#include <iostream>
#include <tuple>

class CComplex{
    public:
    CComplex(int real, int imaginary = 0) : m_Real(real), m_Imaginary(imaginary){}
    friend CComplex operator - (const CComplex & num1, const CComplex & num2);
    friend CComplex operator + (const CComplex & num1, const CComplex & num2);
    friend std::ostream & operator << (std::ostream & os, CComplex num);

    /* CComplex operator + (const CComplex & other) const {
        CComplex out(m_Real + other.m_Real, m_Imaginary + other.m_Imaginary);
        return out;
    }*/
    CComplex operator * (const CComplex & other) const {
        CComplex out(m_Real*other.m_Real - m_Imaginary*other.m_Imaginary,
                    m_Real*other.m_Imaginary + m_Imaginary*other.m_Real);
        return out;
    }
    CComplex& operator += (const CComplex & other){
        m_Real += other.m_Real;
        m_Imaginary += other.m_Imaginary;
        return *this;
    }
    CComplex& operator -= (const CComplex & other){
        m_Real -= other.m_Real;
        m_Imaginary -= other.m_Imaginary;
        return *this;
    }
    bool operator < (const CComplex & other){
        return std::tie(m_Real, m_Imaginary) < std::tie(other.m_Real, other.m_Imaginary);
        /*if(m_Real < other.m_Real) return true;
        else if(m_Real > other.m_Real) return false;
        if(m_Imaginary < other.m_Imaginary) return true;
        else return false;*/
    }
    bool operator <= (const CComplex & other){
        if(m_Real > other.m_Real) return false;
        if(m_Imaginary > other.m_Imaginary) return false;
        return true;
    }
    bool operator > (const CComplex & other){
        if(m_Real > other.m_Real) return true;
        else if(m_Real < other.m_Real) return false;
        if(m_Imaginary > other.m_Imaginary) return true;
        else return false;
    }
    bool operator >= (const CComplex & other){
        if(m_Real < other.m_Real) return false;
        if(m_Imaginary < other.m_Imaginary) return false;
        return true;
    }
    bool operator == (const CComplex & other){
        if((m_Real == other.m_Real) && (m_Imaginary == other.m_Imaginary))return true;
        else return false;
    }
    CComplex operator - (){
        return CComplex(0 - m_Real, 0 - m_Imaginary);
    }

    CComplex& operator ++ (){
        m_Real++;
        m_Imaginary++;
        return *this;
    }
    CComplex& operator -- (){
        m_Real--;
        m_Imaginary--;
        return *this;
    }
    CComplex operator ++ (int){
        CComplex tmp = *this;
        m_Real++;
        m_Imaginary++;
        return tmp;
    }
    CComplex operator -- (int){
        CComplex tmp = *this;
        --m_Real;
        --m_Imaginary;
        return tmp;
    }

    /*explicit operator std::to_string(){

    }*/

    private:
    int m_Real;
    int m_Imaginary;
};

CComplex operator - (const CComplex & num1, const CComplex & num2) {
        CComplex out(num1.m_Real - num2.m_Real, num1.m_Imaginary - num2.m_Imaginary);
        return out;
    }
CComplex operator + (const CComplex & num1, const CComplex & num2) {
        CComplex out(num1.m_Real + num2.m_Real, num1.m_Imaginary + num2.m_Imaginary);
        return out;
    }
std::ostream & operator << (std::ostream & os, CComplex num){
    os << num.m_Real << " + (" << num.m_Imaginary << ")i"; 
    return os;
}

int main(){
    CComplex c1(1, 2);
    CComplex c2(3, 4);
    std::cout << "soucet: " << c1 + c2 << std::endl;
    std::cout << "rozdil: " << c1 - c2 << std::endl;
    std::cout << "soucin: " << c1 * c2 << std::endl;
    std::cout << "rozdil=: " << (c1 -= c2) << std::endl;
    std::cout << "minus: " <<  (-c2) << std::endl;
    std::cout << "soucet s cislem: " << c1 + 5 << std::endl;
    std::cout << "rozdil s cilsem: " << 5 - c2 << std::endl;
    std::cout << "porovnani: " << std::boolalpha << (c1 == c2) << std::endl;
    std::cout << "mensi: " << std::boolalpha << (c1 < c2) << std::endl;
    std::cout << "postinkrement: " << c1++ << std::endl;
    std::cout << "preinkrement: " << ++c1 << std::endl;
    return 0;
}