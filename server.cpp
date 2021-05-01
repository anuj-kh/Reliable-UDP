/* A Boodhayana S Vishwamithra (2017B4A70761H)
Anuj Kharbanda (2017B4A71508H)
Giridhar Bajpai (2017B4A71451H)
Vinay Agarwal (2017B3A70661H)
Soumil Agarwal (2017B4A71606H)
Thejas Sasikumar (2017B4A70614H) */


#include<bits/stdc++.h>
#include<stdlib.h>
#include<sys/socket.h> 
#include<sys/types.h> 
#include<unistd.h> 
#include<arpa/inet.h> 
#include<netinet/in.h> 

#include "protocol.h"

using namespace std; 

int main(){

    char ip[20] = "127.0.0.1";

    struct sockaddr_in server_address, client_address; 
    update_destination_address(8080, ip, server_address); 
	
    int sock ; 
	sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	if(sock < 0){
		printf("Socket cannot be created" ); 
        return 0;
	}

    int bindstatus = bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)); 
	
	if(bindstatus < 0){
		printf("Could not bind the socket")	; 
        return 0;   
	}

    // server_connection(sock, std::ref(client_address)); 

    cout<<"Connection done!"<<endl;

    
    std::thread t1(receive_info, sock, std::ref(client_address));
    t1.join();
    filename = "copy_" + filename; 
    char * newfile = const_cast<char*> (filename.c_str());

    FILE * fp = fopen(newfile, "wb"); 
    char * k = const_cast<char*>(res.c_str());
    

    if (fp)
        fputs(k,fp);
    else
        perror("File opening failed");
    
    fclose(fp);

    // fputs(k, fp);
    // cout<<"sent: "<<res<<endl;

    close(sock); 
    return 0; 
}
