#ifndef FUNCTOR_H
#define FUNCTOR_H

class server;

class functor {
private:
	server *serv;
public:
	functor(server *s);
	void operator()(int fd);
};

#endif
