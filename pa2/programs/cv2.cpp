#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class Date{
    public:
        Date(int day, int month, int year){
            m_Day = day;
            m_Month = month;
            m_Year = year;
        }
        Date(string date){
            m_Day = stoi(date.substr(0,2));
            m_Month = stoi(date.substr(3, 2));
            m_Year = stoi(date.substr(6, 4));
        }
        void print(std::ostream &os){
            os << setw(2) << setfill('0') << m_Day << "." << setw(2) << setfill('0') << m_Month << "." << setw(4) << setfill('0') << m_Year << endl;
        }

        int compare(Date other){
            if(m_Year > other.m_Year){ return 1;}
            else if(m_Year < other.m_Year){return -1;}

            if(m_Month > other.m_Month){ return 1;}
            else if(m_Month < other.m_Month){return -1;}

            if(m_Day > other.m_Day){ return 1;}
            else if(m_Day < other.m_Day){return -1;}
            return 0;
        }
        bool less_than(Date other){
            if(m_Year > other.m_Year){ return false;}
            else if(m_Year < other.m_Year){return true;}

            if(m_Month > other.m_Month){ return false;}
            else if(m_Month < other.m_Month){return true;}

            if(m_Day < other.m_Day){return true;}
            return false;
        }
    private:
        int m_Day;
        int m_Month;
        int m_Year;
};

enum class Gender{
    MALE,
    FEMALE,
    UNKNOWN,
};

string toString(Gender gender){
    switch(gender){
        case Gender::MALE: return "male";
        case Gender::FEMALE: return "female";
        default: return "other";
    }
}

class Person{
    public:
        Person(const string & name, const Date & bday, Gender gender) : m_Name(name), m_Bday(bday), m_Gender(gender){
            
        }

        void print(std::ostream &os){
            os << m_Name << endl;
            m_Bday.print(os);
            os << toString(m_Gender) << endl;
        }
    private:
        string m_Name;
        Date m_Bday;
        Gender m_Gender;
    
};

int main(){
    

    Date d1(1,1,2000);
    Person p1("jmeno prijmeni", d1, Gender::UNKNOWN);
    p1.print(std::cout);
/*
    Date d2(1,5,2000);
    Date d3(26,1,1982);
    Date d4(1,1,2000);

    d1.print(cout);
    cout << d1.compare(d2) << endl;
    cout << d1.compare(d3) << endl;
    cout << d1.compare(d4) << endl;
    cout << boolalpha << d1.less_than(d2) << endl;
    cout << boolalpha << d1.less_than(d3) << endl;
    cout << boolalpha << d1.less_than(d4) << endl;

    Date b1("01-01-2000");
    b1.print(cout);
    */
}