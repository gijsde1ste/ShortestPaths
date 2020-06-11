#ifndef INTERNAL_DEQUE_H
#define INTERNAL_DEQUE_H


namespace tpie{
template <typename T>
class internal_deque: public linear_memory_base<internal_deque<T> >
{
    array<T> elements;
    size_t first, last;
    public:
        internal_deque(size_t size) : first(size / 2 + 1), last(size / 2) { elements.resize(size); }

        inline int length(){
            return last - first + 1;
        }

        inline const T & index(int i){
            return elements[first + i];
        }

        inline void push(T val, bool front){
            if (front){
                first--;
                elements[first] = val;
            } else {
                last++;
                elements[last] = val;
            }
        }

        inline const T & pop(bool front){
            if (front){
                return elements[first++];
            } else {
                return elements[last--];
            }
        }

        inline void undo(){

        }

        inline void split(int index, bool front){

        }

    protected:

    private:
};

}

#endif // INTERNAL_DEQUE_H
