#ifndef DEQUE_H
#define DEQUE_H

#include <tpie/stream.h>
#include <tpie/compressed/stream.h>

namespace tpie {

template <typename T>
class deque
{
public:
    ////////////////////////////////////////////////////////////////////
    /// \brief Initialize anonymous deque.
    ////////////////////////////////////////////////////////////////////
    deque(double blockFactor=1.0)
        : frontStream(blockFactor)
        , backStream(blockFactor)
        , frontBuffer(buffer_size(blockFactor))
        , backBuffer(buffer_size(blockFactor))
        , frontBufferItems(0)
        , backBufferItems(0)
        , frontBufferBegin(0)
        , backBufferBegin(0)
    {
        frontStream.open(static_cast<memory_size_type>(0), access_normal,
                      compression_normal);

        backStream.open(static_cast<memory_size_type>(0), access_normal,
                      compression_normal);

    }

    inline void pushFront(const T & t)
    {
        if (frontBuffer.size() == frontBufferItems) frontEmptyBuffer();
        frontBuffer[frontBufferItems++] = t;
    }

    inline const T & popFront() {

        if (frontBufferItems - frontBufferBegin) return frontBuffer[--frontBufferItems];

        if (!frontEmpty()) return frontStream.read_back();
        else {
            if (backStream.offset() == 0) return backBuffer[backBufferBegin++];
            else{
                split(false);
                return popFront();
            }
        }
    }

	inline const T & peekFront() {
		if (frontBufferItems - frontBufferBegin) return frontBuffer[frontBufferItems-1];
		if (!frontEmpty()){
            frontBuffer[0] = frontStream.read_back();
            frontStream.read();
            return frontBuffer[0];
		} else {
            if (backStream.offset() == 0) return backBuffer[backBufferBegin];
            else {
                split(false);
                return peekFront();
            }
		}
	}

    inline void pushBack(const T & t)
    {
        if (backBuffer.size() == backBufferItems) backEmptyBuffer();
        backBuffer[backBufferItems++] = t;
    }

    inline const T & popBack() {
        if (backBufferItems - backBufferBegin) return backBuffer[--backBufferItems];

        if (!backEmpty()) return backStream.read_back();
        else {
            if (frontStream.offset() == 0) return frontBuffer[frontBufferBegin++];
            else{
                split(true);
                return popBack();
            }
        }
    }

	inline const T & peekBack() {
		if (backBufferItems - backBufferBegin) return backBuffer[backBufferItems-1];
		if (!backEmpty()){
            backBuffer[0] = backStream.read_back();
            backStream.read();
            return backBuffer[0];
		} else {
            if (frontStream.offset() == 0) return frontBuffer[frontBufferBegin];
            else {
                split(true);
                return peekBack();
            }
		}
	}

    stream_size_type size() const {
		return frontSize() + backSize();
    }

	inline bool empty() {
        return size() == 0;
	}

protected:
    /** The streams used to store the items. */
    file_stream<T> frontStream;
    file_stream<T> backStream;

private:
    array<T> frontBuffer, backBuffer;
    size_t frontBufferItems, backBufferItems;
    size_t frontBufferBegin, backBufferBegin;

    inline void frontEmptyBuffer() {
        if (frontBufferItems == 0) return;
        frontStream.truncate(frontStream.get_position());
        frontStream.write(frontBuffer.begin() + frontBufferBegin, frontBuffer.begin() + frontBufferItems);
        frontBufferItems = 0;
        frontBufferBegin = 0;
    }

    inline void backEmptyBuffer() {
        if (backBufferItems == 0) return;
        backStream.truncate(backStream.get_position());
        backStream.write(backBuffer.begin() + backBufferBegin, backBuffer.begin() + backBufferItems);
        backBufferItems = 0;
        backBufferBegin = 0;
    }

    static memory_size_type buffer_size(double blockFactor) {
		return file_stream<T>::block_size(blockFactor)/sizeof(T);
	}

    stream_size_type frontSize() const {
		return frontStream.offset() + frontBufferItems - frontBufferBegin;
    }

	inline bool frontEmpty() {
        return frontSize() == 0;
	}

    stream_size_type backSize() const {
		return backStream.offset() + backBufferItems - backBufferBegin;
    }

	inline bool backEmpty() {
        return backSize() == 0;
	}

	inline void split(bool front) {
        std::cout << "beginSplit" << std::endl;
        file_stream<T> tmpStream;
        tmpStream.open(static_cast<memory_size_type>(0), access_normal,
                      compression_normal);

        if (front){
            backStream.truncate(0);
            stream_size_type offset = frontStream.offset();

            if (frontStream.size() > backBuffer.size()){
                while (frontStream.offset() * 2 > offset) {
                    tmpStream.write(frontStream.read_back());
                }
            }

            while(frontStream.can_read_back()){
                backStream.write(frontStream.read_back());
            }

            frontStream.truncate(0);
            while(tmpStream.can_read_back()){
                frontStream.write(tmpStream.read_back());
            }
        } else {
            frontStream.truncate(0);
            stream_size_type offset = backStream.offset();

            if (backStream.size() > frontBuffer.size()){
                while (backStream.offset() * 2 > offset) {
                    tmpStream.write(backStream.read_back());
                }
            }

            while(backStream.can_read_back()){
                frontStream.write(backStream.read_back());
            }

            backStream.truncate(0);
            while(tmpStream.can_read_back()){
                backStream.write(tmpStream.read_back());
            }
        }

        std::cout << "Endsplit" << std::endl;
	}
};
}

#endif // DEQUE_
