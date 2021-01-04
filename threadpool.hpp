/*
 * Пул потоков с ожиданием результата.
 * Кашицын Денис, 2020
 * kamikadze1024@yandex.ru
 */
#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <memory>
#include <atomic>
#include <thread>
#include <future>
#include "threadsafequeue.hpp"

namespace ThPool {

//обертка над методами к исполнению
class FunctionWrapper {
private:
    //базовый класс структуры, которую вызывает обертка
    struct ImplBase {
        //определяю в базовом классе метод вызова функции
        virtual void call() = 0;
        virtual ~ImplBase() {}
    };

    /*
     * Шаблон - наследник базового класса, в котором есть реализация
     * и объект - функция, которую я буду вызывать
     */
    template<typename F>
    struct ImplType: ImplBase {
        F m_f;
        //перемещающий конструктор
        ImplType(F&& f): m_f(std::move(f)) {}
        void call() { m_f(); }
    };

    //уникальный указатель типа базового класса
    std::unique_ptr<ImplBase> m_impl;

public:
    /*
     * Конструктор обертки (перемещающий конструктор передаваемой ф-ции),
     * в который я передаю универсальную
     * ссылку на метод, который сохраняю в обертке.
     * Во время конструирования, я создаю новый объект
     * структуры, реализующей вызов функции, перемещаю в него
     * объект функции, и сохраняю этот объект структуры
     * в уникальном указателе m_impl.
     */
    template<typename F>
    FunctionWrapper(F&& f): m_impl(new ImplType<F>(std::move(f))) {}

    /*
     * Оператор вызова функции без параметров.
     * (Во вторых скобках могут быть параметры)
     * Функтор, функциональный объект.
     * Пример:
     * FunctionWrapper task
     * task();//вызвать функцию
     */
    void operator()() {
        m_impl->call();
    }

    //автогенерируемый конструктор
    FunctionWrapper() = default;

    /*
     * Перемещающий конструктор FunctionWrapper.
     * Перемещает уникальный указатель на структуру,
     * содержащую функцию для исполнения.
     */
    FunctionWrapper(FunctionWrapper &&other):
        m_impl(std::move(other.m_impl)) {}

    //не копирую, а перемещаю
    FunctionWrapper& operator=(FunctionWrapper &&other) {
        m_impl = std::move(other.m_impl);
        return *this;
    }

    //запрещено все, связанное с копированием
    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
};

/*
 * Класс, который поможет мне дождаться окончания работы
 * всех потоков в случае исключения
 */
class JoinThreads final {
private:
    //ссылка на вектор потоков
    std::vector<std::thread> &m_threads;

public:
    //запрещаю неявное преобразование типов
    explicit JoinThreads(std::vector<std::thread> &thds)
        : m_threads(thds) {}

    ~JoinThreads() {
        /*
         * Если есть потоки, к которым еще не присоединились
         * Присоединиться. (Дождаться результата)
         */
        for(unsigned int i = 0; i < m_threads.size(); i++) {
            if(m_threads[i].joinable()) {
                m_threads[i].join();
            }
        }
    }
};

//класс пула потоков
class ThreadPool final {
private:
    //флаг работы потока
    std::atomic<bool>                            m_thdFlag;

    //потокобезопасная очередь задач
    Containers::ThreadsafeQueue<FunctionWrapper> m_tasksQueue;

    //вектор потоков
    std::vector<std::thread>                     m_threads;

    //"дожидальщик" потоков
    JoinThreads                                  m_joiner;

    //рабочий метод пула потоков
    void workerThread() {
        while(m_thdFlag.load()) {
            FunctionWrapper task;
            if(m_tasksQueue.try_pop(task)) {
                //если есть задачи - исполни
                task();
            } else {
                /*
                 * Ждать, пока не появится задача.
                 * В зависимости от реализации системы,
                 * лучше будет использовать
                 * sleep_for.
                 */
                std::this_thread::yield();
            }
        }
    }
public:
    ThreadPool(): m_thdFlag(true), m_joiner(m_threads) {
        /*
         * Узнаю количество потоков, которое может
         * поддержать оборудование
         */
        int thdCnt = std::thread::hardware_concurrency();

        /*
         * Создаю потоки, которые выполняют метод workerThread
         * этого пула потоков.
         * И добавляю их в пул потоков
         */
        try {
            for(unsigned int i = 0; i < thdCnt; i++) {
                m_threads.push_back(
                            std::thread(&ThreadPool::workerThread, this));
            }
        }  catch (...) {
            m_thdFlag.store(false);
            throw;
        }
    }

    //останавливаю поток  при разрушении объекта
    ~ThreadPool() {
        m_thdFlag.store(false);
    }

    /*
     * Обертываю функцию или допускающий вызов объект
     * и помещаю его в m_tasksQueue.
     * Функция вернет std::future<> с возвращенным задачей значением
     * и позволит ждать ее завершения.
     */
    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
        submit(FunctionType f) {
        /* Псевдоним типа результата, который вернет задача
         * Иными словами, это тип возвращаемого значения, который
         * вернет задача при вызове задача()
         */
        using resultType = typename std::result_of<FunctionType()>::type;

        /*
         * Запакованная задача, только в отличие от
         * std::function, оттуда можно будет взять
         * std::future<> типа возвращаемого значения
         * выполненной задачи.
         */
        std::packaged_task<resultType()> task(std::move(f));

        /*
         * Получаю объект std::future<>, он-же будущий результат,
         * типа возвращаемого значения
         * выполненной задачи. Круглых скобок уже нет,
         * т.к. задача уже не должна вызывать operator()
         */
        std::future<resultType> res(task.get_future());

        m_tasksQueue.push(std::move(task));

        //возвращаю будущий результат потребителю
        return res;
    }

    //узнать количество потоков, которое поддерживается аппаратурой
    unsigned int getDeviceSupportedThdCnt() {
        return m_threads.size();
    }
};

}

#endif // THREADPOOL_HPP
