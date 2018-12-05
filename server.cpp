#include "Protocol.h"

int main()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) { cout << "Blad inicjalizacji." << endl; }

    SOCKET sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    SOCKADDR_IN server;		//adres z którym połączymy
    int serverlen = sizeof(server);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");	//IPv4
    server.sin_family = AF_INET;
    server.sin_port = htons(27015);	//port

    if (bind(sd, (SOCKADDR*)&server, serverlen) == SOCKET_ERROR)
    {
    	fprintf(stderr, "Could not bind name to socket.\n");
    	closesocket(sd);
    	WSACleanup();
    	exit(0);
    }


    SOCKADDR_IN client;
    int clientlen = sizeof(client);
    ZeroMemory(&client, clientlen);

    //-------------------------------------------------

    int iResult, iSendResult;

    BinProt msg;
    bitset<208> buff;
    bitset<4> status;
    status.set(0);  //musi odebrać najpierw BEGIN
    vector<Client> clients;	//wektor klientów
    int session_counter=1;
    //odbiera informację od klienta o rozpoczęciu przesyłu danych

    while(true){
        iResult = recvfrom(sd, (char*)&buff, 26, 0, (SOCKADDR*)&client, &clientlen);
        BinProt recv = msg.getAll(buff);

        //jeśli otrzymał status begin
        if (recv.getStatus() == status) {
            msg.clean();	//ustawia ponownie na 0 wszystkie pola
            buff.reset();	//czyści buffer

            bitset<4> identf(session_counter);	//identyfikator sesji
            msg.setStatus(3);		//ACK
            msg.setIdentf(identf);	//ustala identyfikator sesji
            buff = msg.combineAll();//datagram
            session_counter++;

            clients.push_back(Client(identf,client));

            //wysyła datagram z identyfikatorem sesji
            iSendResult = sendto(sd, (char*)&buff, 26, 0, (SOCKADDR*)&client, clientlen);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(sd);
                WSACleanup();
                exit(0);
            }

            msg.clean();	//ustawia ponownie na 0 wszystkie pola
            buff.reset();	//czyści buffer
            recv.clean();

        }
    }
    closesocket(sd);
    WSACleanup();
    system("pause");
    return 0;
}