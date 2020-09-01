#ifndef INTERNAL_DEQUE_H
#define INTERNAL_DEQUE_H

#include <definition.h>

namespace tpie{
template <typename T>
class internal_deque: public linear_memory_base<internal_deque<T> >
{
    array<T> elements;
    public:
        size_t first, last;
        internal_deque(size_t size = 0) : first(size / 2 + 1), last(size / 2) { elements.resize(size); }

        inline int length(){
            return last - first + 1;
        }

        inline const T & index(int i){
            return elements[i];
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

        inline void undoAdd(bool front, T x){
            if (front){
                elements[first] = x;
                first++;
            } else {
                elements[last] = x;
                last--;
            }
        }

        inline void undoSplit(bool front, int i){
            if (front){
                first = i;
            } else{
                last = i;
            }
        }

        inline void split(bool front, int i){
            if (front){
                first = first + i;
            } else {
                last = last - i;
            }
        }

        inline size_t size(){
            return last - first + 1;
        }

        inline bool empty() {
            return size() == 0;
        }

        inline void print(){
            std::cout << "------" << std::endl;
            for (int i = first; i <= last; i++){
                std::cout << elements[i].point.x << " " << elements[i].point.y << std::endl;
            }

            std::cout << "------" << std::endl;
        }

    protected:

    private:
};

}

#endif // INTERNAL_DEQUE_H
