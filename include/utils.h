#ifndef __UTILS_H__
#define __UTILS_H__

#include <FreeRTOS.h>
#include <functional>
#include <semphr.h>
#include <string.h>

#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))

template <typename T> class ArrayList {
  public:
    ArrayList() : m_data(nullptr), m_count(0), m_data_len(0) {}

    ArrayList(const ArrayList &other)
        : ArrayList(other.m_data, other.m_count) {}

    ArrayList(size_t max_size) {
        m_data = (T *)calloc(max_size, sizeof(T));
        m_data_len = max_size;
        m_count = max_size;
    }

    ArrayList(const T *data, size_t data_len) {
        m_data = (T *)malloc(sizeof(T) * data_len);
        m_data_len = data_len;
        m_count = data_len;
        memcpy((void *)m_data, (void *)data, sizeof(T) * m_data_len);
    }

    ~ArrayList() {
        if (m_data != nullptr) {
            free(m_data);
        }
    }

    void resize(size_t data_len) {
        if (m_data_len < data_len) {
            T *old_data = m_data;
            m_data = (T *)malloc(sizeof(T) * data_len);
            m_data_len = data_len;
            if (old_data != nullptr) {
                if (m_count > 0) {
                    memcpy(m_data, old_data, sizeof(T) * m_count);
                }
                free(old_data);
            }
        }
    }

    bool add(const T &data) {
        if (m_count >= m_data_len) {
            resize(2 * m_count + 1);
        }
        if (m_count < m_data_len) {
            m_data[m_count] = data;
            m_count++;
            return true;
        }
        return false;
    }

    bool remove(const T &data) {
        for (size_t i = 0; i < m_count; i++) {
            if (m_data[i] == data) {
                m_data[i] = m_data[m_count - 1];
                m_count--;
            }
        }
        return false;
    }

    T &operator[](size_t index) { return m_data[index]; }

    T operator[](size_t index) const { return m_data[index]; }

    T *data() { return m_data; }

    const T *data() const { return m_data; }

    size_t count() const { return m_count; }

    void foreach (std::function<T &> op) {
        for (int i = 0; i < m_count; i++) {
            op(m_data[i]);
        }
    }

  private:
    T *m_data;
    size_t m_count;
    size_t m_data_len;
};

class Mutex {
  public:
    Mutex() : m_mutex(xSemaphoreCreateMutex()) {}

    void Lock() { xSemaphoreTake(m_mutex, portMAX_DELAY); }

    bool TryLock(uint16_t ticks) {
        return xSemaphoreTake(m_mutex, ticks) == pdTRUE;
    }

    void Unlock() { xSemaphoreGive(m_mutex); }

  private:
    SemaphoreHandle_t m_mutex;
};

class LockGuard {
  public:
    LockGuard(Mutex &mutex) : m_mutex_ptr(mutex) { m_mutex_ptr.Lock(); }
    ~LockGuard() { m_mutex_ptr.Unlock(); }

  private:
    Mutex &m_mutex_ptr;
};

void hex_dump(const void *data, size_t data_len);

class ITaskManager {
  public:
    ITaskManager(uint32_t refresh_rate = 60, BaseType_t core = 0);

    virtual void start(void);
    virtual void stop(void);

  protected:
    static void ITaskManagerTask(void *ctx);

    virtual void setup(void) = 0;
    virtual void update(void) = 0;
    virtual void cleanup(void) = 0;

  protected:
    uint32_t m_refresh_rate;
    BaseType_t m_core;
    TaskHandle_t m_task_handler;
    bool m_stop;
};

#endif
