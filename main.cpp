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

    for (int i = 0; i < count; ++i) {
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

    for (int i = 0; i < array.size() / 2; ++i) {
        int v = array[i];
        array[i] = array[array.size() - i - 1];
        array[array.size() - i - 1] = v;
    }

    return array;
}

class String
{
public:
    String(const char *initStr = 0) :
        m_size(0)
    {
        if (initStr) {
            m_size = own_strlen(initStr);
            own_memcpy(m_buffer, initStr, m_size);
        }
        m_buffer[m_size] = 0;
    }

    int size() const { return m_size; }
    bool isEmpty() const { return m_size == 0; }

    const char *constData() const { return m_buffer; }

    void clear()
    {
        m_size = 0;
        m_buffer[m_size] = 0;
    }

    char at(int index) const
    {
        return m_buffer[index];
    }

    char &operator[](int index)
    {
        return m_buffer[index];
    }

    bool operator==(const String &anotherString)
    {
        if (m_size != anotherString.size()) {
            return false;
        }

        for (int i = 0; i < m_size; ++i) {
            if (m_buffer[i] != anotherString.m_buffer[i]) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const String &anotherString)
    {
        return !(*this == anotherString);
    }

    String &operator+=(const char c)
    {
        m_buffer[m_size] = c;
        ++m_size;
        m_buffer[m_size] = 0;
        return *this;
    }

    String &operator+=(const int n)
    {
        Array<int> digits = splitter(n);
        for (int i = 0; i < digits.count(); ++i) {
            m_buffer[m_size] = digits.at(i) + '0';
            ++m_size;
        }
        m_buffer[m_size] = 0;
        return *this;
    }

    String &operator+=(const String &anotherStr)
    {
        own_memcpy(&m_buffer[m_size], anotherStr.m_buffer, anotherStr.m_size);
        m_size += anotherStr.m_size;
        m_buffer[m_size] = 0;
        return *this;
    }

private:
    int m_size;
    char m_buffer[256];
};

class IODevice
{
public:
    virtual void writeData(const void *data, int size) = 0;
};

class TextWriter
{
public:
    TextWriter(IODevice *device) :
        m_device(device)
    {
    }

    TextWriter &operator<<(char c)
    {
        m_device->writeData(&c, 1);
        return *this;
    }

    TextWriter &operator<<(const char *str)
    {
        m_device->writeData(str, own_strlen(str));
        return *this;
    }

    TextWriter &operator<<(const String &str)
    {
        m_device->writeData(str.constData(), str.size());
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
protected:
    IODevice *m_device;

};

class File : public IODevice
{
public:
    File(int handle) :
        m_handle(handle)
    {
    }

protected:
    void writeData(const void *data, int size)
    {
        write(m_handle, data, size);
    }

private:
    int m_handle;
};

class Buffer : public IODevice
{
public:
    Buffer() :
        m_currentPos(0)
    {
        m_buffer[0] = 0;
    }

    void reset()
    {
        m_currentPos = 0;
    }

    String toString() const
    {
        return String(m_buffer);
    }

protected:
    void writeData(const void *data, int size)
    {
        own_memcpy(&m_buffer[m_currentPos], data, size);
        m_currentPos += size;
        m_buffer[m_currentPos] = 0;
    }

private:
    char m_buffer[256];
    int m_currentPos;
};

File coutFile(STDOUT_FILENO);
TextWriter cout(&coutFile);

const char endl = '\n';

int main(int argc, char *argv[])
{
    cout << "Hello, world!" << endl;
    cout << 50 << endl;

    String s1 = "Hello";
    String s2 = "world";

    s1 += " ";
    s1 += s2;

    s1 += '!';

    s1 += endl;

    s1 += 2123;

    cout << s1 << endl;

    cout << "String isEmpty(): ";
    if (s1.isEmpty()) {
        cout << "Fail!" << endl;
    } else {
        cout << "OK." << endl;
    }
    s1.clear();

    cout << "String isEmpty() after clear(): ";
    if (!s1.isEmpty()) {
        cout << "Fail!" << endl;
    } else {
        cout << "OK." << endl;
    }

    Buffer buffer;
    TextWriter bufferWriter(&buffer);
    bufferWriter << "Text";
    bufferWriter << 747;

    String expected = "Text747";

    cout << "Buffer, TextWriter and String !=: ";
    if (buffer.toString() != expected) {
        cout << "Fail!" << endl;
    } else {
        cout << "OK." << endl;
    }

    return 0;
}
