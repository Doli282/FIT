#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
using namespace std;

class TrainNetwork {
public:
#ifdef VARARGS
    template <class... Args>
    TrainNetwork& addLine(Args... args);
#endif

    TrainNetwork& addLine(std::initializer_list<std::string> stations){
        if(stations.size() < 2){
            return *this;
        }
        auto it = stations.begin();
        auto prevIt = it;
        ++it;
        for(; it != stations.end(); ++it){
            cout << *it << " - " << *prevIt << endl;
            graph[*it].emplace(*prevIt);
            graph[*prevIt].emplace(*it);
        }
        return *this;
    }

    std::list<std::string> path(const std::string& from, const std::string& to) const{
        queue<string> touched; // fronta s prohledavanymi stanicemi
        set<string> searched; // mnozina jiz prohledanych stanic
        list<string> StBetween; // vysledny list mezilehlych stanic
        map<string, string> arrivedFrom; // mapa stanic:   kam-odkud

        touched.push(from);
        searched.insert(from); // prohledavam vychozi stanici
        while(1){
            if(touched.size() == 0){ // fronta je prazdna, presto jsem nenasel
                throw runtime_error("nenasel");
            }
            string tmp = touched.front(); // prohledavana stanice
            for(auto station : graph.at(tmp)){ // pro vsechny sousedy prohledavane stanice
                if(station == to){ // dorazil jsem do cile
                    cout << "nasel" << endl;
                    arrivedFrom.emplace(make_pair(station,tmp));
                    goto konec;
                }
                if(searched.find(station) == searched.end()){ // stanice jeste neni v seznamu prohledavanych stanic
                    touched.push(station); // pridej do fronty na prohledani
                    searched.insert(station); // pridej mezi prohledane
                    arrivedFrom.emplace(make_pair(station,tmp));
                }
            }
        }
konec:
        string prevStation = arrivedFrom.at(to);
        cout << prevStation << endl;
        while(prevStation != from){
            cout << prevStation << " - " << endl;
            StBetween.push_front(prevStation);
            prevStation = arrivedFrom.at(prevStation);
        }
        return StBetween;
    }
    //std::map<std::string, unsigned> inMaxDistance(const std::string& from, unsigned maxDist) const;


    std::map<std::string, std::set<std::string>> graph;
};


int main()
{
    TrainNetwork prague;

    prague.addLine(/* A */ {"Nemocnice Motol", "Petřiny", "Nádraží Veleslavín", "Bořislavka", "Dejvická", "Hradčanská", "Malostranská", "Staroměstská", "Můstek", "Muzeum", "Náměstí Míru", "Jiřího z Poděbrad", "Flora", "Želivského", "Strašnická", "Skalka", "Depo Hostivař"});
    prague.addLine(/* B */ {"Černý Most", "Rajská Zahrada", "Hloubětín", "Kolbenova", "Vysočanská", "Českomoravská", "Palmovka", "Invalidovna", "Křižíkova", "Florenc", "Náměstí Republiky", "Můstek", "Národní třída", "Karlovo náměstí", "Anděl", "Smíchovské nádraží", "Radlická", "Jinonice", "Nové Butovice", "Hůrka", "Lužiny", "Luka", "Stodůlky", "Zličín"});
    prague.addLine(/* C */ {"Háje", "Opatov", "Chodov", "Roztyly", "Kačerov", "Budějovická", "Pankrác", "Pražského Povstání", "Vyšehrad", "I. P. Pavlova", "Muzeum", "Hlavní nádraží", "Florenc", "Vltavská", "Nádraží Holešovice", "Kobylisy", "Ládví", "Střížkov", "Prosek", "Letňany"});

    assert(prague.path("Nemocnice Motol", "Dejvická") == std::list<std::string>({"Nemocnice Motol", "Petřiny", "Nádraží Veleslavín", "Bořislavka", "Dejvická"}));
    assert(prague.path("Křižíkova", "Florenc") == std::list<std::string>({"Křižíkova", "Florenc"}));
    assert(prague.path("Křižíkova", "Muzeum") == std::list<std::string>({"Křižíkova", "Florenc", "Hlavní nádraží", "Muzeum"}));
    assert(prague.path("Můstek", "Muzeum") == std::list<std::string>({"Můstek", "Muzeum"}));
    assert(prague.path("Staroměstská", "Náměstí Republiky") == std::list<std::string>({"Staroměstská", "Můstek", "Náměstí Republiky"}));
    assert(prague.path("Náměstí Míru", "Pankrác") == std::list<std::string>({"Náměstí Míru", "Muzeum", "I. P. Pavlova", "Vyšehrad", "Pražského Povstání", "Pankrác"}));

    try {
        prague.path("Praha, Černý Most", "Liberec");
        assert(false);
    } catch (const std::runtime_error& e) {
    }

    try {
        prague.path("Černý Most", "Liberec");
        assert(false);
    } catch (const std::runtime_error& e) {
    }

    prague.addLine(/* ZZZ */ {"Praha, Černý Most", "Liberec"});
    try {
        prague.path("Hlavní nádraží", "Liberec");
        assert(false);
    } catch (const std::runtime_error& e) {
    }

#ifdef INMAXDISTANCE
    assert(prague.inMaxDistance("Můstek", 0) == (std::map<std::string, unsigned>{{"Můstek", 0}}));
    assert(prague.inMaxDistance("Můstek", 1) == (std::map<std::string, unsigned>{{"Můstek", 0}, {"Náměstí Republiky", 1}, {"Muzeum", 1}, {"Staroměstská", 1}, {"Národní třída", 1}}));
    assert(prague.inMaxDistance("Můstek", 3) == (std::map<std::string, unsigned>{{"Můstek", 0}, {"Náměstí Republiky", 1}, {"Muzeum", 1}, {"Staroměstská", 1}, {"Národní třída", 1}, {"Malostranská", 2}, {"Karlovo náměstí", 2}, {"Florenc", 2}, {"Hlavní nádraží", 2}, {"I. P. Pavlova", 2}, {"Náměstí Míru", 2}, {"Anděl", 3}, {"Hradčanská", 3}, {"Jiřího z Poděbrad", 3}, {"Vyšehrad", 3}, {"Křižíkova", 3}, {"Vltavská", 3}}));
#endif

    prague.addLine(/* D */ {"Náměstí Míru", "Náměstí Bratří Synků", "Pankrác", "Olbrachtova", "Nádraží Krč", "Nemocnice Krč", "Nové Dvory", "Libuš", "Písnice", "Depo Písnice"});

    assert(prague.path("Náměstí Míru", "Pankrác") == std::list<std::string>({"Náměstí Míru", "Náměstí Bratří Synků", "Pankrác"}));

    prague.addLine(/* O */ {"Nádraží Podbaba", "Dejvická", "Vozovna Střešovice", "Malovanka", "Strahov", "Anděl", "Smíchovské nádraží", "Dvorce", "Olbrachtova", "Budějovická", "Brumlovka", "Michle", "Eden / Slavia", "Želivského", "Nákladové nádraží Žižkov", "Chmelnice", "Nádraží Libeň", "Vysočanská", "Prosek", "Tupolevova", "Nové Čakovice", "Nádraží Čakovice", "Terminál sever (VRT)", "Ďáblice?", "Chabry?", "Bohnice?", "Zoologická zahrada", "Nádraží Podbaba"});

    assert(prague.path("Budějovická", "Olbrachtova") == std::list<std::string>({"Budějovická", "Olbrachtova"}));
    assert(prague.path("Anděl", "Dejvická") == std::list<std::string>({"Anděl", "Strahov", "Malovanka", "Vozovna Střešovice", "Dejvická"}));
    assert(prague.path("Zoologická zahrada", "Dejvická") == std::list<std::string>({"Zoologická zahrada", "Nádraží Podbaba", "Dejvická"}));


#ifdef VARARGS
    prague.addLine(/* varargs*/ "Station 1", "Station 2", "Station 3");
    assert(prague.path("Station 2", "Station 3") == std::list<std::string>({"Station 2", "Station 3"}));
#endif
}
