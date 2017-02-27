#include <memory>
#include <memory>  
#include <mutex>  
#include <condition_variable>

template<typename T>
class threadsafe_queue {
private:
    mutable std::mutex mut;   
    std::queue<T> data_queue;  
    std::condition_variable data_cond;
public:
	threadsafe_queue();
	
    threadsafe_queue(const threadsafe_queue&);
    
    // 禁止赋值操作是为了简化 
	threadsafe_queue& operator=(const threadsafe_queue&) = delete;
	
    void push(T new_value);
    
    // 尝试删除队首元素，若删除成功则通过value返回队首元素，并返回true;
    // 若队为空，则返回false 
	bool try_pop(T& value);
    
    // 若队非空shared_ptr返回并删除的队首元素;
    // 若队空，则返回的shared_ptr为NULL
	std::shared_ptr<T> try_pop();
    
    // 若队非空，通过value返回队首元素并删除，函数返回true;
    // 若队为空，则通过condition_variable等待有元素入队后再获取闭并删除队首元素
	void wait_and_pop(T& value);
    
    //和前面一样，只不过通过shared_ptr返回队首元素
	std::shared_ptr<T> wait_and_pop();
	
    bool empty() const;
};
