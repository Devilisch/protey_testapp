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
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define OFFLINE_STATUS 0
#define ONLINE_STATUS 1
#define OFF 0
#define ON 1

using namespace std;

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
	int socket_port = 1235;
	int socket_info = 0;
	int connection_info = 0;
	int connection_status = OFFLINE_STATUS;
	in_addr_t connection_ip = INADDR_LOOPBACK;
	int char_max_size = 100;
	int buffer_size = 100; //Test
	char buffer_test[4] = "abc";
	char * buffer_test1 = new char[buffer_size]; 
	char * buffer = new char[buffer_size];
	struct sockaddr_in socket_info_struct;
	//---

	void set_TCP() { socket_type = SOCK_STREAM; socket_protocol = IPPROTO_TCP; printf("<Client>: Using protocol - TCP.");}
	void set_UDP() { socket_type = SOCK_DGRAM; socket_protocol = IPPROTO_UDP; printf("<Client>: Using protocol - UDP.");}
	void set_port( int port ) { socket_port = port; printf("<Client>: Using port - %i", socket_port); }; //Добавить ограничение и сообщение об ошибке

	void create_connection() {
		buffer_test1 = (char*)"Hello world! 132";
		socket_info = socket( socket_domain, socket_type, socket_protocol );
		if( socket_info == -1 ) printf( "ERROR: Socket creation error.\n" );
		else {
			socket_info_struct.sin_family = socket_domain;
			socket_info_struct.sin_port = htons(socket_port);
			socket_info_struct.sin_addr.s_addr = htonl(connection_ip);

			if( connect( socket_info, (struct sockaddr*)&socket_info_struct, sizeof(socket_info_struct) ) == -1 ) {
				printf( "ERROR: Connection error.\n" );
				close( socket_info );
			} else {
				printf( "<Client>: Connect to the server.\n" );
				connection_status = ONLINE_STATUS;
				
				recv(socket_info, buffer, buffer_size, MSG_NOSIGNAL);
				printf("%s", buffer);
			}
		}

	}

	void shutdown_connection() {
		shutdown( socket_info, SHUT_RDWR);
		close( socket_info );
		printf( "<Client>: Disconnect from the server.\n" );
		connection_status = OFFLINE_STATUS;
	}

	void send_message(char * message) {
		//Отправка сообщения и приём ответа от сервера
		//Добавить ограничения на размер сообщения и сделать вывод ошибок
		send(socket_info, message, 100, MSG_NOSIGNAL);
		printf("<Client>: Send message \"%s\".\n", message);
		recv(socket_info, buffer, buffer_size, MSG_NOSIGNAL); printf("%s", buffer);
		recv(socket_info, buffer, buffer_size, MSG_NOSIGNAL); printf("%s", buffer);
		recv(socket_info, buffer, buffer_size, MSG_NOSIGNAL); printf("%s", buffer);
		recv(socket_info, buffer, buffer_size, MSG_NOSIGNAL); printf("%s", buffer);
		recv(socket_info, buffer, buffer_size, MSG_NOSIGNAL); printf("%s", buffer);
		shutdown_connection();
	}
};



//Интерфейс работы с пользвателем
//+Функция обработки входящих команд (с выводом ошибок при их некорректном вводе)
//+Список команд: connect, send, disconnect, send_test_message
//-----
//Функция чтения сообщения с экрана и отправки его на сервер (с выводом сообщения об ошибке в случаях, если размер сообщения превышает 64 кб).
class socket_interface : socket_connection {
public:
	int programm_status = ON;
	int port = 1111;
	char * message = new char[buffer_size];
	void new_request(int req) {
		switch(req){
			case 1/*"-connect"*/: create_connection(); break;
			case 2/*"-TCP"*/: set_TCP(); break;
			case 3/*"-UDP"*/: set_UDP(); break;
			case 4/*"-port"*/: printf("<Client>: Enter port number: "); scanf("%i", &port); printf("\n"); set_port(port); break;
			case 5/*"-send"*/: printf("<Client>: Enter your message: "); scanf("%s", message); printf("\n"); send_message(message); break;
			case 6/*"-send_test1_message"*/: send_message(buffer_test1); break;
			case 7/*"-disconnect"*/: shutdown_connection(); break;
			case 8/*"-exit"*/: if(connection_status == ONLINE_STATUS) shutdown_connection(); programm_status = OFF; break;
			default: printf( "Unknown request, try again.\n" );
		}
	}
};



int main(){
	socket_interface connection;
	int request;
	while( connection.programm_status == ON ) {
		printf("<Client>: Number of client commands:\n\t1 - connect to the server;\n\t2 - use TCP connection;\n\t3 - use UDP connection;\n\t4 - edit port number;\n\t5 - send message;\n\t6 - send message from Test 1 ( Hello world! 132);\n\t7 - shutdown server;\n\t8 - exit from programm.\n\tEnter number of command: ");
		scanf("%i", &request);
		connection.new_request(request);
	}
	return 0;
}