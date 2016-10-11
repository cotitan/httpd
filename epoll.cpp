#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <sys/epoll.h>

#define IPADDRESS   "127.0.0.1"
#define PORT        9091
#define MAXSIZE     1024
#define LISTENQ     5
#define FDSIZE      1000
#define EPOLLEVENTS 100

void handle_events(int epollfd,struct epoll_event *events,
        int num,int listenfd,char *buf);
void handle_accpet(int epollfd,int listenfd);
void delete_event(int epollfd,int fd,int state);
void add_event(int epollfd,int fd,int state);
void do_read(int epollfd,int fd,char *buf);
void do_write(int epollfd,int fd,char *buf);
void modify_event(int epollfd,int fd,int state);
int socket_bind(const char *ip, short port);

int main() {
    
    //创建一个描述符
    int epollfd = epoll_create(FDSIZE);
    int listenfd = socket_bind(IPADDRESS, PORT);
    struct epoll_event events[EPOLLEVENTS];

    char buf[1024];

    //添加监听描述符事件
    add_event(epollfd, listenfd, EPOLLIN);

    //循环等待
    for ( ; ; ){
        //该函数返回已经准备好的描述符事件数目
        int ret = epoll_wait(epollfd, events, EPOLLEVENTS,-1);
        //处理接收到的连接
        handle_events(epollfd, events, ret, listenfd, buf);
    }
}

//事件处理函数
void handle_events(int epollfd,struct epoll_event *events,
        int num,int listenfd,char *buf) {
    int i;
    int fd;
    //进行遍历;这里只要遍历已经准备好的io事件。num并不是当初epoll_create时的FDSIZE。
    for (i = 0;i < num;i++) {
        fd = events[i].data.fd;
        //根据描述符的类型和事件类型进行处理
        if ((fd == listenfd) &&(events[i].events & EPOLLIN))
            handle_accpet(epollfd,listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(epollfd,fd,buf);
        else if (events[i].events & EPOLLOUT)
            do_write(epollfd,fd,buf);
    }
}

//添加事件
void add_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

//处理接收到的连接
void handle_accpet(int epollfd,int listenfd) {
    int clifd;
    struct sockaddr_in cliaddr;     
    socklen_t  cliaddrlen;     
    clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);     
    if (clifd == -1)         
        perror("accpet error:");     
    else {         
        printf("accept a new client: %s:%d\n",
            inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);                       //添加一个客户描述符和事件         
        add_event(epollfd,clifd,EPOLLIN);     
    } 
 }

//读处理
void do_read(int epollfd,int fd,char *buf) {
    int nread;
    nread = read(fd,buf,MAXSIZE);
    if (nread == -1) {         
        perror("read error:");         
        close(fd); //记住close fd        
        delete_event(epollfd,fd,EPOLLIN); //删除监听 
    }
    else if (nread == 0) {         
        fprintf(stderr,"client close.\n");
        close(fd); //记住close fd       
        delete_event(epollfd,fd,EPOLLIN); //删除监听 
    }
    else {         
        printf("read message is : %s",buf);        
        //修改描述符对应的事件，由读改为写         
        modify_event(epollfd,fd,EPOLLOUT);     
    }
}

// handle wrting
void do_write(int epollfd,int fd,char *buf) {     
    int nwrite;     
    nwrite = write(fd,buf,strlen(buf));     
    if (nwrite == -1){         
        perror("write error:");        
        close(fd);   //记住close fd       
        delete_event(epollfd, fd, EPOLLOUT);  //删除监听    
    } else {
        modify_event(epollfd, fd, EPOLLIN); 
    }    
    memset(buf,0,MAXSIZE); 
}

void delete_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

void modify_event(int epollfd,int fd,int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}

// bind socket
int socket_bind(const char *ip, short port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv;
    serv.sin_port = htons(port);
    serv.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serv.sin_addr);
    memset(&serv.sin_zero, 0, sizeof(serv.sin_zero));

    if (bind(fd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
        perror("error binding!\n");
        exit(EXIT_FAILURE);
    }
    if (listen(fd, 1000) == -1) {
        perror("error listening!\n");
        exit(EXIT_FAILURE);
    }

    return fd;
}
