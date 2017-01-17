#ifndef FUNCTOR_H
#define FUNCTOR_H

class server;

class functor {
private:
	server *serv;
public:
	void run(int fd);
	explicit functor(server *s);
}

#endif
