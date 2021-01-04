/*
 * Пул потоков с ожиданием результата.
 * Кашицын Денис, 2020
 * kamikadze1024@yandex.ru
 */
#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <memory>

namespace ThPool {

//обертка над методами к исполнению
class FunctionWrapper {
private:
    //базовый класс обертки
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


//класс пула потоков
class ThreadPool {
public:
    ThreadPool();
};

}

#endif // THREADPOOL_HPP
