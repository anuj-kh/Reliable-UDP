/* A Boodhayana S Vishwamithra (2017B4A70761H)
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


typedef struct message message;

using namespace std; 
string filename =""; 
// auto start = std::chrono::system_clock::now();
// auto stop = std::chrono::system_clock::now();
// std::chrono::time_point start, stop; 
string meta = "THIS IS METADATA\nNUMBER OF PACKETS:";
int lac = -1; 
int pkt_size = 504;
// set<int> track_ack; 
int data_size = 500; 
// int cnt = 0;
string res = "";
int start_seq = 0; 
int end_seq ; 


short window_size = 100 ;

// bool can_receive = true; 
bool received_fin = false;
map<short, bool> acknowledged; 

short unacked = -1; 
mutex mut1, mut2;


struct message { 
	char packet_type ; // 0, 1, 2, 3 : syn / ack / data / fin
      	
	short seq_ack ; 

	string data ; 		
	// using timer here is no use
};  

map < short, message*> send_queue, receive_buffer; 
pair<short, short> window; 
short seq, ack; /* seq is sequence number of sending pkt, ack is acknowledgment number of received pkt*/

// A few utility functions: 
message * create_packet(char type, short seq , string data){
	message * mess = new message();
	mess->packet_type = type; 
	mess-> seq_ack = seq;
	mess->data = data;   

	return mess; 
}

bool is_ack(message * mess){
    return (mess-> packet_type == '1'); 
}

bool is_data(message* mess){
    return (mess-> packet_type == '2');
}

bool is_syn(message* mess){
    return (mess-> packet_type == '0'); 
}

bool is_fin(message* mess){
    return (mess-> packet_type == '3'); 
}

void update_source_address(int client_port, char* client_ip, sockaddr_in &source_address ){
	/*
		Takes the ip and port of client and updates.  
	*/	
    source_address.sin_family = AF_INET ;
	source_address.sin_port = htons(client_port);
	// source_address.sin_addr.s_addr = INADDR_ANY;
	inet_aton(client_ip, &source_address.sin_addr); 
}

void update_destination_address(int server_port, char * server_ip, sockaddr_in &destination_address ){
	/*
		Takes the ip and port of server and updates.  
	*/
	destination_address.sin_family = AF_INET; 
	// destination_address.sin_addr.s_addr = INADDR_ANY;
	inet_aton(server_ip, &destination_address.sin_addr); 
	destination_address.sin_port = htons(server_port); 
	// std::cout<<"aa="<<server_port<<endl; 
	// std::cout<<destination_address.sin_addr.s_addr<<endl;
	// std::cout<<destination_address.sin_port<<endl;
}

void err_n_die(string error_message){
	std::cout<<error_message<<endl; 
	return ; 
}


void send_pkt(message * mess, int sock, struct sockaddr_in destination_address ){
	// std::cout<<"Inside send pkt\n";
	char x[pkt_size] ; 
	// // cout<<"hoi"<<endl;
	// if(mess == NULL){
	// 	cout<<"hio"<<endl;
	// }
	// cout<<mess->packet_type<<" "<<mess->seq_ack<<endl;
	
	snprintf(x , pkt_size, "%c~%hi~" , mess->packet_type, mess->seq_ack  );
	string info(x); 
	// cout<<"msg"<<endl;
	info.append(mess->data);
	// std::cout<<"Message in send_pkt is :\n";
	// for(int i=0;i<info.length();i++)
	// {	
		// std::cout<<info[i];
	// }
	// std::cout<<endl;
	int len=info.length();
	char* c = const_cast<char*>(info.c_str());
	// std::cout<<c<<" "<<sizeof(char)*len<<endl;
	// string y = format("{}{}{}",mess->packet_type, mess->seq_ack , mess-> data ); 
	int sendstatus = sendto(sock, c , sizeof(char)*len, 0 , (struct sockaddr * ) &destination_address, sizeof(destination_address)); 
	if(sendstatus < 0){
		err_n_die("Could not send!"); 
	}
	
	return ; 
}


int receive_pkt(int sock, struct sockaddr_in &source_address, message **msg ){ 
	/*
        Source address is passed as pointer
    */
	// std::cout<<"In recv pkt\n";

	char source_message[pkt_size+ 204] ; //
	memset( source_message, '\0', sizeof(char)*(pkt_size+ 100) );

	socklen_t len = sizeof(source_address);  
	
	int receivestatus = recvfrom(sock, source_message, pkt_size, 0, (struct sockaddr*)&source_address, &len);
	// std::cout<<"recvfrom ho gaya\n";  
	if(receivestatus <= 0){
        
		err_n_die("Could not receive stuff!" );
		
        return receivestatus;
        // bool can_receive = false;  
	}
	// std::cout<<"Received this packet:\n";
	// for(int i = 0; i< (int)pkt_size; i++){
	// 	std::cout<<source_message[i];
	// }

	string x(source_message); 
       		
	message* msg_copy = new message();
    msg_copy->packet_type = source_message[0]; 
	// std::cout<<"\naa="<<(msg_copy->packet_type)<<endl;
	//First '~' is stored at position 1 because packet_type is char
	int l1 =-1; // this stores the second '~'. 
	for(int i = 2; i< (int)pkt_size; i++){
		if(source_message[i] == '~'){
			l1 = i;
			break;
		}
	}
	msg_copy->seq_ack = (short) stoi(x.substr(2, l1-2));
	// std::cout<<"\naa2="<<(msg_copy->seq_ack)<<endl;
	msg_copy-> data = x.substr(l1+1, x.size()-l1); 
	// std::cout<<"End of receive_pkt "<<msg_copy->data<<endl;
	*msg=msg_copy;
	return receivestatus;  
}

void create_entries(string info){
	// std::cout<<"in create_entries: "<<info<<endl;

	int val = (info.size() % data_size); 
	int num = 1; 
	if(info.size() > data_size){
		num = info.size() / data_size + ((info.size() % data_size == 0 ) ? 0 : 1); 
	}	
    

	// message * metadata = create_packet('2', seq, ("THIS IS METADATA\nNUMBER OF PACKETS:"+ to_string(num)));

	// send_queue[seq] = metadata; 
	

	for(int i = 0; i< num; i++){
		int len = min((int)data_size, ((int)info.size() -  i*(int)data_size) ); 
		string val = info.substr(i*data_size, len); 

		message*x = create_packet('2', seq, val);
		send_queue[seq++] = x;
	}
	

}
void timeout(short x,int sock,struct sockaddr_in destination_address, bool & val){
	while(acknowledged[x] == false){
		std::this_thread::sleep_for(500ms) ; 
		if(acknowledged[x]){
			break; 
		}
		send_pkt(send_queue[x], sock, destination_address);
	}
	val = true;
	return ;
}

void send_info( int sock, struct sockaddr_in destination_address){
	std::cout<<"In send_info\n";

	end_seq = send_queue.size() - 1 + start_seq; 

	// cout<<start_seq<<" "<<end_seq<<endl;
	acknowledged.clear();
	
	mut1.lock(); 
	
	window.first = start_seq;
	window.second = min((window.first + window_size - 1), end_seq);  

	short unsent_first = window.first; 

	mut1.unlock(); 

	// cout<<"window_first: "<<window.first<<endl;
	// cout<<"window_sec: "<<window.second<<endl;
	// cout<<"unseen_first: "<<unsent_first<<endl;
	// std::cout<<"Completed mut1 in send_info\n";

	short not_yet_acked; 


	while(true){
		// cout<<"Inside the while loop of send info"<<endl;		
		
		if(acknowledged[end_seq]){
			cout<<"All packets are acknowledged"<<endl;
			return;
			
		}

		// std::cout<<"Window boundary "<<"first: "<<window.first<<" second: "<<window.second<<endl;
		
		mut1.lock(); 

        not_yet_acked = window.first;
		for(short i = unsent_first; i<= window.second; i++ ){
			// cout<<i<<" "<<endl;
			// if(send_queue[i] == NULL){
			// 	cout<<"null "<<i<<endl;
			// }
			send_pkt(send_queue[i],sock, destination_address); 
			
		}
		// return ;
		unsent_first = window.second ; 

		mut1.unlock(); 

		// cout<<"Second mut1 in send_info is done."<<endl;
		
		while(acknowledged[not_yet_acked] == false){
			std::this_thread::sleep_for(100ms);
			if(acknowledged[not_yet_acked] == true){
				break;
			}
			else{
				send_pkt(send_queue[not_yet_acked], sock, destination_address);
				// std::cout<<"Timeout: Resent"<<endl;
			}
		}
		
		// std::cout<<"Out of timeout loop"<<endl;
		
		// std::cout<<"end_seq: "<<end_seq<<endl;
		// std::cout<<"acknowledged[end_seq]: "<<acknowledged[end_seq]<<endl; 
		// std::cout<<"not yet acked: "<<not_yet_acked<<endl;
		// std::cout<<"acknowledged of not_yet_acked : "<<acknowledged[not_yet_acked]<<endl;		
		
	}
		
	return;
	
}


void receive_acknowledgement(int sock, struct sockaddr_in &source_address){
	acknowledged.clear();
	// set<short> track_ack;
	// cout<<"Recieve_ack function:"<<endl;

	short lastaccepted = -1;  
    bool flag = false; 

	message * pkt ; 
	int can_recv = receive_pkt(sock, source_address, &pkt);
	// cout<<"Can_recv: "<<can_recv<<endl;

	while(can_recv > 0){

		// cout<<"Recv ack: Inside while(can_recv): "<<endl;;
		// cout<<"Is Ack: "<<is_ack(pkt)<<endl;
		// cout<<pkt->seq_ack<<"sequence"<<endl;

		if(is_ack(pkt)){
			
			lastaccepted = pkt->seq_ack - 1; 
			for(int i = start_seq; i< pkt->seq_ack; i++){
				acknowledged[i] = true; 
			}
            if(flag == false and acknowledged[0] == true){
                cout<<"Connection established.\n"<<endl;
                flag = true;
            }

			if(pkt->seq_ack == window.first){
				// cout<<"Received Neg-Ack."<<endl;
				send_pkt(send_queue[pkt->seq_ack], sock, source_address);
			}
			else if(pkt->seq_ack < window.first){
				;
			}
			else{
				// cout<<"Received Pos-Ack."<<endl;

				mut1.lock();
				
				window.first = pkt->seq_ack;
				window.second = min((window.first + window_size - 1), end_seq); 
			
				mut1.unlock();
			}
			// std::cout<<"Window boundary "<<"first: "<<window.first<<" second: "<<window.second<<endl;
			// cout<<"Acknowledged ? "<<endl;
			// for (auto c: acknowledged){
			// 	cout<<c.first<<" "<<c.second<<endl;
			// }
			if(acknowledged[end_seq] == true){
				can_recv = -1;
				return ; 
			}
			else{
				can_recv = receive_pkt(sock, source_address, &pkt);
			}
		}
	}
		
}

void receive_info( int sock, struct sockaddr_in &source_address){
	
	cout<<"In recv Info."<<endl;
	map<short, message*> data; 

	short last_corr_received = -1; 

    message * pkt; 
	int can_recv = receive_pkt(sock, source_address, &pkt);
	// cout<<"Can_recv: "<<can_recv<<endl;
	auto start = std::chrono::system_clock::now();

	int npackets = -2; 

	while(can_recv >0){
		// this_thread::sleep_for(1000ms);
		// cout<<"Recv info: Inside while(can_recv): "<<endl;

		if(is_data(pkt) or is_syn(pkt)){
			// cout<<"Sequence: "<<(pkt->seq_ack)<<"\nData: "<<(pkt->data)<<endl; 
			if(data.count(pkt->seq_ack) == 0){
				data[pkt->seq_ack] = pkt;
			}
            if(is_syn(pkt)){
                filename = pkt->data; 
            }

			message * ack; 
			// cout<<last_corr_received<<endl;
			if(last_corr_received + 1 == pkt->seq_ack){
				// cout<<"c1"<<endl;
				// cout<<"Correct ordder packet."<<endl;
				last_corr_received++; 
				ack = create_packet('1',last_corr_received + 1, "");
				send_pkt(ack, sock, source_address);

				// cout<<"here1"<<endl;
				 
				while(receive_buffer.empty() == false && last_corr_received+1 == receive_buffer.begin()->first){
					last_corr_received ++; 
					ack = create_packet('1',last_corr_received + 1, "");
					send_pkt(ack, sock, source_address);
					receive_buffer.erase(last_corr_received);	
				}
				// cout<<"here2"<<endl;
				if((pkt->data).find(meta) != string::npos){ // metadata
					// cout<<"here3"<<endl;
					npackets= stoi((pkt->data).substr(35, (pkt->data).size() - 35));
					// cout<<"Number of packets: "<<npackets<<endl;
				}
				
			}
			else if(pkt->seq_ack <= last_corr_received){
				// cout<<"c2"<<endl;
				ack = create_packet('1', pkt->seq_ack + 1, "");
				send_pkt(ack,sock,source_address);
			}
			else{
				// cout<<"c3"<<endl;	
				if(receive_buffer.count(pkt->seq_ack) == 0){
					receive_buffer[pkt->seq_ack] = pkt;
				}
				ack = create_packet('1',last_corr_received + 1, "");
				send_pkt(ack, sock, source_address);
			}

		}
		else{
			break;
		}

		// cout<<"R data size: "<<data.size()<<endl;
		if(data.size() == npackets+2){
			res = "";
			data.erase(data.begin()->first);
            data.erase(data.begin()->first);
			for(auto x: data){
				// cout<<x.first<<endl;
				// cout<<x.second->data<<" "<<endl;
				res += x.second->data; 
			}
			can_recv = -1;
			auto stop = std::chrono::system_clock::now();
			double y = (double)chrono::duration_cast<chrono::seconds>(stop - start).count();
			std::cout<<"\n Throughput : "<<(res.size()*1.0)/y <<endl;
			return ;
		}
		int can_recv = receive_pkt(sock, source_address, &pkt);
		// cout<<"recv status: " << can_recv<<endl;
	}
	
}

void client_connection(int sock , struct sockaddr_in &destination_address){
    int status = connect(sock, (struct sockaddr *) & destination_address, sizeof(destination_address)); 
    if(status < 0){
        err_n_die("Could not connect "); 
        return ; 
    }
    send_queue.clear(); 
    send_queue[0] = create_packet('0', 0, "");

    std::thread t1 (send_info, sock , destination_address); 
    std::thread t2 (receive_acknowledgement, sock, std::ref(destination_address)); 
    t1.join();
    t2.join(); 
   
    // std:: thread t4 (receive_acknowledgement, sock, std::ref(destination_address));  

    
}

void server_connection(int sock, struct sockaddr_in &source_address){
    
    std::thread t1(receive_info, sock, std::ref(source_address)); 
    t1.join();
    std::thread t2(receive_acknowledgement, sock, std::ref(source_address));
    t2.join(); 
}
		



