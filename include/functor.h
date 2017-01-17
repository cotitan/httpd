#ifndef FUNCTOR_H
#define FUNCTOR_H

class server;

class functor {
private:
	server *serv;
public:
	void operator()(int fd);
	explicit functor(server *s);
};

#endif
