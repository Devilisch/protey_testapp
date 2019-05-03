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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define OFFLINE_STATUS 0
#define ONLINE_STATUS 1
#define OFF 0
#define ON 1



//Интерфейс работы сервера
//+Функция включения сервера (с выводом ошибок)
//Функция ожидания сообщений от клиента
//Функция обработки сообщения от клиента, а именно создания массива чисел, найденных в сообщении (использовать vector)
//Функция нахождения суммы вектора цифр
//Функция сортировки чисел в порядке убывания (возможно стоит воспользоваться встроенной функцией sort или qsort. Проверить возможно ли это сделать в рамках данного тестового задания)
//Функция поиска максимального значения (возможно стоит воспользоваться встроенными функциями)
//Функция поиска минимального значения (возможно стоит воспользоваться встроенными функциями)
//Функция отправки сообщений клиенту (возможно стоит реализовать через отправку 2 сообщений: эхо-сообщение и сообщение со статистикой дополнительного задания)
//+Функция отключения сервера
class socket_connection {
public:
	//Настройки для TCP
	//Стоит защитить данные переменные
	int socket_domain = PF_INET;
	int socket_type = SOCK_STREAM;
	int socket_protocol = IPPROTO_TCP;
	int socket_port = 1234; //Выбрать правильный порт для создания соединения
	int socket_info = 0;
	int connection_info = 0;
	int connection_status = OFFLINE_STATUS;
	string socket_type_string = "TCP";
	string connection_ip = "127.0.0.1"; //Может быть ошибка
	struct sockaddr_in socket_info_struct;
	//---

	void set_TCP() { socket_type = SOCK_STREAM; socket_protocol = IPPROTO_TCP; string socket_type_string = "TCP";}
	void set_UDP() { socket_type = SOCK_DGRAM; socket_protocol = IPPROTO_UDP; string socket_type_string = "UDP";}
	void set_port( int port ) { socket_port = port; }; //Добавить ограничение и сообщение об ошибке

	void create_connection() {
		socket_info = socket( socket_domain, socket_type, socket_protocol );
		if( socket_info == -1 ) printf( "ERROR: Socket creation error.\n" );
		else {
			printf( "<Server>: Create %s socket.", socket_type_string );
			memset( &socket_info_struct, 0, sizeof( socket_info_struct ) );
			socket_info_struct.sin_family = socket_domain;
			socket_info_struct.sin_port = htons(socket_port);
			socket_info_struct.sin_addr.s_addr = htonl(INADDR_ANY); //Возможно стоит сменить, подумать пересмотрев лекцию
			if( bind( socket_info, (struct sockaddr*) &socket_info_struct, sizeof(socket_info_struct) ) == -1 ) {
				printf( "ERROR: Binding function error.\n" );
				close( socket_info );
			} else if ( listen( socket_info, 10 ) == -1 ) { //Возможно стоит изменить второй параметр
				printf( "ERROR: Listening function error.\n" );
				close( socket_info );
			} else for(;;) { 
				connection_info = accept( socket_info, 0, 0 );
				if( connection_info < 0 ) {
					printf( "ERROR: Connection error.\n" );
					close( socket_info );
				}
			}

	}

	void send_message(string message) { //Может быть ошибка
		//Отправка сообщения и приём ответа от сервера
		//Добавить ограничения на размер сообщения и сделать вывод ошибок
	}

	void vector_sum() {
		printf("- sum: %i\n", sum);
		//Отправляем то же самое клиенту
	}

	void vector_sort() {
		printf("- sort: ");
		for(auto i : vec) {
			printf("%i", vec);
		}
		printf("\n");
		//Отправляем то же самое клиенту
	}

	void vector_max() {
		printf("- max: %i\n", max);
		//Отправляем то же самое клиенту
	}

	void vector_min() {
		printf("- min: %i\n", min);
		//Отправляем то же самое клиенту
	}

	void shutdown_connection() {
		shutdown( socket_info, SHUT_RDWR)
		close( socket_info );
		printf( "<Server>: Shutdown connection.\n" );
		connection_status = OFFLINE_STATUS;
	}
}

//Интерфейс работы с пользователем
//Функция обработки входящих команд (с выводом ошибок при их некорректном вводе)
//Список команд: start_server, shutdown_server