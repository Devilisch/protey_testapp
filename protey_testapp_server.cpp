//protey_testapp_server
//Deadline: 07/05/2019 - 08/05/2019
//Send to: sb.spb90@mail.ru
/*
ТЗ: Необходимо написать 2 программы под Linux, клиент и сервер.
Сервер принимает сообщения от клиента по протоколам TCP и UDP и отправляет их обратно клиенту (работает в режиме эхо).
Клиент читает сообщения произвольной длинны с консоли и отправляет их обратно на сервер. Протокол, по которому отправляет сообщения клиент, указывается в консоли при запуске.
Размер сообщений не фиксированный, но не привышает 64кб.
ВАЖНО! Объём передаваемых по сети данных был минимально необходимым.
Задание должно быть выполнено на языке C++ (использование ООП / STL приветствуется).
Для работы с сетью использовать Berkeley Sockets. Использование Qt или boost для работы с сетью при выполнении задания не допускается.

Дополнение к работе сервера:
- во входном сообщении найти вхождения всех цифр от 0 до 9 и вывести на консоль:
	1. их сумму;
	2. все числа в порядке убывания;
	3. максимальное и минимальное значения.
*/


/*
Тест №1
Создать соединение по заданным параметрам и передать на сервер сообщение.
Параметры:
Сетевой протокол: IPv4.
Тип сокета: потоковый сокет.
Протокол: TCP.
Сообщение: Hello world! 321
Ответ сервера: 	Hello world! 132
				Info about numbers in the message:
				- sum: 6
				- sort: 321
				- max: 3
				- min: 1
*/


//Интерфейс работы сервера
//Функция включения сервера (с выводом ошибок)
//Функция-цикл работы сервера
//Функция ожидания сообщений от клиента
//Функция обработки сообщения от клиента, а именно создания массива чисел, найденных в сообщении (использовать vector)
//Функция нахождения суммы вектора цифр
//Функция сортировки чисел в порядке убывания (возможно стоит воспользоваться встроенной функцией sort или qsort. Проверить возможно ли это сделать в рамках данного тестового задания)
//Функция поиска максимального значения (возможно стоит воспользоваться встроенными функциями)
//Функция поиска минимального значения (возможно стоит воспользоваться встроенными функциями)
//Функция отправки сообщений клиенту (возможно стоит реализовать через отправку 2 сообщений: эхо-сообщение и сообщение со статистикой дополнительного задания)
//Функция отключения сервера

//Интерфейс работы с пользователем
//Функция обработки входящих команд (с выводом ошибок при их некорректном вводе)
//Список команд: start_server, shutdown_server