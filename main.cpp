#include <unistd.h>

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

int own_strlen(const char *str)
{
    int size = 0;
    while (str[size]) {
        ++size;
    }

    return size;
}

void own_memcpy(void *dest, const void *src, int count)
{
    char *d = static_cast<char*>(dest);
    const char *s = static_cast<const char*>(src);

    for (int i = 0; i < count; i) {
        d[i] = s[i];
    }
}

template<typename T>
class Array
{
public:
    Array() :
        m_size(0)
    {
    }

    void append(T n)
    {
        m_buffer[m_size] = n;
        ++m_size;
    }

    T at(int index) { return m_buffer[index]; }
    T &operator[](int index) { return m_buffer[index]; }

    int size() const { return m_size; }
    int count() const { return m_size; }

    void clear() { m_size = 0; }

private:
    int m_size;
    T m_buffer[20];
};

Array<int> splitter(long long number)
{
    Array<int> array;

    if (number == 0) {
        array.append(0);
        return array;
    }
    while (number) {
        long reduced = number / 10;
        array.append(number - reduced * 10);
        number = reduced;
    }

    for (int i = 0; i < array.size(); ++i) {
        int v = array[i];
        array[i] = array[array.size() - i - 1];
        array[array.size() - i - 1] = v;
    }

    return array;
}

class TextWriter
{
public:
    TextWriter(int handle) :
        m_handle(handle)
    {
    }

    TextWriter &operator<<(char c)
    {
        write(m_handle, &c, 1);
        return *this;
    }

    TextWriter &operator<<(const char *str)
    {
        write(m_handle, str, own_strlen(str));
        return *this;
    }

    TextWriter &operator<<(const int n)
    {
        Array<int> digits = splitter(n);
        for (int i = 0; i < digits.count(); ++i) {
            char c = digits.at(i) + '0';
            *this << c;
        }
        return *this;
    }

private:
    int m_handle;
};

TextWriter cout(STDOUT_FILENO);
const char endl = '\n';

int main(int argc, char *argv[])
{
    cout << "Hello, world!" << endl;
    cout << 50 << endl;

    return 0;
}
