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
	int socket_domain = PF_INET;
	int socket_type = SOCK_STREAM;
	int socket_protocol = IPPROTO_TCP;
	int socket_port = 1234; //Выбрать правильный порт для создания соединения
	int socket_info = 0;
	int connection_info = 0;
	int connection_status = OFFLINE_STATUS;
	string socket_type_string = "TCP";
	string connection_ip = "127.0.0.1"; //Может быть ошибка
	int char_max_size = 100;
	int buffer_size = 100; //Test
	char *buffer = new char[char_max_size] {"ERROR: recv is not work."};
	vector<int> buffer_vector;

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

	void recv_message() { //Может быть ошибка
		//Приём и отправка сообщений клиенту 
		//Добавить ограничения на размер сообщения и сделать вывод ошибок
		//Если будет цикл, то возможно стоит пустить его отдельным процессом, чтобы иметь возможность управлять сервером
		//Может принимать сообщение с длинной отправленного сообщения?
		recv(connection_info, buffer, buffer_size, MSG_NOSIGNAL);
		printf("<Client>: %c", buffer);
		//create_vector_from_buffer_number();
		//vector_statistics();
		buffer = "ERROR: recv is not work.";
	}

	void create_vector_from_buffer_number() {
		int i = 0;
		while( buffer[i] = "\0" ){
			if( buffer[i] > "0" && buffer[i] < "9" ) {
				buffer_vector.push_back(buffer[i] - "0");
			}
			i++;
		}
	}

	void vector_sum() {
		int sum = 0;
		char *info_message = "- sum: ";
		for( int element : buffer_vector ) {
			sum += element;
		}
		info_message += itoa(sum); //try string(&itoa(sum))
		printf("<Server>: %c\n", info_message);
		//Отправляем то же самое клиенту
	}

	void vector_sort() {
		sort( buffer_vector.begin(), buffer_vector.end(), greater<int> ); //Проверить на наличие ошибок в последнем параметре
		string info_message = "- sort: ";
		for(int element : buffer_vector) {
			info_message += itoa(sum); //try string(&itoa(sum))
		}
		printf("<Server>: %s\n", info_message);
		//Отправляем то же самое клиенту
	}

	void vector_max() {
		int max = max_element( buffer_vector.begin(), buffer_vector.end() );
		string info_message = "- max: ";
		info_message += itoa(max); //try string(&itoa(sum))
		printf("<Server>: %s\n", info_message);		
		//Отправляем то же самое клиенту
	}

	void vector_min() {
		int min = min_element( buffer_vector.begin(), buffer_vector.end() );
		string info_message = "- min: ";
		info_message += itoa(min); //try string(&itoa(sum))
		printf("<Server>: %s\n", info_message);
		//Отправляем то же самое клиенту
	}

	void vector_statistics() {
		if( buffer_vector.size() > 0 ) {
			printf("<Server>: Info about numbers in the message:\n"); //Отправляем то же самое клиенту
			vector_sum();
			vector_sort();
			vector_max();
			vector_min();
		} else printf("<Server>: Missing numbers in received message.\n"); //Отправляем то же самое клиенту
		buffer_vector.erase( buffer_vector.begin(), buffer_vector.end() );
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
class socket_interface : socket_connection {
public:
	int programm_status = ON;
	void new_request(string req) {
		switch(req){
			case "-start_server": create_connection(); break;
			case "-TCP": set_TCP(); break;
			case "-UDP": set_UDP(); break;
			case "-port": /*set_port(port); */ break;
			case "-shutdown_server": shutdown_connection(); break;
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