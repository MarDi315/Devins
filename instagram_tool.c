#include <stdio.h>
#include <string.h>   // Для работы со строками
#include <stdlib.h>   // Для malloc и free

// Библиотеки потоков
#include <pthread.h>

// Библиотеки libcurl
#include <curl/curl.h>
#include <curl/easy.h>

//Наш собственный токен
static const char authToken[] = "7ffbeb46b740dbb57eb9053419e9cbe117a37ed01d65a9a782f1ec863";

// Функция для получения ID аккаунта
long getUserId(char *username) {
    // Создаем переменные
	long userId = 0;
	char* jsonBuffer;

    // Создаем объект curl
	CURL *curl = curl_easy_init();
	if (curl) {
        // Обращаемся к Instagram
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.instagram.com/web/users/search/?q="+username+"&context=blended");

        // Добавляем наш токен
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Authorization: Bearer "+authToken);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		//Насочиаем данные в буфер
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, jsonBuffer);

		// Выполняем
		curl_easy_perform(curl);

    	//Парсим данные
		jsonBuffer = strstr(jsonBuffer,"\"pk\":");
		jsonBuffer = jsonBuffer+5;
		userId = (long)strtol(jsonBuffer,NULL, 10);

		// Освобождаем память
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}

	// Возвращаем ID аккаунта
	return userId;
}

// Функция для получения пароля
void getPassword(long userId) {
    // Создаем переменные
	char* jsonBuffer;
	char* password;

    // Создаем объект curl
	CURL *curl = curl_easy_init();
	if (curl) {
	    // Обращаемся к Instagram
		char url[256];
		sprintf(url, "https://www.instagram.com/web/users/%ld/info/", userId);
		curl_easy_setopt(curl, CURLOPT_URL, url);

        // Добавляем наш токен
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Authorization: Bearer "+authToken);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Насочиваем данные в буфер
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, jsonBuffer);

        // Выполняем
		curl_easy_perform(curl);

		//Парсим данные
		char* tmp = strstr(jsonBuffer,"\"password\":\"");
		if (tmp!=NULL)
		  {
			password = tmp + 12;
			// Замена пароля на точки при выводе на экран
			for (int i=0; i<strlen(password); i++)
				if (password[i] != ' ')
					password[i] = '*';

            // Вывод пароля
			printf("Password: %s\n", password);
		}

        // Освобождаем память
		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
}

int main(int argc, char *argv[]) {
    // Проверяем корректность введенных аргументов
	if (argc !=2)
		printf("Usage: %s [instagram username]\n", argv[0]);
	else {
		long userId = getUserId(argv[1]);

        // Проверяем существование пользователя
		if (userId>0) {
			getPassword(userId);
		}

        // Пользователь не существует
		else
			printf("Неверное имя пользователя\n");
	}

	return 0;
}
