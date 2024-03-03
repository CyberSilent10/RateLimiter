#include <vector>

using std::vector;

template<typename T>
class CircularArray {
      int m_sz, m_start, m_end;
      vector<T> m_vec;
      public:
	CircularArray(int sz);
	void push_back(T data);
	T getStart();
};

template<typename T>
CircularArray<T>::CircularArray(int sz) : m_sz(sz), m_start(0), m_end(0)
{
	if(m_sz < 0) {
		m_sz = 1;
	}
	m_vec = vector<T>(m_sz);
}

template<typename T>
void CircularArray<T>::push_back(T data) {
     if(m_end == m_start) {
     	    // Circular array is full, oldest data will be overwritten.
     	     m_start = (m_start+1) % m_sz;
     }
     m_vec[m_end] = data;
     m_end = (m_end + 1) % m_sz;
}

template<typename T>
T CircularArray<T>::getStart() {
	return m_vec[m_start];
}