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
#include <vector>
#include <algorithm>

#define OFFLINE_STATUS 0
#define ONLINE_STATUS 1
#define OFF 0
#define ON 1

using namespace std;
/*
//Учитывается ли последний элемент массива
template<class iterator>
auto max_element(iterator first_element, iterator last_element) {
	auto result = *first_element;
	if( first_element == last_element ) return result;
	else {
		first_element++;
		for( ; first_element != last_element; first_element++ )
			if( result < *first_element )
				result = *first_element;
		return result;
	}

}


//Учитывается ли последний элемент массива
template<class iterator>
auto min_element(iterator first_element, iterator last_element) {
	auto result = *first_element;
	if( first_element == last_element ) return result;
	else {
		first_element++;
		for( ; first_element != last_element; first_element++ )
			if( result > *first_element )
				result = *first_element;
		return result;
	}

}
*/


template <class T> struct greater {
  bool operator() (const T& x, const T& y) const {return x>y;}
  typedef T first_argument_type;
  typedef T second_argument_type;
  typedef bool result_type;
};


//Интерфейс работы сервера
//+Функция включения сервера (с выводом ошибок)
//Функция ожидания сообщений от клиента
//Функция обработки сообщения от клиента, а именно создания массива чисел, найденных в сообщении (использовать vector)
//+Функция нахождения суммы вектора цифр
//Функция сортировки чисел в порядке убывания (возможно стоит воспользоваться встроенной функцией sort или qsort. Проверить возможно ли это сделать в рамках данного тестового задания)
//Функция поиска максимального значения (возможно стоит воспользоваться встроенными функциями)
//Функция поиска минимального значения (возможно стоит воспользоваться встроенными функциями)
//Функция отправки сообщений клиенту (возможно стоит реализовать через отправку 2 сообщений: эхо-сообщение и сообщение со статистикой дополнительного задания)
//+Функция отключения сервера
class socket_connection {
public:
	//Настройки для TCP
	//Стоит защитить данные переменные
	int socket_domain = AF_INET;
	int socket_type = SOCK_STREAM;
	int socket_protocol = IPPROTO_TCP;
	int socket_port = 1235;
	int socket_info = 0;
	int connection_info = 0;
	int connection_status = OFFLINE_STATUS;
	in_addr_t connection_ip = INADDR_ANY;
	int char_max_size = 100;
	int buffer_size = 100; //Test
	char * buffer = new char[buffer_size];
	vector<int> buffer_vector;

	struct sockaddr_in socket_info_struct;
	//---

	void set_TCP() { socket_type = SOCK_STREAM; socket_protocol = IPPROTO_TCP; printf("<Server>: Using protocol - TCP.\n");}
	void set_UDP() { socket_type = SOCK_DGRAM; socket_protocol = IPPROTO_UDP; printf("<Server>: Using protocol - UDP.\n");}
	void set_port( int port ) { socket_port = port; printf("<Server>: Using port - %i.\n", socket_port); };

	void create_connection() {
		socket_info = socket( socket_domain, socket_type, socket_protocol );
		if( socket_info == -1 ) printf( "ERROR: Socket creation error.\n" );
		else {
			socket_info_struct.sin_family = socket_domain;
			socket_info_struct.sin_port = htons(socket_port);
			socket_info_struct.sin_addr.s_addr = htonl(INADDR_ANY); 
			if( bind( socket_info, (struct sockaddr*) &socket_info_struct, sizeof(socket_info_struct) ) == -1 ) {
				printf( "ERROR: Binding function error.\n" );
				close( socket_info );
			} else if ( listen( socket_info, 1 ) == -1 ) { //Возможно стоит изменить второй параметр
				printf( "ERROR: Listening function error.\n" );
				close( socket_info );
			} else /*for(;;)*/ { 
				printf("<Server>: Waiting for accept connection.\n");
				connection_info = accept( socket_info, 0, 0 );
				if( connection_info < 0 ) {
					printf( "ERROR: Connection error.\n" );
					close( socket_info );
				} else { 
					printf("<Server>: Connection accept.\n"); 
					send(connection_info, (char*)"<Server>: Connection accept.\n", buffer_size, MSG_NOSIGNAL);
					recv_message(); 
					shutdown_connection(); 
				}
			}

		}
	}

	void recv_message() { //Может быть ошибка
		//Приём и отправка сообщений клиенту 
		//Добавить ограничения на размер сообщения и сделать вывод ошибок
		//Если будет цикл, то возможно стоит пустить его отдельным процессом, чтобы иметь возможность управлять сервером
		//Может принимать сообщение с длинной отправленного сообщения?
		recv(connection_info, buffer, buffer_size, MSG_NOSIGNAL);
		printf("<Client>: %s\n", buffer);
		create_vector_from_buffer_number();
		vector_statistics();
	}

	void create_vector_from_buffer_number() {
		int i = 0;
		printf("<Server>: Create vector from buffer numbers: ");
		for( int i = 0; i < buffer_size; i++ )
			if( buffer[i] >= '0' && buffer[i] <= '9' ) {
				buffer_vector.push_back((int)(buffer[i] - '0'));
				printf("%i ", (int)(buffer[i] - '0') );
			}
		printf("\n");
	}

	void vector_sum() {
		int sum = 0;
		char * info_message = new char[100];
		for( int element : buffer_vector ) {
			sum += element;
		}
		sprintf(info_message,"\t\t- sum: %i\n",sum);
		printf("<Server>: %s", info_message);
		send(connection_info, info_message, 100, MSG_NOSIGNAL); //edit
	}

	void vector_sort() {
		sort( buffer_vector.begin(), buffer_vector.end() ); //Проверить на наличие ошибок в последнем параметре
		char * info_message = new char[100];
		long int result = 0, ex = 1;

		for(int element : buffer_vector) {
			result += element*ex;
			ex *= 10;
		}

		sprintf(info_message,"\t\t- sort: %li\n", result);
		printf("<Server>: %s", info_message);
		send(connection_info, info_message, 100, MSG_NOSIGNAL); //edit
	}

	void vector_max() {
		auto max = max_element( buffer_vector.begin(), buffer_vector.end() );
		char * info_message = new char[100];
		sprintf(info_message, "\t\t- max: %i\n", *max );
		printf("<Server>: %s", info_message);
		send(connection_info, info_message, 100, MSG_NOSIGNAL); //edit		
	}

	void vector_min() {
		auto min = min_element( buffer_vector.begin(), buffer_vector.end() );
		char * info_message = new char[100];
		sprintf(info_message, "\t\t- min: %i\n", *min );
		printf("<Server>: %s", info_message);
		send(connection_info, info_message, 100, MSG_NOSIGNAL); //edit
	}

	void vector_statistics() {
		if( buffer_vector.size() > 0 ) {
			printf("<Server>: Info about numbers in the message:\n"); //Отправляем то же самое клиенту
			send(connection_info, "<Server>: Info about numbers in the message:\n", 100, MSG_NOSIGNAL); //edit
			vector_sum();
			vector_sort();
			vector_max();
			vector_min();
		} else {
			printf("<Server>: Missing numbers in received message.\n"); 
			send(connection_info, "<Server>: Missing numbers in received message.\n" , 100, MSG_NOSIGNAL); //edit
			send(connection_info, "" , 100, MSG_NOSIGNAL);
			send(connection_info, "" , 100, MSG_NOSIGNAL);
			send(connection_info, "" , 100, MSG_NOSIGNAL);
			send(connection_info, "" , 100, MSG_NOSIGNAL);
		} 
		buffer_vector.erase( buffer_vector.begin(), buffer_vector.end() );
	}

	void shutdown_connection() {
		shutdown( socket_info, SHUT_RDWR);
		close( socket_info );
		printf( "<Server>: Shutdown connection.\n" );
		connection_status = OFFLINE_STATUS;
	}
};

//Интерфейс работы с пользователем
//Функция обработки входящих команд (с выводом ошибок при их некорректном вводе)
//Список команд: start_server, shutdown_server
class socket_interface : socket_connection {
public:
	int programm_status = ON;
	int port = 1111;
	void new_request(int req) {
		switch(req){
			case 1/*"-start_server"*/: create_connection(); break;
			case 2/*"-TCP"*/: set_TCP(); break;
			case 3/*"-UDP"*/: set_UDP(); break;
			case 4/*"-port"*/: printf("<Server>: Enter port number: "); scanf("%i", &port); set_port(port); break;
			case 5/*"-shutdown_server"*/: shutdown_connection(); break;
			case 6/*"-exit"*/: if(connection_status == ONLINE_STATUS) shutdown_connection(); programm_status = OFF; break;
			default: printf( "Unknown request, try again.\n" );
		}
	}
};



int main(){
	socket_interface connection;
	int request;
	while( connection.programm_status == ON ) {
		printf("<Server>: Number of server commands:\n\t1 - start server;\n\t2 - use TCP connection;\n\t3 - use UDP connection;\n\t4 - edit port number;\n\t5 - shutdown server;\n\t6 - exit from programm.\n\tEnter number of command: ");
		scanf("%i", &request); printf("\n");
		connection.new_request(request);
	}
	return 0;
}