#include <regex>
#include "Protocol.h"

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) { cout << "Blad inicjalizacji." << endl; }

    SOCKET sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    SOCKADDR_IN serwer;
    int serwerlen = sizeof(serwer);
    serwer.sin_addr.s_addr = inet_addr("127.0.0.1");
    serwer.sin_family = AF_INET;
    serwer.sin_port = htons(27015);

    //Connection = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sd == INVALID_SOCKET)
    {
        fprintf(stderr, "Could not create socket.\n");
        WSACleanup();
        exit(0);
    }

    //-----------------------------------------------

    int iResult, iSendResult;

    BinProt msg;	//domyślnie wszystkie pola na 0
    BinProt recv;
    bitset<208> buff;	//datagram

    //uzgadnianie identyfikatora sesji
    msg.setStatus(1); //status na begin
    buff = msg.combineAll();

    //wysyła datagram z statusem begin, aby rozpocząć sesję
    iSendResult = sendto(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, sizeof(serwer));
    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sd);
        WSACleanup();
        exit(0);
    }

    msg.clean();	//ustawia na 0 wszystkie pola
    buff.reset();	//czyści buffer

    //otrzymanie odpowiedzi od serwera
    iResult = recvfrom(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, &serwerlen);
    recv = msg.getAll(buff);

    bitset<4>identf = recv.getIdentf();	//identyfikator sesji
    cout << endl << endl << "Ustalono identyfikator sesji: " << identf << endl << endl;

    serwer.sin_port=htons(27015+identf.to_ulong()); //port
    cout<<"NOWY PORT: "<<27015+identf.to_ulong()<<endl;

    while (true) {

        //WYKONYWANIE OPERACJI NA TRZECH ARGUMENTACH

        msg.clean();	//ustawia na 0 wszystkie pola
        buff.reset();	//czyści buffer
        recv.clean();   //ustawia na 0 wszystkie pola
        string argS,opS,L1S,L2S,L3S;
        int arg,op,L1,L2,L3;

        do{
            cout << "Podaj na ilu liczbach chcesz wykonac operacje(1do3): ";
            cin>>argS;
            if(!if_only_numbers(argS)||atoi(argS.c_str())>3||atoi(argS.c_str())<1||argS.size()>10){
                cout<<"Invalid input, try again.\n"<<endl;
            }
        }while(!if_only_numbers(argS)||atoi(argS.c_str())>3||atoi(argS.c_str())<1||argS.size()>10);
        arg=atoi(argS.c_str());

        do{
            cout << "Podaj pierwsza liczbe na ktorej chcesz wykonac operacje: ";
            cin>>L1S;
            if(!if_only_numbers(L1S)||L1S.size()>10){
                cout<<"Invalid input, try again.\n"<<endl;
            }
        }while(!if_only_numbers(L1S)||L1S.size()>10);
        L1=atoi(L1S.c_str());

        if(arg == 2|| arg == 3){
            do{
                cout << "Podaj druga liczbe na ktorej chcesz wykonac operacje: ";
                cin>>L2S;
                if(!if_only_numbers(L2S)||L2S.size()>10){
                    cout<<"Invalid input, try again.\n"<<endl;
                }
            }while(!if_only_numbers(L2S)||L2S.size()>10);
            L2=atoi(L2S.c_str());
        }
        if(arg == 3){
            do{
                cout << "Podaj trzecia liczbe na ktorej chcesz wykonac operacje: ";
                cin>>L3S;
                if(!if_only_numbers(L3S)||L3S.size()>10){
                    cout<<"Invalid input, try again.\n"<<endl;
                }
            }while(!if_only_numbers(L3S)||L3S.size()>10);
            L3=atoi(L3S.c_str());
        }
        cout << "Wybierz operacje:\n1. Mnozenie\n2. Dodawanie\n3. Dzielenie\n4. Odejmowanie\n\n";
        do{
            cout << "Podaj trzecia liczbe na ktorej chcesz wykonac operacje: ";
            cin>>opS;
            if(!if_only_numbers(opS)||atoi(opS.c_str())<1||atoi(opS.c_str())>4||opS.size()>10){
                cout<<"Invalid input, try again.\n"<<endl;
            }
        }while(!if_only_numbers(opS)||atoi(opS.c_str())<1||atoi(opS.c_str())>4||opS.size()>10);
        op=atoi(opS.c_str());

        if(arg == 1){
            msg.setStatus(4); //SEND1
            msg.setL1(L1);
        }else if(arg == 2){
            msg.setStatus(5); //SEND2
            msg.setL1(L1);
            msg.setL2(L2);
        }else if(arg == 3){
            msg.setStatus(6); //SEND3
            msg.setL1(L1);
            msg.setL2(L2);
            msg.setL3(L3);
        }
        msg.setOperation(op);
        msg.setIdentf(identf);
        cout << endl;
        msg.printValues();
        buff = msg.combineAll();
        
        //wysyłanie danych
        iSendResult = sendto(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, sizeof(serwer));
        if (iSendResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(sd);
            WSACleanup();
            exit(0);
        }

        msg.clean();	//ustawia na 0 wszystkie pola
        buff.reset();	//czyści buffer

        //otrzymanie odpowiedzi od serwera
        iResult = recvfrom(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, &serwerlen);
        recv = msg.getAll(buff);

        bitset<4> recv_stat;
        recv_stat.set(2);   //ACK
        if (recv.getStatus() != recv_stat) {cout << endl << "ERROR" << endl << endl;}
        else {cout << "Wynik operacji: " << recv.getL1().to_ulong() << endl;}

        //SUMOWANIE WIELU LICZB

        msg.clean();
        recv.clean();
        buff.reset();

        bool flag = true;
        while (flag == true) {
            int n;
            smatch invalid_character3;
            string odp3;
            do{
                cout << "Podaj liczbe do zsumowania (aby zakonczyc podawanie wpisz 0): ";
                cin>>odp3;
                regex_search(odp3,invalid_character3,(regex("\\D")));

                if(!invalid_character3.empty()||odp3.size()>9){cout<<"Invalid input, try again.\n"<<endl;}
            }while(!invalid_character3.empty()||odp3.size()>9);
            n=atoi(odp3.c_str());

            if (n == 0) {	//koniec wysyłania liczb
                msg.setStatus(2); //FIN
                msg.setIdentf(identf);
                buff = msg.combineAll();
                iSendResult = sendto(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, sizeof(serwer));
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(sd);
                    WSACleanup();
                    exit(0);
                }
                flag = false;
                msg.clean();
                recv.clean();
                buff.reset();
            }
            else {		//wysyłanie kolejnych liczb
                msg.setL1(n);
                msg.setIdentf(identf);
                msg.setOperation(2);
                msg.setStatus(4);   //wysyłamy 1 liczbę
                buff = msg.combineAll();
                iSendResult = sendto(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, sizeof(serwer));
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(sd);
                    WSACleanup();
                    exit(0);
                }
                //obecny wynik sumowania
                iResult = recvfrom(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, &serwerlen);
                recv = msg.getAll(buff);
                if (recv.getStatus() != recv_stat) { cout << endl << "ERROR" << endl << endl; }
                else { cout << "Obecna suma: " << recv.getL1().to_ulong() << endl; }
                msg.clean();
                recv.clean();
                buff.reset();
            }
        }
        //wynik sumowania
        iResult = recvfrom(sd, (char*)&buff, 26, 0, (SOCKADDR*)&serwer, &serwerlen);
        recv = msg.getAll(buff);
        if (recv.getStatus() != recv_stat) { cout << endl << "ERROR" << endl << endl; }
        else { cout << "Wynik sumowania: " << recv.getL1().to_ulong() << endl; }
    }
    closesocket(sd);
    WSACleanup();
    system("pause");
    return 0;
}


