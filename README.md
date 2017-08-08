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
### **Unix**
В примере используется пакетный менеджер APT, который по умолчанию стоит на Ubuntu.
На macOS можно пользоваться Homebrew, и тогда на 1-2 этапах
будет использоваться команда `brew` вместо `apt`.

1. Обновляем package list:

    ```sh
    sudo apt update
    ```
    
2. Устанавливаем всё необходимое.

    ```sh
    sudo apt install --y git cmake g++ python3
    ```
    
3. Клонируем репозиторий в вашу любимую папку:

    ```sh
    cd path/to/favorite/folder
    git clone https://gitlab.com/Ploshkin/compression.git
    ```
    После этого там должна появиться папка compression.
    
4. Заходим в папку compression, создаём папку build и заходим в неё:

    ```sh
    mkdir build
    cd build

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
    make  # нужно быть в папке compression/build
    ```
    
7. Проверяем папку compression/build на наличие исполняемого файла
   compress.
8. Тестируем шаблон:

    ```sh
    cd ..  # поднимаемся из build в папку compression
    python3 test.py
    ```
    
    После успешного выполнения тестов в папке compression появится файл
    res.csv, который можно открыть и посмотреть, что тестирование
    действительно прошло успешно.
9. Ура, всё получилось! Вы готовы создавать лучший в мире архиватор.
    
    
### **Windows**
Эпиграф:

— Надо было ставить Linux!

1. В первую очередь ставим [Git for Windows](https://git-for-windows.github.io/).
2. Теперь ставим [CMake](https://cmake.org/download/).
3. Для тестирующего скрипта ставим интерпретатор [Python 3](https://www.python.org/downloads/).
4. На всякий случай ставим [MinGW](http://www.mingw.org/) по этой
    [ссылке](https://sourceforge.net/projects/mingw/files/Installer/).
5. Запускаем Windows PowerShell.
6. Клонируем репозиторий в вашу любимую папку:

    ```sh
    cd path/to/favorite/folder
    git clone https://gitlab.com/Ploshkin/compression
    ```
    После этого там должна появиться папка compression.
    
7. Заходим в папку compression, создаём папку build и заходим в неё:

    ```sh
    mkdir build
    cd build
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
    mingw32-make  # нужно быть в папке compression/build
    ```
    
10. Проверяем папку compression/build на наличие исполняемого файла
   compress.exe.
11. Тестируем шаблон:

    ```sh
    cd ..  # поднимаемся из build в папку compression
    python tests/run.py
    ```
    
    После успешного выполнения тестов в папке compression появится файл
    res.csv, который можно открыть и посмотреть, что тестирование
    действительно прошло успешно.
12. Ура, всё получилось! Вы готовы создавать лучший в мире архиватор.

## Запуск
Общий вид:

```sh
# Unix
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

Например, если мы хотим заархивировать файл the_financier.txt в файл
the_financier.cmp методом PPM, то команда будет выглядеть так:

```sh
# Unix
compress --input the_financier.txt --output the_financier.cmp --mode c --method ppm
```

```sh
# Windows
compress.exe --input the_financier.txt --output the_financier.cmp --mode c --method ppm
```

## Работа с репозиторием и системой контроля версий
Git — это просто, полезно и очень удобно.

Для выполнения задания вам понадобится всего 7 команд:

1. Инициализировать себя (в самом-самом начале):

    ```sh
    git config --global user.name "Ivan Ivanov"
    git config --global user.email "ivanov@email.com"
    ```
    
2. Клонировать репозиторий:
    
    ```sh
    git clone <repo-url>
    ```
    
    `repo-url` — URL к репозиторию.
    
3. Добавить удалённый репозиторий:

    ```sh
    git remote add <name> <repo-url>
    ```
    
    `name` — имя репозитория для использования в дальнейшем в командной строке.
    
    Пример:
    
    ```sh
    git remote add my_cmpr https://gitlab.com/<Your nickname>/compressor.git
    ```
    
4. Добавить файл/папку к текущим изменениям:

    ```sh
    git add <path>
    ```
    
    Если вы находитесь в корневой папке проекта, можно сделать так:
    
    ```sh
    git add .
    ```
    
    Тогда добавятся все файлы, которые изменились со времени последнего коммита.
    
5. Сделать коммит:

    ```sh
    git commit -m "<comment>"
    ```
    
    `comment` — комментарий к коммиту.
    
    Эта команда создает в локальном репозитории новую версию проекта,
    которая включает все изменения, добавленные с помощью `git add`.
    
    Пример:
    
    ```sh
    git commit -m "Add PPM"
    ```
    
6. Запушить репозиторий:

    ```sh
    git push <name> <branch>
    ```
    
    `name` — имя репозитория из пункта 3,
    `branch` — имя ветки.
    
    Эта команда пушит ваш локальный репозиторий в указанный,
    так что все файлы в указанном репозитории будут обновлены до последней версии.
    
    Коротко про ветки: Git может поддерживать несколько независимых веток одного
    проекта. У каждого проекта существует главная ветка — `master`, которая,
    по-хорошему, должна представлять из себя последнюю рабочую версию проекта.
    В остальных ветках пилится что-то новое, и, в случае успешного тестирования,
    происходит слияние ветки с главной.
    Но вы можете не заморачиваться и всегда пушить в `master`.
    
    Пример:
    
    ```sh
    git push my_cmpr master
    ```
    
7. Pull репозитория:
    
    ```sh
    git pull <name> <branch>
    ```
    
    Эта команда, наоборот, "забирает" из указанного репозитория версию проекта,
    и, если ваш локальный проект имеет более страую версию, то он обновляется.
    
    Пример:
    
    ```sh
    git pull my_cmpr master
    ```
    
Удобная связка команд для того, чтобы запушить новую версию (например,
вы реализовали декодирование арифметическим алгоритмом):

```sh
# из корневой папки проекта
git add .
git commit -m "Add ARI decompressor"
git push my_cmpr master
```

Вот и всё!

Если вы хотите более подробно ознакомиться с Git:
[Getting Started](https://www.atlassian.com/git/tutorials/setting-up-a-repository)

## Описание репозитория
* `config.cfg` — файл с указанием реализованных методов сжатия в формате списка 
    [JSON](http://www.json.org/).
    
    Всего допускается 3 значения: "ARI", "PPM", "BWT".
    
    Пример содержимого файла:
    
    ```json
    ["ARI", "BWT"]
    ```
    
    Этот файл необходим для тестирующего скрипта, чтобы определить,
    какие методы тестировать.
    
* `test.py` — тестирующий скрипт на языке Python. 
    Его можно использовать для тестирования вашего архиватора следующим образом:

    ```sh
    python test.py [--testdir <dir>] [--timeout <timeout>]
    ```
    
    * `dir` — папка, в которой лежат тестовые файлы, по умолчанию — tests;
    * `timeout` — время, отведённое для тестирования каждого файла, по умолчанию
    180с.
    
    Скрипт сжимает и распаковывает каждый файл, находящийся в указанной папке,
    вычисляет размер получившихся файлов, сравнивает содержимое и записывает
    результат по каждому файлу в res.csv.
    
    Возможные вердикты:
    * OK — файл успешно сжат, распакован и совпадает с исходным;
    * WA — файл успешно сжат и распакован, но не совпадает с исходным;
    * RE1 — ошибка выполнения при сжатии;
    * TL1 — превышен таймаут при сжатии;
    * RE2 — ошибка выполнения при распаковке;
    * TL2 — превышен таймаут ожидания при распаковке.
    
* `tests` — папка с тестовыми файлами. На этих файлах каждый раз тестируется
    ваше решение при его пуше на GitLab. Настоятельно рекомендуется не трогать
    эту папку — она здесь исключительно для вашего удобства, основное
    тестирование будет проводиться на наших закрытых тестовых файлах.

* `src` — папка с исходными файлами:
    * main.c — основной файл;
    * utils.c — файл, содержащий функции, чтобы парсить аргументы командной строки;
    * ari.c, ppm.c, bwt.c — файлы, которые вам нужно изменить, написав
    соответствующий алгоритм;
    * *.h — заголовочные файлы ко всем вышеуказанным;
    * CMakeLists.txt — файл, необходимый для генерации нужного вам формата
    прокета с помощью CMake.

    Вообще говоря, никто вас не обязывает использовать именно такую архитектуру
    решения. Самое главное — ваше решение должно поддерживать указанный формат
    запуска.