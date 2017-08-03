# Задание №1 **"Универсальный архиватор"**

## Описание
Вам необходимо реализовать архиватор.

## Правила
* Не списывайте, будем карать.
* После дедлайна из вашего репозитория берется последний успешный коммит
и проводится его тестирование на закрытом наборе.

## Алгоритм действий
1. Зарегистрировать аккаунт на GitLab.
2. Fork'нуть себе этот проект, сделать его закрытым.
3. Прислать ссылку на репозиторий куратору курса.
4. Собрать шаблон на своём компьютере.
5. Реализовать архиватор.
6. Запушить итоговую версию в свой репозиторий на GitLab.

## Сборка
### **Ubuntu**
1. Обновляем package list:

    ```sh
    sudo apt update
    ```
    
2. Устанавливаем всё необходимое.

    ```sh
    sudo apt install --yes git
    sudo apt install --yes cmake
    sudo apt install --yes g++
    sudo apt install --yes python3
    ```
    
3. Клонируем репозиторий в вашу любимую папку:

    ```sh
    cd path/to/favorite/folder
    git clone https://gitlab.com/Ploshkin/compression.git
    ```
    После этого там должна появиться папка compression.
    
4. Заходим в папку compression/project, создаём папку bin и заходим в неё:

    ```sh
    cd compression/project
    mkdir bin
    cd bin

    ```

5. Теперь конфигурируем проект с помощью CMake, указав ../src как папку
    с исходниками, а после ключа `-G` — формат проекта, который мы хотим получить.
    Например, эта команда сгенерирует самый обыкновенный Makefile:

    ```sh
    cmake -G "Unix Makefiles" ../src
    ```
    
    А эта — проект для CodeBlocks:
    
    ```sh
    cmake -G "CodeBlocks - Unix Makefiles" ../src
    ```
    
    Все поддерживаемые форматы можно найти в справке `cmake --help`.
    В дальнейшем вам **не нужно** будет конфигурировать проект повторно.
    
6. Собираем проект удобным образом, например:

    ```sh
    make  # нужно быть в папке compression/project/bin
    ```
    
7. Проверяем папку compression/project/bin на наличие исполняемого файла
   compress.
8. Тестируем шаблон:

    ```sh
    cd ../..  # поднимаемся из bin в папку compression
    python3 test.py
    ```
    
    После успешного выполнения тестов в папке compression появится файл
    res.csv, который можно открыть и посмотреть, что тестирование
    действительно прошло успешно.
9. Ура, всё получилось! Вы готовы создавать лучший в мире архиватор.
    
    
### **Windows**
Эпиграф:

— Надо было ставить Linux!

1. В первую очередь ставим Git (https://git-for-windows.github.io/).
2. Теперь ставим CMake (https://cmake.org/download/).
3. Для тестирующего скрипта ставим интерпретатор Python (https://www.python.org/downloads/).
4. На всякий случай ставим MinGW (https://sourceforge.net/projects/mingw/files/Installer/).
    Основной сайт: http://www.mingw.org/
5. Запускаем Windows PowerShell.
6. Клонируем репозиторий в вашу любимую папку:

    ```sh
    cd path/to/favorite/folder
    git clone https://gitlab.com/Ploshkin/compression
    ```
    После этого там должна появиться папка compression.
    
7. Заходим в папку compression/project, создаём папку bin и заходим в неё:

    ```sh
    cd compression/project
    mkdir bin
    cd bin

    ```

8. Теперь конфигурируем проект с помощью CMake, указав ../src как папку
    с исходниками, а после ключа `-G` — формат проекта, который мы хотим получить.
    Например, эта команда сгенерирует самый обыкновенный Makefile:

    ```sh
    cmake -G "MinGW Makefiles" ../src
    ```
    
    А эта — проект для CodeBlocks:
    
    ```sh
    cmake -G "CodeBlocks - MinGW Makefiles" ../src
    ```
    
    Все поддерживаемые форматы можно найти в справке `cmake --help`.
    В дальнейшем вам **не нужно** будет конфигурировать проект повторно.
    
9. Собираем проект удобным образом, например:

    ```sh
    mingw32-make  # нужно быть в папке compression/project/bin
    ```
    
10. Проверяем папку compression/project/bin на наличие исполняемого файла
   compress.exe.
11. Тестируем шаблон:

    ```sh
    cd ../..  # поднимаемся из bin в папку compression
    python test.py
    ```
    
    После успешного выполнения тестов в папке compression появится файл
    res.csv, который можно открыть и посмотреть, что тестирование
    действительно прошло успешно.
12. Ура, всё получилось! Вы готовы создавать лучший в мире архиватор.

## Запуск
Общий вид:

```sh
# Ubuntu
compress [options]
```

```sh
# Windows
compress.exe [options]
```

Опции:

```cmd
--help                     = Вызов справки, аналогичной этой.
--input  <file>            = Входной файл для архивации/разархивации, по умолчанию `input.txt`.
--output <file>            = Выходной файл для архивации/разархивации, по умолчанию `output.txt`.
--mode   {c | d}           = Режим работы: `c` - архивация, `d` - разархивация; по умолчанию `c`.
--method {ari | ppm | bwt} = Метод архивации/разархивации файла, по умолчанию `ari`.
```

Например, если мы хотим заархивировать файл the_financier.txt в файл the_financier.cmp методом PPM, то команда будет выглядеть так:

```sh
# Ubuntu
compress --input the_financier.txt --output the_financier.cmp --mode c --method ppm
```

```sh
# Windows
compress.exe --input the_financier.txt --output the_financier.cmp --mode c --method ppm
```