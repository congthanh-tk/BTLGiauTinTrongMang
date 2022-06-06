#include "main.h"

#ifdef __MACH__
 #include <mach/clock.h>
 #include <mach/mach.h>
#endif

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
extern char *__progname;
int low = 0; // 1
int high = 0; // 0

void
inform_disconnected(char *binary_string, char *address)
{
	strcat(binary_string, "0");
	revstr(binary_string);
	printf("binary string received from %s is: %s\n", address, binary_string);
	char* msg = BinaryToASCII(binary_string);
	revstr(msg);
	printf("Message received from %s is: %s \n", address, msg);
	fprintf(stderr, "The connection with %s was closed by the foreign host.\n", address);
    printf("\n\n");
}

void 
*socketThread(void *arg)
{
    socketInfor si11;
    si11 = *((socketInfor *)arg);
    int newSocket = si11.socket;
    char address[2048] = "";
    strcpy(address, si11.address);
    // struct sockaddr_in saNS = *((struct sockaddr_in *)arg[1]);
    char binary_string[2048] = "";
    int maxNS = newSocket;
    int firstNum = 0;
    int checkFirst = 1;
    fd_set rsetNS; //set of socket descriptors
    char buf[1024];  //data buffer of 1K 
    int x;
    options_t *options_array;
    int ret = 0;
    cceap_header_t *hdr;

    long ms;
	time_t s;
	struct timespec spec_now;
	struct timespec spec_last;
	int first_call = 1;

    for (;;) {
        FD_ZERO(&rsetNS);
        FD_SET(newSocket, &rsetNS);
        
        ret = select(maxNS + 1, &rsetNS, NULL, NULL, NULL);
        if (ret) {
            int size_header = sizeof(cceap_header_t);
        
            bzero(buf, sizeof(buf));

            if (FD_ISSET(newSocket, &rsetNS)) {
                if ((x = recv(newSocket, buf, sizeof(buf) - 1, 0)) == -1){
                    printf("Error recv()");
                    // err(ERR_EXIT, "recv()");
                }
                if (x == 0){
                    inform_disconnected(binary_string, address);
                    strcpy(binary_string, "");
                }
                
                if (x >= size_header) {
                    char dst_n_pad[DSTPADSIZ + 1] = { '\0' };
                    
                    hdr = (cceap_header_t *) buf;

                    int seq_num = hdr->sequence_number;

                    // if(checkFirst){
                    //     firstNum = seq_num;
                    //     checkFirst = 0;
                    // } else {
                    //     if((seq_num - firstNum) > 0){
                    //         strcat(binary_string, "0");
                    //     } else {
                    //         strcat(binary_string, "1"); 
                    //     }
                    //     checkFirst = 1;
                    // }
                    #ifdef __MACH__ /* code from Stackoverflow.com (Mac OS lacks clock_gettime()) */
                        #warning "Including experimental code for MacOS. CCEAP runs best on Linux!"
                        clock_serv_t cclock;
                        mach_timespec_t mts;

                        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
                        clock_get_time(cclock, &mts);
                        mach_port_deallocate(mach_task_self(), cclock);
                        spec_now.tv_sec = mts.tv_sec;
                        spec_now.tv_nsec = mts.tv_nsec;
                    #else
                        clock_gettime(CLOCK_REALTIME, &spec_now);
                    #endif

                    printf("received data (%d bytes) from %s and %d:\n", x, address, newSocket);
                    printf(" > time diff to prev pkt: ");
                    if (first_call) {
                        printf("0.000\n");
                        first_call = 0;
                    } else {
                        s  = spec_now.tv_sec - spec_last.tv_sec;
                        ms = (spec_now.tv_nsec - spec_last.tv_nsec) / 1.0e6;	
                        if (ms < 0) {
                            ms = 1000 + ms;
                            s -= 1;
                        }
                        int time = s*1000+ms;
                        if(abs(time - high) < abs(time - low)){
                            strcat(binary_string, "0"); 
                        } else {
                            strcat(binary_string, "1");
                        }
                        printf("%"PRIdMAX".%03ld\n", (intmax_t)s, ms);
                    }
                    bcopy(&spec_now, &spec_last, sizeof(struct timespec));

                    printf(" > sequence number:       %d\n", seq_num);
                    printf(" > destination length:    %d\n", hdr->destination_length);
                    printf(" > dummy value:           %d\n", hdr->dummy);
                    strncpy(dst_n_pad, hdr->destination_and_padding, DSTPADSIZ);
                    printf(" > destination + padding: %s\n", dst_n_pad);
                    printf(" > number of options:     %d\n", hdr->number_of_options);
                    if (hdr->number_of_options >= 1) {
                        int i;
                        
                        /* check whether too many options */
                        if ((x - sizeof(cceap_header_t)) > (MAX_NUM_OPTIONS * sizeof(options_t))) {
                            fprintf(stderr, "received too many options\n");
                            // exit(ERR_EXIT);
                        }
                        /* check whether number_of_options is correct */
                        if (hdr->number_of_options * sizeof(options_t) > (x - sizeof(cceap_header_t))) {
                            fprintf(stderr, "did not receive enough bytes for %d options\n",
                                hdr->number_of_options);
                            break;
                            // exit(ERR_EXIT);
                        } else {
                            
                        }
                        
                        options_array = (options_t *) (buf + sizeof(cceap_header_t));
                        printf("    > options overview:\n");
                        for (i = 0; i <  hdr->number_of_options; i++) {
                            printf("\t\t\t  #%d: (identifier: %hhu, type: %hhu, value: %hhu)\n",
                                i + 1,
                                (options_array + i)->opt_identifier,
                                (options_array + i)->opt_type,
                                (options_array + i)->opt_value);
                        }
                    }
                } else {
                    // printf("did not implement caching of segments but received only part of a "
                    // 	"buffer -- sry ...\n");
                    // exit(ERR_EXIT);
                    // printf("********* Connect to new client **********");
                    break;
                }
                
            } else {
                printf("huh?!\n");
                // exit(ERR_EXIT);
                break;
            }
        } else if (ret == 0) {
            /* do nothing */
        } else { /* ret = -1 */
            if (errno == EINTR)
                continue;
            else{
                printf("Error Select");
                break;
                // err(ERR_EXIT, "select");
            }
        }
    }
    close(newSocket);
    pthread_exit(NULL);
}

int
main(int argc, char *argv[])
{	
    int ch;
    int connfd;
    struct sockaddr_in sa; // Socket addresss
    int lst_port = 0;
    int quiet = 0;
    fd_set rset; //set of socket descriptors
    int opt = TRUE;
    int max_sd;  
    int master_socket , addrlen , activity ;   
    pthread_t tid[60];

    if (argc <= 4) {
		fprintf(stderr, "usage: %s [-P port] [low_time (ms)] [high_time (ms)]\n", __progname);
		exit(1);
	}
	
	low = atoi(argv[3]);
	high = atoi(argv[4]);

	while ((ch = getopt(argc, argv, "vhP:q")) != -1) {
		switch (ch) {
		case 'P':
			/* TCP port to listen on */
			lst_port = atoi(optarg);
			break;
		case 'q':
			quiet = 1;
			break;
		case 'h':
		default:
			usage(USAGE_SERVER);
			/* NOTREACHED */
		}
	}

	if (!quiet)
		print_gpl();

	/* basic checks of provided parameters */
	if (lst_port >= 0xffff || lst_port < 1) {
		fprintf(stderr, "TCP listen port out of range or not specified.\n");
		exit(ERR_EXIT);
	}
	
	/* short welcome notice */
	if (quiet) {
		fprintf(stdout, "---\n");
	} else {
		fprintf(stdout, "CCEAP - Covert Channel Educational Analysis Protocol (Server)\n");
		// fprintf(stdout, "   => version: " CCEAP_VER ", written by: " CCEAP_AUTHOR "\n");
	}
	
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  


    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }
    
    /* prepare and create the connection to the peer */
    bzero(&sa, sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(lst_port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(master_socket, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  

    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  

    //accept the incoming connection 
    addrlen = sizeof(struct sockaddr_in);  
    puts("Waiting for connections ..."); 

    int count = 0;
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&rset);  
     
        //add master socket to set 
        FD_SET(master_socket, &rset);  
        max_sd = master_socket;  
     
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &rset , NULL , NULL , NULL);  
       
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &rset))  
        {  
            if ((connfd = accept(master_socket, 
                    (struct sockaddr *)&sa, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }

            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , connfd , inet_ntoa(sa.sin_addr) , ntohs(sa.sin_port));  
            socketInfor si;
            si.socket = connfd;
            si.address = inet_ntoa(sa.sin_addr);

            //for each client request creates a thread and assign the client request to it to process
            //so the main thread can entertain next request
            if( pthread_create(&tid[count++], NULL, socketThread, &si) != 0 )
                printf("Failed to create thread\n");
            if(count >= 50){
                count = 0;
                while(count < 50)
                {
                    pthread_join(tid[count++], NULL);
                }
                count = 0;
            }
        }
    }  
         
    return 0;  
}  


