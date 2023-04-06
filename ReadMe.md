# Модельный рендерер-трассировщик лучей (C++, CPU)
## Зависимости
Подразумеватеся что проект запускается на `Linux`.
Для работы и сборки требуется установленная `OpenGL` библиотека `glfw3`. Установить такую можно введя в командной строке 
```
$ sudo apt-get install libglfw3-dev
```
Для сборки программы также требуется компилятор `g++` и утилиты `cmake` и `make`.

## Сборка и запуск
В папке проекта:
```
$ cmake ./
$ make
$ ./bin/main
```
В цели проекта не входил пользовательский интерфейс создания сцены (но может рассматриваться как его дальнейшее развитие), поэтому результат работы программы один - демонстрация возможноcтей "движка": по окончании отрисовки, в отдельном окне появится рендер заранее заданной сцены. Вы увидите что то вроде панельного дома ночью в стеклянном кубе, стоящем на матовом кубе и несколько шаров вокруг (светящийся, матовый и полупрозрачный).  
Рендер происходит на `cpu` и занимает существенное время, поэтому по умолчанию генерируется урезанное в качестве изображение (в моем случае на `Intel Core i5-8250U CPU @ 1.60GHz` это занимает несколько минут). В папке `./resources` лежит несколько более тяжелых заранее отрендеренных изображений.

## Кратко о проделанной работе
В этом проекте я полностью написал небольшой движок для  трассировки лучей на C++ (в учебных целях чистый C++ без  STL). Единственный использованный готовый код - попиксельная отрисовка окна в OpenGL.

Конкретно реализованы:
 
- `lin_al.hpp`: библиотека шаблонов n-мерных векторов произвольного типа со всеми основными операторами и специальными векторными операциями

- модуль `graphicObjects`:
	- объект камеры со всеми необходимыми настройками:  
		- расположение и ориентация в пространстве
		- угол обзора fov
		- разрешение матрицы камеры
		- уровень антиалайазинга(лучей на пиксель)
	- иерархия графических объектов со своими методами взаимодействия с пересекаемым лучем и алгоритмом поиска пересечения : графический объект ->
		- шар
		- полигон
		- полигональный объект(коллекция полигонов)  
	- объект луча с соответствующими методами (напр. `Ray::Cast()`)

- модуль `interactonModels`:
	- иерархия материалов (моделей взаимодействия с объектом). Реализованы модели взаимодействия:
		- `Schlick`: взаимодействие с полупрозрачным объектом с учетом:
			- коэффициентов преломления (по закону Снеллиуса)
			- спектра внутреннего поглощения света (в толще материала)
 			- фактора Френеля (посредством аппроксимации Шлика)
			- эффекта полного внутреннего отражения
		- `Lambert`: взаимодействие с асболютно ламбертовой поверхностью, с учетом
			- угла падения на материал и полного рассеяния (модель Ламберта)
			- спектра поверхностного полглощения света
		- `SimpleEmission`: взаимодействие с простым излучающим материалом (самая элементарная модель)

- `main.cpp`: задание сцены и рендеринг с помощью модулей, перечисленных выше

Также:
- настраиваются глубина рекурсии и точность равномерного рассеяния
- для проверки пересечения с полигонами реализован быстрый алгоритм Мёллера-Трамбора

---
Владимиров Аркадий 2021.
 
  
