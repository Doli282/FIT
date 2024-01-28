//#include <string>
#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

class PrefixCounter{
  private:
    vector<string> dict;

  public:
    bool AddWord(string word){
        dict.push_back(word);
        return true;
    }
    int CountPrefix(string prefix) const{
        int count = 0;
        for(auto word : dict){
            if(compare(word, prefix, prefix.size())) count ++;
        }
        return count;
    }
    bool compare(const char * w1, const char * w2, int len){
        //if(strncmp(w1, w2, len) == 0) true;
        return false;
    }
};

int main(void){
    PrefixCounter db;
    db.AddWord("asdf");
    db.AddWord("asdfv");
    db.AddWord("vwqefv");
    db.AddWord("fdsa");
    db.AddWord("weasdfv");
    db.AddWord("asdf");
    int cnt = db.CountPrefix("asdf");
    cout << cnt << endl;
    return 0;
}