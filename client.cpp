/*  A Boodhayana S Vishwamithra (2017B4A70761H)
Anuj Kharbada (2017B4A71508H)
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
// typedef struct message message;

using namespace std; 

int main(int argc, char * argv[] ){
    char ip[20] = "127.0.0.1";
    filename = argv[1]; 
    struct sockaddr_in server_address; 
    update_destination_address(8080, ip, server_address); 
   
    int sock; 
    if((sock = socket(PF_INET,SOCK_DGRAM, IPPROTO_UDP)) < 0){
        printf("not able to create socket");
        return 0;
    }
    
    
	
    FILE * fp = fopen(const_cast<char*> (filename.c_str()), "rb");
	char x[500];
    



    







    
    
    seq = 2;

    
    
    while(fgets(x, 400  , fp) != NULL){
        create_entries(string(x));
    }

    

    message * metadata ;
    metadata = create_packet('2', 1, ("THIS IS METADATA\nNUMBER OF PACKETS:"+ to_string(send_queue.size())));
    send_queue[1] = metadata;
    send_queue[0] = create_packet('0', 0, filename); 
    cout<<"size of the send queue " << send_queue.size()<<endl;

    

    

    // for(auto x: send_queue){
    //     cout<<x.first<<endl;
    // }
    //     if(x.second == NULL){
    //         cout<<"Mull "<<x.first<<endl;
    //     }
    //     else{
    //         cout<<"fine "<<x.first<<endl;
    //     }
    // }
    // string x = "ABSV";
    // create_entries(x);

    
    // cout<<"size: "<<send_queue.size(); 
    start_seq = 0, end_seq= 0;
    end_seq = send_queue.size() - 1 + start_seq;
    // cout<<end_seq<<endl;
    
   
    
    // for (auto x: send_queue){
    //     cout<<x.first<<endl;
    //     cout<<x.second->seq_ack<<endl;
    //     cout<<x.second->data<<endl;

    // }
    
    

    std::thread t1(send_info, sock, server_address);
    std::thread t2(receive_acknowledgement, sock, std::ref(server_address));
    
    t1.join();
    t2.join();

    close(sock);
    return 0; 
}


