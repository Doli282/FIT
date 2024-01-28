#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(){

    string name;
    int number;
    cout << "zadej nazev souboru a cislo" << endl;
    cin >> name;
    cin >> number;

    ofstream outFile(name);
    outFile << number << '\n';
    return 0;
}