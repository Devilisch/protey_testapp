//protey_testapp_client
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


//Интерфэйс работы с сокетами / соединением
//Функция создания соединения с сервером по заданным настройкам (с выводом ошибок при некорректно введённых данных)
//Функция отправки сообщений (с выводом ошибки при отсутствии соединения с сервером)
//Дополнительно: функция сжатия / архивирования сообщений (ПОМЕТКА: посмотреть и выбрать оптимальный метод сжатия для данного случая)
class socket_connection {
public:
	//Настройки для TCP
	int socket_domain = PF_INET;
	int socket_type = SOCK_STREAM;
	int socket_protocol = IPPROTO_TCP;

	void set_TCP() { socket_type = SOCK_STREAM; socket_protocol = IPPROTO_TCP; }
	void set_UDP() { socket_type = SOCK_DGRAM; socket_protocol = IPPROTO_UDP; }
}

//Интерфейс работы с пользвателем
//Функция обработки входящих команд (с выводом ошибок при их некорректном вводе)
//Список команд: connect, send, disconnect, send_test_message, send_big_message
//-----
//Функция чтения сообщения с экрана и отправки его на сервер (с выводом сообщения об ошибке в случаях, если размер сообщения превышает 64 кб). 
//Дополнительно: возможно стоит реализовать отправку больших сообщений с помощью их разделения на несколько.

