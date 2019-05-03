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

//Интерфэйс работы с сокетами / соединением
//+Функция создания соединения с сервером по заданным настройкам (с выводом ошибок при некорректно введённых данных)
//Функция отправки сообщений (с выводом ошибки при отсутствии соединения с сервером)
//Дополнительно: функция сжатия / архивирования сообщений (ПОМЕТКА: посмотреть и выбрать оптимальный метод сжатия для данного случая)
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
			printf( "<Client>: Create %s socket.", socket_type_string );
			memset( &socket_info_struct, 0, sizeof( socket_info_struct ) );
			socket_info_struct.sin_family = socket_domain;
			socket_info_struct.sin_port = htons(socket_port);
			connection_info = inet_pton( socket_domain, connection_ip, &socket_info_struct.sin_addr );
			if( connection_info < 0 ) {
				printf( "ERROR: Uncorrect first parameter.\n" );
				close( socket_info );
			} else if ( connection_info == 0 ) {
				printf( "ERROR: Uncorrect second parameter.\n" );
				close( socket_info );
			} else if( connect( socket_info, (struct sockaddr*)&socket_info_struct, sizeof(socket_info_struct) ) == -1 ) {
				printf( "ERROR: Connection error.\n" );
				close( socket_info );
			} else {
				printf( "<Client>: Connect to the server.\n" );
				connection_status = ONLINE_STATUS;
			}
		}

	}

	void send_message(string message) { //Может быть ошибка
		//Отправка сообщения и приём ответа от сервера
		//Добавить ограничения на размер сообщения и сделать вывод ошибок
	}

	void shutdown_connection() {
		shutdown( socket_info, SHUT_RDWR)
		close( socket_info );
		printf( "<Client>: Disconnect from the server.\n" );
		connection_status = OFFLINE_STATUS;
	}
}



//Интерфейс работы с пользвателем
//+Функция обработки входящих команд (с выводом ошибок при их некорректном вводе)
//+Список команд: connect, send, disconnect, send_test_message, send_big_message
//-----
//Функция чтения сообщения с экрана и отправки его на сервер (с выводом сообщения об ошибке в случаях, если размер сообщения превышает 64 кб). 
//Дополнительно: возможно стоит реализовать отправку больших сообщений с помощью их разделения на несколько.
class socket_interface : socket_connection {
public:
	int programm_status = ON;
	void new_request(string req) {
		switch(req){
			case "-connect": create_connection(); break;
			case "-TCP": set_TCP(); break;
			case "-UDP": set_UDP(); break;
			case "-port": /*set_port(port);*/ break;
			case "-send": /*send_message();*/ break;
			case "-send_test1_message": send_message("Hello world! 132"); break;
			case "-disconnect": shutdown_connection(); break;
			case "-exit": programm_status = OFF; break;
			default: printf( "Unknown request, try again.\n" );
		}
	}
}



int main(){
	socket_interface connection;
	string request;
	while( connection.programm_status == ON ) {
		scanf("%s", request);
		connection.new_request(request);
	}
	return 0;
}