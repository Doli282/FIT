//============================================================================
// Name        : Server.cpp
// Author      : Viktor Cerny, Lukas Dolansky
// Version     :
// Copyright   : Your copyright notice
// Description : TCP server - demo
//============================================================================

#include <iostream>
using namespace std;

#include <cstdlib>
#include <cstdio>
#include <sys/socket.h> // socket(), bind(), connect(), listen()
#include <unistd.h> // close(), read(), write()
#include <netinet/in.h> // struct sockaddr_in
#include <strings.h> // bzero()
#include <wait.h> // waitpid()
#include <stdexcept>
#include <map>
#include <cstring>
#include <sstream>
#include <string_view>

#define BUFFER_SIZE 10240
#define TIMEOUT 1
#define TIMEOUT_RECHARGING 5
#define MODUL 65536

#define SERVER_KEY_REQUEST "107 KEY REQUEST\a\b"
#define SERVER_SYNTAX_ERROR "301 SYNTAX ERROR\a\b"
#define SERVER_LOGIC_ERROR "302 LOGIC ERROR\a\b"
#define SERVER_KEY_OUT_OF_RANGE_ERROR "303 KEY OUT OF RANGE\a\b"
#define SERVER_LOGIN_FAILED "300 LOGIN FAILED\a\b"
#define SERVER_OK "200 OK\a\b"
#define SERVER_MOVE "102 MOVE\a\b"
#define SERVER_TURN_LEFT "103 TURN LEFT\a\b"
#define SERVER_TURN_RIGHT "104 TURN RIGHT\a\b"
#define SERVER_PICK_UP "105 GET MESSAGE\a\b"
#define SERVER_LOGOUT "106 LOGOUT\a\b"
#define CLIENT_FULL_POWER "FULL POWER"
#define CLIENT_RECHARGE "RECHARGING"
#define DELIMITER "\a\b"

//[x,y]
struct CPosition{
    CPosition(int x, int y) : m_X(x), m_Y(y){}
    CPosition(const CPosition & other){m_X = other.m_X; m_Y = other.m_Y;}
    int m_X;
    int m_Y;
    //CPosition operator +(const CPosition & other){return CPosition(m_X+other.m_X, m_Y+other.m_Y);}
    //CPosition operator -(const CPosition & other){return CPosition(m_X-other.m_X, m_Y-other.m_Y);}
    CPosition & operator = (const CPosition & other){m_X = other.m_X; m_Y = other.m_Y; return *this;}
    bool operator ==(const CPosition & other){return (m_X == other.m_X && m_Y == other.m_Y);}
};

// 0 = RIGHT; 1 = DOWN; 2 = LEFT; 3 = UP
struct CDirection{
    CDirection(int dir) : m_direction(dir) {}
    CDirection(const CDirection & other){m_direction = other.m_direction;}
    CDirection(const CPosition & previousPosition, const CPosition & currentPosition) {calculateDirection(previousPosition, currentPosition);}
    int m_direction; // 0 = RIGHT; 1 = DOWN; 2 = LEFT; 3 = UP
    CDirection operator++(int){CDirection a(m_direction); m_direction = (m_direction + 1) % 4; return a;}
    CDirection operator--(int){CDirection a(m_direction); m_direction = (m_direction + 3) % 4; return a;}
    void calculateDirection(const CPosition & previousPosition, const CPosition & currentPosition){
        if(previousPosition.m_X < currentPosition.m_X){m_direction = 0; return;} // RIGHT
        if(previousPosition.m_X > currentPosition.m_X){m_direction = 2; return;} // LEFT
        if(previousPosition.m_Y < currentPosition.m_Y){m_direction = 3; return;} // UP
        if(previousPosition.m_Y > currentPosition.m_Y){m_direction = 1; return;} // DOWN
    }
};

struct KEYS{
    int serverKey;
    int clientKey;
};
map<int,KEYS> KEY_IDs = { {0, {23019, 32037}} , {1, {32037, 29295}} , {2, {18789, 13603}} , {3, {16443, 29533}} , {4, {18189, 21952}} };

// posle zpravu klientovi
int sendMsg(int & c, string message){
    char * buffer =  &message[0];
    // Priznak MSG_NOSIGNAL znamena, ze v pripade predcasneho ukonceni
    // na druhe strane nedojde k preruseni aktulaniho procesu signalem
    // SIGPIPE, ale funkce send() skonci s nenulovou navratovou hodnotou
    // a my muzeme korektne ukoncit nas program.
    if(send(c, buffer, strlen(buffer), MSG_NOSIGNAL) < 0){
        perror("Nemohu odeslat data:");
        close(c);
        return -3;
    }
    return 0;
}

// zaznamenan SYNTAX ERROR
void syntaxErr(int & c){
    cout << "SYNTAX ERROR" << endl;
    sendMsg(c, SERVER_SYNTAX_ERROR);
    close(c);
    return;
}

// zaznamenan LOGIC ERROR
void logicErr(int & c){
    cout << "LOGIC ERROR" << endl;
    sendMsg(c, SERVER_LOGIC_ERROR);
    close(c);
    return;
}

void shiftchars(char * zaznamnik, size_t & zaznamnikLen, size_t shift){
    size_t prevIndex = shift;
    for(size_t index = 0; prevIndex < zaznamnikLen;){
        //cout << "vymena" << zaznamnik[index] << " za ";
        zaznamnik[index++] = zaznamnik[prevIndex++];
        //cout << zaznamnik[index] << endl;
    }
    zaznamnikLen -= shift;
    return;
}

// vyzvedne zpravu ze socketu
int pickUpMsgInner(fd_set & sockets, int & retval, timeval & timeout, int & c, char* buffer, int & bytesRead, char* zaznamnik, size_t & zaznamnikLen, char * zpracovani, size_t & zpracovaniLen, unsigned maxDelka, string & strzpracovani, unsigned setTimeOut){
    while(1){ // dokud nenactu celou zpravu cti ze socketu
        char * n = (char*)memmem(zaznamnik, zaznamnikLen, DELIMITER, 2); // najdi \a\b
        //cout << " in while" << endl;
        // zkontroluj jestli je zprava zaznamenana
        if(n != nullptr){ // cela zprava je zaznamenana <- nasel \a\b
            zpracovaniLen = 0;
            //zaznamnikLen -= 2;
            char * ptr = zaznamnik;
            for(; n != ptr; ){
                zpracovani[zpracovaniLen] = *ptr;
                //cout << *ptr << ",";
                zpracovaniLen++; ptr++; //zaznamnikLen--;
            }
            //cout << endl;
            shiftchars(zaznamnik, zaznamnikLen, zpracovaniLen+2);
            cout << "predano ke zpracovani "  << zpracovaniLen << " z " << zaznamnikLen << endl;
            //zpracovani = zaznamnik.substr(0, n); // vyjmi zpravu ze zaznamniku
            //zaznamnik.erase(0, n + 2);

            zpracovani[zpracovaniLen] = '\0';
            strzpracovani = zpracovani;

            if(zpracovaniLen > (maxDelka - 2)){  // prekracuji delku ocekavane zpravy
                if(strzpracovani == CLIENT_FULL_POWER || strzpracovani == CLIENT_RECHARGE) break;
                cout << "moc dlouha zprava uvnitr - znaku: " << maxDelka << "/" << zaznamnikLen  << endl;
                cout << ">" << strzpracovani << "<" << endl;
                syntaxErr(c);
                return 1;
            }

            break;
        } else {
            
            if(zaznamnikLen > (maxDelka - 2)){  // prekracuji delku ocekavane zpravy
                cout << "podezrela delka> zaznamenana delka: " << zaznamnikLen << " max delka: " << maxDelka << endl; 
                if((zaznamnikLen != (maxDelka - 1)) || (zaznamnik[zaznamnikLen-1] != '\a')){
                    cout << "moc dlouha zprava vne- znaku: " << maxDelka << "/" << zaznamnikLen  << endl;
                    for(size_t i = 0; i < zaznamnikLen; i++){
                        cout <<  zaznamnik[i] << " (" << (int) zaznamnik[i] << "), ";
                    }
                    cout << endl;
                    syntaxErr(c);
                    return 1;
                }
            }

            FD_ZERO(&sockets); // adresu fdset vynuluje
            FD_SET(c, &sockets); // nastavi hodnotu c do promene
            
            timeout.tv_sec = setTimeOut; // select bude cekat 'timeout' vterin pri kazdem cteni
            timeout.tv_usec = 0;
            // Prvnim parametrem je cislo nejvyssiho soketu v 'sockets' zvysene o jedna.
            // (Velka jednoduchost a efektivvnost funkce je vyvazena spatnou
            // ergonomii pro uzivatele.)
            // Posledni z parametru je samotny timeout. Je to struktura typu 'struct timeval',
            // ktera umoznuje casovani s presnosti na mikrosekundy (+/-). Funkci se predava
            // odkaz na promennou a funkce ji muze modifikovat. Ve vetsine implementaci
            // odecita funkce od hodnoty timeoutu cas straveny cekanim. Podle manualu na
            // to nelze spolehat na vsech platformach a ve vsech implementacich funkce
            // select()!!!
            //druhy param = pole socketu pro cteni, treti param = pole pro zapis
            retval = select(c + 1, &sockets, NULL, NULL, &timeout); // funkce select -> blokujici funkce, kontroluje sokety, zda doslo k udalosti
            if(retval < 0){
                perror("Chyba v select(): ");
                close(c);
                return 2;
            }
            if(!FD_ISSET(c, &sockets)){ // select se odblokuje po vyprseni timeoutu nebo po udalosti -> je-li nataven socket -> doslo k udalosti, jinak muselo dojit k timeoutu
                // Zde je jasne, ze funkce select() skoncila cekani kvuli timeoutu.
                cout << "Connection timeout!" << endl;
                close(c);
                return 1;
            }
            // funkce selecta poznala, ze nastala udalost => prisla data => funkce recieve bude mit co cist
            bytesRead = recv(c, buffer, BUFFER_SIZE - 1, 0); // recieve je blokujici funkce -> snazi se precist data z bufferu -> pokud tam nejsou, ceka na ne
            if(bytesRead <= 0){
                perror("Chyba pri cteni ze socketu: ");
                close(c);
                return 2;
            }
            //cout << "cp: ";
            for(int i = 0; i < bytesRead; i++){ // prekopirovani bufferu do zaznamniku
                zaznamnik[zaznamnikLen++] = buffer[i];
                //cout << buffer[i] << ",";
            }
            //cout << endl; cout << "prekopirovano " << zaznamnikLen << endl;
            //buffer[bytesRead] = '\0'; // pridam buffer do zaznamniku
            //zaznamnik += buffer;
            //cout << "zaznamnik: " << zaznamnik << ">"<< endl;
            
            // predchozi pozice kontroly delky
        }
    }
    //zpracovani[zpracovaniLen] = '\0';
    //strzpracovani = zpracovani;
    cout << ">" << strzpracovani << "<" << endl;
    return 0;
}


// zajisti vyzvednuti zpravy ze socketu + obstara nabijeni
int pickUpMsg(fd_set & sockets, int & retval, timeval & timeout, int & c, char* buffer, int & bytesRead, char* zaznamnik, size_t & zaznamnikLen, char * zpracovani, size_t & zpracovaniLen, unsigned maxDelka, string & strzpracovani){
    int retvalue;
    if((retvalue = pickUpMsgInner(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, maxDelka, strzpracovani, TIMEOUT)) != 0) return retvalue; // nacti zpravu
    if(strzpracovani == CLIENT_FULL_POWER) {logicErr(c); return 10;} // prislo FULL POWER
    if(strzpracovani == CLIENT_RECHARGE){ // prislo RECHARGE
        if((retvalue = pickUpMsgInner(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 12, strzpracovani, TIMEOUT_RECHARGING)) != 0) return retvalue; // nacti nasledujici zpravu
        if(strzpracovani != CLIENT_FULL_POWER) {logicErr(c); return 10;} // neprislo ocekavane FULL POWER
        else return pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, maxDelka, strzpracovani); // klient je nabity -> opakuj proces prijmani zpravy
    }
    // prislo neco jineho ...
    return 0;
}


unsigned hashName(char * zpracovani, size_t zpracovaniLen){
    unsigned hash = 0;
    for(size_t i = 0; i < zpracovaniLen; i++){
        hash += zpracovani[i];
    }
    hash *= 1000;
    hash %= MODUL;
    return hash;
}

// nahrani pozice robota ze zpravy CLIENT OK
int loadCoordinates(string & zpracovani, CPosition & pos, int & c){
    // nahrani zpravy
    if(zpracovani.find('.') != string::npos){syntaxErr(c); return 2;} // obsahuje desetinne cislo
    if(isgraph(zpracovani.at(zpracovani.size() - 1)) == 0){syntaxErr(c); return 2;} //konci mezerou nebo netisknutelnym znakem
    istringstream iss(zpracovani);
    string dummy;
    iss >> dummy;
    if(dummy != "OK"){syntaxErr(c); return 2;} // zacina "OK"
    iss >> pos.m_X;
    iss >> pos.m_Y;

    return 0;
}

// rozhodne co ma robot delat dale -> move / turn right / turn left
int getCommand(const CPosition & currPosition, const CDirection & direction){
    if( abs(currPosition.m_X) >= abs (currPosition.m_Y)){
        if(currPosition.m_X > 0){ // napravo od cile
            if(direction.m_direction == 2) return 0; // smeruje doleva -> move
            if(direction.m_direction == 3) return 2; // smeruje nahore -> turn left
            return 1; // smeruje napravo / dolu -> turn right
        } else {// nalevo od cile
            if(direction.m_direction == 0) return 0; // smeruje doprava -> move
            if(direction.m_direction == 1) return 2; // smeruje dolu -> turn left
            return 1; // smeruje nalevo / nahoru -> turn right
        }
    } else {
        if(currPosition.m_Y > 0){ // nahore od cile
            if(direction.m_direction == 1) return 0; // smeruje dolu -> move
            if(direction.m_direction == 2) return 2; // smeruje nalevo -> turn left
            return 1; // smeruje napravo / nahoru -> turn right
        } else {// dole od cile
            if(direction.m_direction == 3) return 0; // smeruje nahoru -> move
            if(direction.m_direction == 0) return 2; // smeruje napravo -> turn left
            return 1; // smeruje nalevo / dolu -> turn right
        }
    }
}

//  2|1
// --+-- 
//  3|4
int determineQuadrant(const CPosition & currPosition){
    if(currPosition.m_X > 0){
        if(currPosition.m_Y > 0){
            return 1;
        } else {
            return 4;
        }
    } else {
        if(currPosition.m_Y > 0){
            return 2;
        } else {
            return 3;
        }
    }
}

// po narazu do prekazky rozhodne o otoceni right/left
int changeCommand(int quadrant, const CDirection & direction){
    switch (quadrant){
        case 1:
            if(direction.m_direction > 1){ // smer doleva/nahoru
                return 2; // turn left
            } else { // smer dolu/doprava
                return 1; // turn right
            }
        case 2:
            if(direction.m_direction == 1 || direction.m_direction == 2 ){ // smer dolu/doleva
                return 2; // turn left
            } else { // smer doprava/nahoru
                return 1; // turn right
            }
        case 3:
            if(direction.m_direction > 1){ // smer doleva/nahoru
                return 1; // turn right
            } else { // smer dolu/doprava
                return 2; // turn left
            }
        case 4:
            if(direction.m_direction == 1 || direction.m_direction == 2 ){ // smer dolu/doleva
                return 1; // turn right
            } else { // smer doprava/nahoru
                return 2; // turn left
            }
    }
    return 1; // defaultne se otoc vpravo <- nemelo by nastat
}

// posli robotovi zpravu co ma udelat
void nextMove(int & c ,int & command, CPosition & currentPosition, CPosition & prevPosition, CDirection & direction, bool & changingCommands){
    if((command == 0) && (currentPosition == prevPosition)){ // narazil na prekazku
        changingCommands = true;
        command = changeCommand(determineQuadrant(currentPosition),direction); // je treba zmenit povel
    } else {  // cesta je volna
        if(changingCommands == true){ // objizdime prekazku
            command = 0;
            changingCommands = false;
        } else {
            command = getCommand(currentPosition, direction); // pokracuj v planu novym povelem
        }
    }
    if(command == 0) {cout << "MOVE" <<endl; sendMsg(c, SERVER_MOVE);}
    else if (command == 1) {cout << "TURN RIGHT" <<endl; sendMsg(c, SERVER_TURN_RIGHT); direction++;}
    else {cout << "TURN LEFT" <<endl; sendMsg(c, SERVER_TURN_LEFT); direction--;}
    cout << " --> parametry: " << "; cmd: " << command << "; dir: " << direction.m_direction << "; change? " << changingCommands << endl; 
    return;
}

int main(int argc, char **argv) {
    cout << " server spusten" << endl;
    // v prvnim argumentu udam cislo portu, kde bude server fungovat -> treba 4000
    if(argc < 2){
        cerr << "Usage: server port" << endl;
        return -1;
    }

    // Vytvoreni koncoveho bodu spojeni
    int l = socket(AF_INET, SOCK_STREAM, 0); // protokol sitove vrstvy (IPv4), protokol transportni vrstvy (TCP), priznkay soketu, navratova hodnota -> 0 spojeni vytvoreno, jinak chyba
    if(l < 0){
        perror("Nemohu vytvorit socket: "); // vypisuje chyby
        return -1;
    }

    int port = atoi(argv[1]); // zjisteni portu z argumentu
    if(port == 0){ // spatny vstup -> port cislo 0 stejne nesmim pouzit
        cerr << "Usage: server port" << endl;
        close(l); // uzavirani portu
        return -1;
    }

    struct sockaddr_in adresa; // prirazeni socketu k sitovemu rozhrani
    bzero(&adresa, sizeof(adresa)); // vynulovani pameti 'adresa'
    adresa.sin_family = AF_INET; // nastaveni rodiny adres
    adresa.sin_port = htons(port); // nastaveni cisla portu, kde server nasloucha <- htons spravne preklada cislo dle ednianity => host to network short (2 bajty)
    adresa.sin_addr.s_addr = htonl(INADDR_ANY); // nastaveni adresy -> INADDR_ANY = 0 -> aby server naslouchal na vsech adresach, htonl = host to network long (4 bajty)

    // Prirazeni socketu k rozhranim
    if(bind(l, (struct sockaddr *) &adresa, sizeof(adresa)) < 0){
        perror("Problem s bind(): ");
        close(l);
        return -1;
    }

    // Oznacim socket jako pasivni
    if(listen(l, 10) < 0){ // az do deseti spojeni
        perror("Problem s listen()!");
        close(l);
        return -1;
    }

    struct sockaddr_in vzdalena_adresa; // vyplni se pri pripojeni klienta
    socklen_t velikost;

    while(true){ // nekonecny cyklus -> v kazde iteraci muze pripojit noveho klienta
        // Cekam na prichozi spojeni
        int c = accept(l, (struct sockaddr *) &vzdalena_adresa, &velikost); // pri pripojeni vyplni hodnoty od klienta, funkce accept je implicitne blokujici
        if(c < 0){ // c -> filedscriptor -> unikatni pro daneho klienta
            perror("Problem s accept()!");
            close(l);
            return -1;
        }
        pid_t pid = fork(); // nove vlakno => kopie aktualniho procesu -> lisi se jen v PID, navratovou hodnotou je PID potomka
        if(pid == 0){ // rozlisuje vlakna -> aby zacla delat neco jineho -> 0 = potomek
            // Kopie hlavniho vlakna ma vlastni referenci na naslouchajici soket.
            // Podproces, ktery obsluhuje klienta, tuto referenci nepotrebuje, takze je dobre
            // tuto referenci smazat. V hlavnim vlakne samozrejme reference na naslouchajici
            // soket zustava.
            close(l);

            struct timeval timeout; // struktura casu -> uklada ho v sekundach
            timeout.tv_sec = TIMEOUT;
            timeout.tv_usec = 0;

            fd_set sockets; //

            int retval;
            char buffer[BUFFER_SIZE]; // sem prijmam data
            char zaznamnik[BUFFER_SIZE];
            size_t zaznamnikLen = 0;
            size_t zpracovaniLen = 0;
            char zpracovani[BUFFER_SIZE];
            string strzpracovani;

            int bytesRead;
            int returnValue;
            bool changingCommand = false;

            /* **** ZACATEK KOMUNIKACE **** */
            // CLIENT USERNAME ---> // max delka USERNAME = 20
            cout << "CLIENT USERNAME" << endl;
            if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 20, strzpracovani)) != 0) return returnValue;
            unsigned hash = hashName(zpracovani, zpracovaniLen);
            // <------ SERVER KEY REQUEST
            cout << "SERVER KEY REEQUEST" << endl;
            sendMsg(c, SERVER_KEY_REQUEST);
            // -------> CLIENT KEY ID // max delka KEY ID = 5
            cout << "CLIENT KEY ID" << endl;
            if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 5, strzpracovani)) != 0) return returnValue;
            int key_ID;
            try{
                key_ID = stoi(strzpracovani);
            }catch(invalid_argument const & ex){ // neni to cislo
                syntaxErr(c); return 1;
            }
            if(key_ID < 0 || key_ID > 4){
                sendMsg(c, SERVER_KEY_OUT_OF_RANGE_ERROR); close(c); return 2;
            }
            unsigned serverCode = (hash + KEY_IDs.at(key_ID).serverKey) % MODUL;
            // < ----- SERVER CONFIRMATION
            cout << "SERVER CONFIRMATION" << endl;
            stringstream ss; ss << serverCode; ss << "\a\b";
            sendMsg(c, ss.str());
            // ----> CLIENT CONFIRMATION // max delka CONFIRMATION = 7
            cout << "CLIENT CONFIRMATION" << endl;
            if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 7, strzpracovani)) != 0) return returnValue;
            unsigned clientCode;
            try{
                clientCode = stoi(strzpracovani);
                if(strzpracovani.find('.') != string::npos) throw invalid_argument(""); // obsahuje desetinne cislo
                if(isgraph(strzpracovani.at(strzpracovani.size() - 1)) == 0)throw invalid_argument(""); //konci mezerou nebo netisknutelnym znakem
            }catch(invalid_argument const & ex){ // neni to cislo
                syntaxErr(c); return 1;
            }
            // <----- SERVER OK nebo SERVER LOGIN FAILED
            
            unsigned calculatedClientCode = (hash + KEY_IDs.at(key_ID).clientKey) % MODUL;
            if(clientCode != calculatedClientCode){
                cout << "SERVER LOGIN FAILED" << endl;
                sendMsg(c, SERVER_LOGIN_FAILED); close(c); return 3;
            }
            cout << "SERVER OK" << endl;
            sendMsg(c, SERVER_OK);
             /* !!!! spojeni navazano !!!! */

            cout << "spojeni navazano" << endl;
            CPosition goalPosition(0,0);
            CPosition currentPosition(0,0);
            CPosition prevPosition(0,0);
            int command = 0; // 0 = MOVE,   1 = TURN RIGHT,       2 = TURN LEFT
            
            
            // iniciailizacni zjistovani pozice
            do{
            cout << "TURN RIGHT" << endl;
            sendMsg(c, SERVER_TURN_RIGHT);
            if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 12, strzpracovani)) != 0) return returnValue;
            // nahrani zpravy
            if(loadCoordinates(strzpracovani, currentPosition, c) != 0) return 2;// nahrani pozice robota ze zpravy CLIENT OK
            prevPosition = currentPosition;
            
            cout << "MOVE" << endl;
            sendMsg(c, SERVER_MOVE);
            if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 12, strzpracovani)) != 0) return returnValue;
            // nahrani zpravy
            if(loadCoordinates(strzpracovani, currentPosition, c) != 0) return 2; // nahrani pozice robota ze zpravy CLIENT OK
            } while (prevPosition == currentPosition); // narazil na prekazku

            CDirection direction(prevPosition, currentPosition); // urci jakym smerem je robot natocen


            while(true){ // v cyklu prijma data od klienta
                if(currentPosition == goalPosition) break; // dorazil do cile
                // rozhodni co dal
                nextMove(c, command, currentPosition, prevPosition, direction, changingCommand);

                // prijmi odpoved
                if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead,zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 12, strzpracovani)) != 0) return returnValue;
                prevPosition = currentPosition;
                if(loadCoordinates(strzpracovani, currentPosition, c) != 0) return 2; // nahrani pozice robota ze zpravy CLIENT OK
                




//                buffer[bytesRead] = '\0';
//                // Pokud prijmu "konec" ukoncim aktualni dceriny proces
//                if(string("konec") == buffer){
//                    break;
//                }
//                cout << buffer << endl;
            }

            // <---- SERVER PICK UP
            cout << "SERVER PICK UP" <<endl;
            sendMsg(c,SERVER_PICK_UP);
            // -----> CLIENT MESSAGE // max delka MESSAGE = 100
            cout << "CLIENT MESSAGE" <<endl;
            if( (returnValue = pickUpMsg(sockets, retval, timeout, c, buffer, bytesRead, zaznamnik, zaznamnikLen, zpracovani, zpracovaniLen, 100, strzpracovani)) != 0) return returnValue;
            cout  << "Final Message: delka: " << strzpracovani.size() << " > "  << strzpracovani << endl;
            // <----- SERVER LOGOUT
            cout << "SERVER LOG OUT" <<endl;
            sendMsg(c, SERVER_LOGOUT);

            /* ** MISE SPLNENA ** */
            close(c);
            return 0;
        }

        // Aby nam nezustavaly v systemu zombie procesy po kazdem obslouzeneme klientovi,
        // je nutne otestovat, zda se uz nejaky podproces ukoncil.
        // Prvni argument rika, cekej na jakykoliv proces potomka, treti argument zajisti,
        // ze je funkce neblokujici (standardne blokujici je, coz ted opravdu nechceme).
        int status = 0;
        waitpid(0, &status, WNOHANG);

        close(c); // Nove vytvoreny socket je nutne zavrit v hlavnim procesu, protoze by na nem v systemu
        // zustala reference a jeho zavreni v novem procesu by nemelo zadny efekt.
    }

    close(l);
    return 0;
}
