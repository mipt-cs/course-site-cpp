Нити исполнения (thread)
########################

:date: 2016-11-30 00:00
:lecture_link: https://youtu.be/RQ3oMjqYuoo

.. default-role:: code
.. contents:: Содержание

Определения
===========
* **Thread** (нить, поток)  —  наименьшая единица обработки, исполнение которой может быть назначено ядром операционной системы. Несколько потоков выполнения могут существовать в рамках одного и того же процесса и совместно использовать ресурсы, такие как память, инструкции процесса (его код) и его контекст (значения переменных, которые они имеют в любой момент времени). В качестве аналогии потоки выполнения процесса можно уподобить нескольким вместе работающим поварам. Все они готовят одно блюдо, читают одну и ту же кулинарную книгу с одним и тем же рецептом и следуют его указаниям, причём не обязательно все они читают на одной и той же странице.
* **Mutex** (мьютекс) —  простейший двоичный семафор, который может находиться в одном из двух состояний — «открыт» и «закрыт». Когда какой-либо поток, принадлежащий процессу, становится владельцем объекта mutex, последний переводится в состояние «закрыт». Если поток освобождает мьютекс, то мьютекс переводится в состояние «открыт». Задача мьютекса — защита объекта от доступа к нему других потоков, отличных от того, который завладел мьютексом. В каждый конкретный момент только один поток может владеть объектом, защищённым мьютексом. Если другому потоку будет нужен доступ к переменной, защищённой мьютексом, то этот поток блокируется до тех пор, пока мьютекс не будет освобождён.
* **Deadlock** (взаимная блокировка) —  ситуация в многозадачной среде, при которой несколько процессов/потоков находятся в состоянии бесконечного ожидания ресурсов, захваченных самими этими процессами/потоками (например нить А захватила ресурс Х и хочет захватить Y, и в тоже время нить B захватила Y и хочет захватить X). 
* **Race condition** (состояние гонки) — ошибка проектирования многопоточной системы или приложения, при которой работа системы или приложения зависит от того, в каком порядке выполняются части кода. Своё название ошибка получила от похожей ошибки проектирования электронных схем.

Введение
========

В C++ 11 появился класс `std::thread`, позволяющий достаточно легко организовывать нити исполнения:

.. code-block:: cpp

	#include <iostream>
	#include <thread>
	#include <string>

	void doSmth(const std::string& first, const std::string& second) {
	    std::cout << first << second;
	}

	int main()
	{
	    std::thread t(doSmth, "Hello, ", "threads!");
	    t.join();
	}


Конструктор `std::thread` принимает первым аргументом функцию исполнения, т.е. функцию, код которой будет исполнен в отдельном потоке. Остальные аргументы — это аргументы исполняемой функции. Количество аргументов ограничено лишь реализацией variadic templates в компиляторе.  Важно помнить, что аргументы будут использованы в другом потоке исполнения, а следовательно нельзя передавать ссылки и указатели на объекты, время жизни которых не больше, чем время жизни потока. Это может обернуться некорректным поведением в лучшем случае и падением в худшем.

Так же `std::thread` всегда копирует аргументы и только потом передаёт их исполняемой функции. Поэтому, даже если ваша функция принимает ссылку в качестве аргумента, это будет не та ссылка, которую вы передали в конструкторе thread. Это будет ссылка на копию в объекте `std::thread`.

Для запуска приведенной выше программы нужно указать компилятору использовать C++ 11 (-std=c++11) и линковать код с многопоточной версией стандартной библиотеки (-pthread или -lpthread):

.. code-block:: text

	g++ -std=c++11 -pthread ./hello.cpp

Функция начинает свое исполнения сразу по окончании работы конструктора `std::thread`. Завершение потока происходит по завершении работы исполняемой функции. После того, как объект `std::thread`  создан возможны три варианта развития событий:

#. Пользователь выполнил `thread.join()`. Это означает, что поток исполнения, который вызвал `join`, будет ожидать завершения исполнения cозданного потока. Блокирует вызывающий поток.
#. Пользователь выполнил `thread.detach()`. Это означает, что пользователя не интересует судьба созданного потока и главный поток исполнения может завершится до того как будет завершён созданный. Не блокирует вызывающий поток.
#. Ни один из вышеупомянутых методов не был вызван. Это приведёт к вызову `std::termination` в деструкторе объекта thread.

Вряд ли 3-й случай является приемлемым, поэтому всегда нужно вызывать либо `join`, либо `detach` до того, как будет вызван деструктор объекта `std::thread`. При этом использование `join` предпочтительнее, так как `detach` используется тогда, когда это действительно нужно или оправдано, в силу того, что представляет меньше контроля за исполнением.

Идентификация потока
--------------------

Бывают моменты, когда необходимо узнать, какая нить в данный момент исполняет код. Для решения этой задачи стандарт предлагает решение в виде уникального идентификатора `id`, который ассоциирован с каждым потоком исполнения. После завершения нити исполнения стандарт разрешает использовать `id` снова. Это стоит учитывать, при завязывании какой бы то ни было логики на `id` потока. Этот `id` может быть получен из объекта `std::thread` с помощью метода `get_id`. Для осуществления операций с текущей нитью (в частности, для определения `id` текущей нити) существует пространство имён `std::this_thread`. Дополним предыдущий пример:


.. code-block:: cpp

	#include <iostream>
	#include <thread>
	#include <string>

	void doSmth(const std::string& first, const std::string& second) {
	    std::cout << std::this_thread::get_id() << " : " << first << second << std::endl;
	}

	int main()
	{
	    std::thread t(doSmth, "Hello, ", "threads!");
	    t.join();
	    std::cout << std::this_thread::get_id() << " : Main thread "  << std::endl;
	}

Блокировки (mutex)
------------------

С++ 11 предоставляет 3 типа операций над базовыми мьютексами:

* `lock` – если мьютекс не принадлежит никакому потоку, тогда поток, вызвавший `lock`, становится его обладателем. Если же некий поток уже владеет мьютексом, то текущий поток(который пытается овладеть им) блокируется до тех пор, пока мьютекс не будет освобожден.
* `try_lock` - если мьютекс не принадлежит никакому потоку, тогда поток, вызвавший `try_lock`, становится его обладателем и метод возвращает `true`. В противном случае возвращает false. `try_lock` не блокирует текущий поток.
* `unlock` – освобождает ранее захваченный мьютекс.
  
Также существуют две дополнительные операции для временны́х (timed) мьютексов:

* `try_lock_for` – расширенная версия `try_lock`, которая позволяет задать продолжительность ожидания до того, как стоит прекратить попытку захватить мьютексом. Т.е. возвращает `true` в том случае, если удалось овладеть мьютексом в заданный промежуток времени. В противном случае возвращает `false`. Принимает `std::chrono::duration` в качестве аргумента.
* `try_lock_until` – та же, что предыдущая, но принимает `std::chrono::time_point` в качестве аргумента.

Всего существует четыре типа мьютексов:

* `std::mutex` – базовый мьютекс, которым может владеть один поток в единицу времени. При попытке повторного овладения мьютексом потоком, уже владеющим им, произойдёт дедлок (или будет брошено исключение с кодом ошибки  `resource_deadlock_would_occur`). 
* `std::recursive_mutex` – обладает теми же свойствами, что и `std::mutex`, но позволяет рекурсивное овладение мьютексом, то есть многократный вызов метода `lock` в потоке, который владеет мьютексом. При этом, метод `unlock` должен быть вызван не меньшее количество раз, чем был вызван `lock`. В противном случае произойдёт дедлок, т.к. поток никогда не освободит мьютекс и остальные потоки будут находиться в вечном ожидании.
* `std::timed_mutex` – обладая свойствами `std::mutex`, также обладает дополнительными методами позволяющими осуществить блокировку на время.
* `std::recursive_timed_mutex` – рекуррентная версия `std::timed_mutex`.

Рассмотрим простейший пример:

.. code-block:: cpp

	#include <iostream>
	#include <chrono>
	#include <thread>
	#include <mutex>
	 
	std::mutex lock;
	 
	void threadFunction()
	{
	     lock.lock();
	 
	     std::cout << "entered thread " << std::this_thread::get_id() << std::endl;
	     std::this_thread::sleep_for(std::chrono::seconds(rand()%10));
	     std::cout << "leaving thread " << std::this_thread::get_id() << std::endl;
	 
	     lock.unlock();
	}
	 
	int main()
	{
	     std::thread t1(threadFunction);
	     std::thread t2(threadFunction);
	     std::thread t3(threadFunction);
	     t1.join();
	     t2.join();
	     t3.join();
	     return 0;
	}

После запуска программа выводит примерно следующее:

.. code-block:: text

	entered thread 3057216320
	leaving thread 3057216320
	entered thread 3065609024
	leaving thread 3065609024
	entered thread 3074001728
	leaving thread 3074001728


Взаимная блокировка (deadlock)
------------------------------

Взаимная блокировка может быть продемонстрирована на следующем примере:

.. code-block:: cpp

	#include <iostream>
	#include <mutex>
	#include <thread>
	#include <chrono>
	 
	std::mutex lock1;
	std::mutex lock2;
	 
	void threadFunction1()
	{
		std::cout <<"Thread 1 :: enter function" << std::endl;
		lock1.lock();
		std::cout <<"Thread 1 :: lock1.lock()" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		lock2.lock();
		std::cout <<"Thread 1 :: lock2.lock()" << std::endl;
		lock2.unlock();
		std::cout <<"Thread 1 :: lock2.unlock()" << std::endl;
		lock1.unlock();
		std::cout <<"Thread 1 :: lock1.unlock()" << std::endl;
	}
	 
	void threadFunction2()
	{
		std::cout <<"Thread 2 :: enter function" << std::endl;
		lock2.lock();
		std::cout <<"Thread 2 :: lock2.lock()" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		lock1.lock();
		std::cout <<"Thread 2 :: lock1.lock()" << std::endl;
		lock1.unlock();
		std::cout <<"Thread 2 :: lock1.unlock()" << std::endl;
		lock2.unlock();
		std::cout <<"Thread 2 :: lock2.unlock()" << std::endl;
	}

	int main()
	{
	     std::thread t1(threadFunction1);
	     std::thread t2(threadFunction2);
	     t1.join();
	     t2.join();
	     return 0;
	}


Задание 1
---------

* Скомпилируйте и запустите программу. Обьясните причину зависания.
* Закомментируйте строчки, содержащие вызов sleep_for и запустите программу снова несколько раз. Решена ли проблема дедлоков?


Состояние гонки (race condition)
--------------------------------

Скомпилируйте и запустите следующую программу:

.. code-block:: cpp

	#include <iostream>
	#include <thread>
	 
	int x;
	 
	void threadFunction1()
	{
		for( int i = 0; i < 1000003; ++i ) {
			x++;
		}
	}
	 
	void threadFunction2()
	{
		for( int i = 0; i < 1000000; ++i ) {
			if ( (x%2) == 0 ) {
				std::cout << "x = " << x << std::endl;
			}
		}

	}

	int main()
	{
	     std::thread t1(threadFunction1);
	     std::thread t2(threadFunction2);
	     t1.join();
	     t2.join();
	     return 0;
	}


Проанализируйте данные, которая выводит программа в процессе работе. 

Задание 2
---------

* Используя мьютекс, модифицируйте программу так, чтобы она гарантированно выводила только четные числа.
* (Задание повышенной сложности). Модифицируйте программу так, чтобы она гарантированно выводила только четные числа без использования мьютексов.
