#ifndef INTERNAL_DEQUE_H
#define INTERNAL_DEQUE_H


namespace tpie{
template <typename T>
class internal_deque: public linear_memory_base<internal_deque<T> >
{
    array<T> elements;
    size_t first, last;
    public:
        internal_deque(size_t size = 0) : first(size / 2 + 1), last(size / 2) { elements.resize(size); }

        inline int length(){
            return last - first + 1;
        }

        inline const T & index(int i){
            return elements[first + i];
        }

        inline void pushFront(T val){
            first--;
            elements[first] = val;
        }

        inline void pushBack(T val){
            last++;
            elements[last] = val;
        }

        inline const T & popFront(){
            return elements[first++];
        }

        inline const T & popBack(){
            return elements[last--];
        }

        inline const T & peekFront(){
            return elements[first];
        }

        inline const T & peekBack(){
            return elements[last];
        }

        inline void undo(){

        }

        inline void split(int index, bool front){

        }

        inline size_t size(){
            return last - first + 1;
        }

        inline bool empty() {
            return size() == 0;
        }

    protected:

    private:
};

}

#endif // INTERNAL_DEQUE_H
