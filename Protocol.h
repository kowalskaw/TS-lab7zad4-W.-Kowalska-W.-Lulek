#ifndef ZADANIE_HAPPY_PROTOCOL_H
#define ZADANIE_HAPPY_PROTOCOL_H


#include <bitset>
#include <array>
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <regex>

using namespace std;

class BinProt {
    bitset<2> op;		//operacje: 00 - mnozenie, 01 - dodawanie, 11 - dzielenie, 10 - inkremetacja
    bitset<64> L1;		//pole liczba 1
    bitset<64> L2;		//pole liczba 2
    bitset<64> L3;		//pole liczba 3
    bitset<4> status;	//0001 - pierwszy datagram(BEGIN)<-wtedy też może być ustanowiony identf,
                        //0010 - ostatni datagram(FIN), 0100 - potwierdzenie(ACK)
    bitset<4> identf;	//identyfikator sesji dla serwera, rozróżnienie klientów

public:
    BinProt() {}

    //ustala operację
    void setOperation(int n) {
        switch (n) {
            case 1: op.reset(); break;		//mnożenie - 00
            case 2: op.set(0); break;		//dodawanie - 01
            case 3: op.set(); break;		//dzielenie - 11
            case 4: op.set(1); break;		//odejmowanie - 10
            default: break;
        }
    }
    //ustawia L1
    void setL1(int n){
        bitset<64> l1(n);
        L1 = l1;
    }
    //ustawia L2
    void setL2(int n){
        bitset<64> l2(n);
        L2 = l2;
    }
    //ustawia L3
    void setL3(int n){
        bitset<64> l3(n);
        L3 = l3;
    }
    //ustatala status
    void setStatus(int n) {
        switch (n) {
            case 1: status.set(0); break;	//BEGIN - 0001
            case 2: status.set(1); break;	//FIN - 0010
            case 3: status.set(2); break;	//ACK - 0100

            case 4: status.set(3); break;	//SEND 1 - 1000 wysyła 1 liczbę
            case 5: status.set(3);      //SEND 2 - 1100 wysyła 2 liczby
                    status.set(2);
                    break;
            case 6: status.set(3);      //SEND 3 - 1110 wysyła 3 liczby
                    status.set(2);
                    status.set(1);
                    break;

            case 7: status.set(0);      //IDENTF ERROR- 1111
                    status.set(1);
                    status.set(2);
                    status.set(3);
                    break;
            default: break;
        }
    }
    //ustalanie identyfikatora
    void setIdentf(bitset<4>& b) {
        identf = b;
    }

    //pobieranie operacji
    bitset<2> getOp() {
        return op;
    }
    //pobieranie danych
    vector<int> getData() {
        vector<int> numbers;
        numbers.push_back(L1.to_ullong());    //pierwsza liczba
        numbers.push_back(L2.to_ullong());    //druga liczba
        numbers.push_back(L3.to_ullong());    //trzecia liczba
        return numbers;
    }
    //pobieranie L1
    bitset<64> getL1(){
        return L1;
    }
    //pobieranie L2
    bitset<64> getL2(){
        return L2;
    }
    //pobieranie L3
    bitset<64> getL3(){
        return L3;
    }
    //pobieranie statusu
    bitset<4> getStatus() {
        return status;
    }
    //pobieranie identyfikatora
    bitset<4> getIdentf() {
        return identf;
    }
    //łączy wszstkie pola w jeden bitset
    bitset<208> combineAll() {
        bitset<208> result;
        //operacja
        bitset<208> bop(op.to_string());
        //dane
        bitset<208> bL1(L1.to_string());
        bitset<208> bL2(L2.to_string());
        bitset<208> bL3(L3.to_string());
        //status
        bitset<208> bstatus(status.to_string());
        //identyfikator sesji
        bitset<208> bidentf(identf.to_string());
        //wypelnienie
        //bitset<6> bgap(0);  //wypełnione zerami
        bop <<= 206;
        result |= bop;
        //cout<<result<<endl;
        bL1 <<= 142;
        result |= bL1;
        //cout<<result<<endl;
        bL2 <<= 78;
        result |= bL2;
        //cout<<result<<endl;
        bL3 <<= 14;
        result |= bL3;
        //cout<<result<<endl;
        bstatus <<= 10;
        result |= bstatus;
        //cout<<result<<endl;
        bidentf <<= 6;
        result |= bidentf;
        //cout<<result<<endl;
        cout<<"Wysylany datagram:\n";
        for(int i=0; i<208; i++){
            cout<<result[i];
            if((i+1)%8==0){cout<<" ";}
        }
        cout<<endl;

        bitset<208>buff; bitset<8> line;  int k=0;    ///wez ostatnie 8, odwroc i daj na poczatek
        for(int i=26; i>0; i--){
            for(int j=0; j<8; j++){
                line.set(7-j,result[(i*8)-j-1]);
            }
            for(int m=0; m<8; m++){
                buff.set(k+m,line[m]);
            }
            k+=8;
        }
        cout<<"Przetworzony datagram: "<<buff<<endl;

        cout<<buff<<endl;

        result=buff;
        return result;
    }
    //rozdziela bitset, żeby uzyskać pojedyncze pola
    BinProt getAll(bitset<208> result) {
        BinProt msg;

        bitset<208>buff; bitset<8> line;  int k=0;    ///wez ostatnie 8, odwroc i daj na poczatek
        for(int i=26; i>0; i--){
            for(int j=0; j<8; j++){
                line.set(7-j,result[(i*8)-j-1]);
            }
            for(int m=0; m<8; m++){
                buff.set(k+m,line[m]);
            }
            k+=8;
        }

        cout<<"Przetworzony datagram: "<<endl;

        result=buff;

        //operacja
        bitset<208> oper = result;
        bitset<2> opertemp(oper.to_string());
        msg.op = opertemp;
        //L1
        bitset<208> bL1 = result;
        bL1 <<= 2;
        bitset<64> L1temp(bL1.to_string());
        msg.L1 = L1temp;
        //L2
        bitset<208> bL2 = result;
        bL2 <<= 66;
        bitset<64> L2temp(bL2.to_string());
        msg.L2 = L2temp;
        //L3
        bitset<208> bL3 = result;
        bL3 <<= 130;
        bitset<64> L3temp(bL3.to_string());
        msg.L3 = L3temp;
        //status
        bitset<208> stat = result;
        stat <<= 194;
        bitset<4> stattemp(stat.to_string());
        msg.status = stattemp;
        //identyfikator sesji
        bitset<208> id = result;
        id <<= 198;
        bitset<4> idtemp(id.to_string());
        msg.identf = idtemp;

        return msg;
    }
    //wyswietla wartosci pol
    void printValues() {
        cout<<"Aktualna zawartosc pol obiektu:\n";
        cout << "Operacja: " << op << endl;
        cout << "Liczba 1.: " << L1 << endl;
        cout << "Liczba 2.: " << L2 << endl;
        cout << "Liczba 3.: " << L3 << endl;
        cout << "Status: " << status << endl;
        cout << "Identyfikator sesji: " << identf << "\n";
        cout << "Pelny Datagram: " << op << L1 << L2 << L3 << status << identf << "\n\n";
    }
    //uzupelnia wszystkie pola zerami
    void clean() {
        status.reset();
        identf.reset();
        op.reset();
        L1.reset();
        L2.reset();
        L3.reset();
    }
};

void server_func (bitset<4> id, sockaddr_in sck_addr) {
    BinProt msg,recv;
    bitset<208> buff;
    bitset<4> status;
    bitset<4> identf = id;
    int iResult,iSendResult;

    sck_addr.sin_port+=htons(27015+id.to_ulong());
    int sck_addrlen=sizeof(sck_addr);

    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");	//IPv4
    server.sin_family = AF_INET;
    server.sin_port = htons(27015+id.to_ulong());	//port

    SOCKET local=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (bind(local, (SOCKADDR*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Could not bind name to socket.\n");
        closesocket(local);
        WSACleanup();
        exit(0);
    }

    while (true) {
        //otrzymuje dane
        iResult = recvfrom(local, (char*)&buff, 26, 0, (SOCKADDR*)&sck_addr, &sck_addrlen);
        recv = msg.getAll(buff);

        //WYKONYWANIE OPERACJI NA TRZECH ARGUMENTACH

        msg.clean();	//ustawia ponownie na 0 wszystkie pola
        buff.reset();	//czyści buffer

        cout << "\nOtrzymane wartosci pol:\n\n";
        recv.printValues();
        bitset<2> operation = recv.getOp();     //otrzymana operacja
        bitset<4> status = recv.getStatus();    //otrzymany status
        vector<int> nums = recv.getData();  //otrzymane liczby

        if (operation.to_string() == "00") {    //MOZENIE
            cout << "MOZENIE\n";
            int temp = 0;
            if(status.to_string() == "1000"){   //jedna liczba wysłana
                cout<< "Wyslano: " << nums[0] << endl;
                temp = nums[0];
            }else if(status.to_string() == "1100"){     //dwie liczby wyslane
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << endl;
                temp = nums[0]*nums[1];
            }else if(status.to_string() == "1110"){     //trzy liczby
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << " i " << nums[2] << endl;
                temp = nums[0]*nums[1]*nums[2];
            }
            cout << "Wynik mnozenia: " << temp << "\n";

            msg.setOperation(1);	//mnożenie
            msg.setL1(temp);        //wynik będzie przechowywany w L1
            msg.setIdentf(identf);  //wcześniej ustalony identyfikator sesji
            msg.setStatus(3);	//ACK
            buff = msg.combineAll();
        }

        else if (operation.to_string() == "01") {   //DODAWANIE
            cout << "DODAWANIE\n";
            int temp;
            if(status.to_string() == "1000"){   //jedna liczba wysłana
                cout<< "Wyslano: " << nums[0] << endl;
                temp = nums[0];
            }else if(status.to_string() == "1100"){     //dwie liczby wyslane
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << endl;
                temp = nums[0]+nums[1];
            }else if(status.to_string() == "1110"){     //trzy liczby
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << " i " << nums[2] << endl;
                temp = nums[0]+nums[1]+nums[2];
            }
            cout << "Wynik dodawania: " << temp << "\n";

            msg.setOperation(2);	//dodawanie
            msg.setL1(temp);        //wynik będzie przechowywany w L1
            msg.setIdentf(identf);  //wcześniej ustalony identyfikator sesji
            msg.setStatus(3);	//ACK
            buff = msg.combineAll();
        }
        else if (operation.to_string() == "11") {   //DZIELENIE
            cout << "DZIELENIE\n";
            int temp;
            if(status.to_string() == "1000"){   //jedna liczba wysłana
                cout<< "Wyslano: " << nums[0] << endl;
                temp = nums[0];
            }else if(status.to_string() == "1100"){     //dwie liczby wyslane
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << endl;
                temp = nums[0]/nums[1];
            }else if(status.to_string() == "1110"){     //trzy liczby
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << " i " << nums[2] << endl;
                temp = nums[0]/nums[1]/nums[2];
            }
            cout << "Wynik dzielenia: " << temp << "\n";

            msg.setOperation(3);	//dzielenie
            msg.setL1(temp);        //wynik będzie przechowywany w L1
            msg.setIdentf(identf);  //wcześniej ustalony identyfikator sesji
            msg.setStatus(3);	//ACK
            buff = msg.combineAll();
        }

        else if (operation.to_string() == "10") {   //ODEJMOWANIE
            cout << "ODEJMOWANIE\n";
            int temp;
            if(status.to_string() == "1000"){   //jedna liczba wysłana
                cout<< "Wyslano: " << nums[0] << endl;
                temp = nums[0];
            }else if(status.to_string() == "1100"){     //dwie liczby wyslane
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << endl;
                temp = nums[0]-nums[1];
            }else if(status.to_string() == "1110"){     //trzy liczby
                cout<< "Wyslano: " << nums[0] << " i " << nums[1] << " i " << nums[2] << endl;
                temp = nums[0]-nums[1]-nums[2];
            }
            cout << "Wynik odejmowania: " << temp << "\n";

            msg.setOperation(4);	//odejmowanie
            msg.setL1(temp);        //wynik będzie przechowywany w L1
            msg.setIdentf(identf);  //wcześniej ustalony identyfikator sesji
            msg.setStatus(3);	//ACK
            buff = msg.combineAll();
        }

        msg.printValues();
        iSendResult = sendto(local, (char*)&buff, 26, 0, (SOCKADDR*)&sck_addr, sck_addrlen);
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(local);
            WSACleanup();
            exit(0);
        }
        msg.clean();
        recv.clean();
        buff.reset();

        //SUMOWANIE WIELU LICZB

        status.set(1);	//FIN
        int suma = 0;
        bool flag = true;   //dzięki niej serwer wie kiedy przestać odbierać liczby do sumowania
        while (flag == true) {
            iResult = recvfrom(local, (char*)&buff, 26, 0, (SOCKADDR*)&sck_addr, &sck_addrlen);
            recv = msg.getAll(buff);
            int n = recv.getL1().to_ullong();

            if (recv.getStatus() == status) {	//jeśli klient wyśle fin
                cout << "Wyliczona suma: " << suma << endl;
                msg.setOperation(2);	//dodawanie
                msg.setL1(suma);
                msg.setIdentf(identf);
                msg.setStatus(3);	//ACK
                buff = msg.combineAll();

                //serwer odsyła ostateczny wynik sumowania
                iSendResult = sendto(local, (char*)&buff, 26, 0, (SOCKADDR*)&sck_addr, sck_addrlen);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(local);
                    WSACleanup();
                    exit(0);
                }
                flag = false;   //serwer przestaje odbierać liczby

                msg.clean();
                recv.clean();
                buff.reset();
            }
            else if(recv.getStatus().to_string() == "1000"){    //jedna liczba
                suma += n; 
                cout << "Wyliczona suma: " << suma << endl;
                msg.setOperation(2);	//dodawanie
                msg.setL1(suma);
                msg.setIdentf(identf);
                msg.setStatus(3);	//ACK
                buff = msg.combineAll();

                //serwer sukcesywnie odsyła bieżącą sumę
                iSendResult = sendto(local, (char*)&buff, 26, 0, (SOCKADDR*)&sck_addr, sck_addrlen);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(local);
                    WSACleanup();
                    exit(0);
                }

                msg.clean();
                recv.clean();
                buff.reset();
            }
        }
    }
}

class Client{
public:

    Client(bitset<4> ident,sockaddr_in sck){
        this->id=ident;
        this->sck_addr=sck;
        this->t=thread(server_func,id,sck_addr);
    }

    thread& get_thread(){
        return this->t;
    }
    bitset<4>& get_id(){
        return this->id;
    }
private:
    bitset<4> id;
    int data;
    SOCKADDR_IN sck_addr;
    thread t;
};

bool find_sck_addr(vector<Client> &wek, Client c){
    for (int i=0; i<wek.size(); i++){
        if(wek[i].get_id()==c.get_id()){return true;}
    }
    return false;
}

bool if_only_numbers(string s){
    smatch invalid_character;
    atoi(s.c_str());
    regex_search(s,invalid_character,(regex("\\D")));
    if(invalid_character.empty()){return true;}
    else{return false;}
}
#endif //ZADANIE_HAPPY_PROTOCOL_H
