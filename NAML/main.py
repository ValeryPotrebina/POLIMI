# Python Programming Guide - Полное руководство по программированию на Python
# Автор: AI Assistant
# Дата: 2025

"""
ОСНОВЫ ПРОГРАММИРОВАНИЯ НА PYTHON
=================================

Этот файл содержит все основные концепции Python с примерами кода.
"""

# ============================================================================
# 1. ОСНОВЫ СИНТАКСИСА И ПЕРЕМЕННЫЕ
# ============================================================================

def basic_syntax_examples():
    """Основы синтаксиса Python"""
    
    print("=== ОСНОВЫ СИНТАКСИСА ===")
    
    # Переменные и типы данных
    name = "Python"              # str - строка
    age = 30                     # int - целое число
    height = 175.5              # float - число с плавающей точкой
    is_programming = True        # bool - логический тип
    nothing = None              # NoneType - отсутствие значения
    
    print(f"Имя: {name}, Возраст: {age}, Рост: {height}")
    print(f"Программирует: {is_programming}, Значение: {nothing}")
    
    # Проверка типов
    print(f"Тип переменной name: {type(name)}")
    print(f"Тип переменной age: {type(age)}")

# ============================================================================
# 2. СТРУКТУРЫ ДАННЫХ
# ============================================================================

def data_structures_examples():
    """Основные структуры данных Python"""
    
    print("\n=== СТРУКТУРЫ ДАННЫХ ===")
    
    # СПИСКИ (Lists) - изменяемые, упорядоченные
    fruits = ["яблоко", "банан", "апельсин"]
    numbers = [1, 2, 3, 4, 5]
    mixed = [1, "текст", True, 3.14]
    
    print(f"Список фруктов: {fruits}")
    print(f"Первый фрукт: {fruits[0]}")
    print(f"Последний фрукт: {fruits[-1]}")
    
    # Операции со списками
    fruits.append("виноград")           # Добавить элемент
    fruits.insert(1, "киви")           # Вставить по индексу
    fruits.remove("банан")             # Удалить элемент
    print(f"Измененный список: {fruits}")
    
    # КОРТЕЖИ (Tuples) - неизменяемые, упорядоченные
    coordinates = (10, 20)
    person = ("Иван", 25, "инженер")
    
    print(f"Координаты: {coordinates}")
    print(f"Имя: {person[0]}, Возраст: {person[1]}")
    
    # СЛОВАРИ (Dictionaries) - изменяемые, неупорядоченные пары ключ-значение
    student = {
        "имя": "Анна",
        "возраст": 22,
        "курс": 3,
        "предметы": ["математика", "физика", "программирование"]
    }
    
    print(f"Студент: {student}")
    print(f"Имя студента: {student['имя']}")
    print(f"Предметы: {student.get('предметы', [])}")
    
    # Операции со словарями
    student["оценка"] = 5              # Добавить новый ключ
    student["возраст"] = 23            # Изменить значение
    del student["курс"]                # Удалить ключ
    
    # МНОЖЕСТВА (Sets) - изменяемые, неупорядоченные, уникальные элементы
    unique_numbers = {1, 2, 3, 4, 5}
    colors = {"красный", "зеленый", "синий"}
    
    print(f"Уникальные числа: {unique_numbers}")
    print(f"Цвета: {colors}")
    
    # Операции с множествами
    colors.add("желтый")               # Добавить элемент
    colors.discard("красный")          # Удалить элемент
    print(f"Измененные цвета: {colors}")

# ============================================================================
# 3. УСЛОВНЫЕ КОНСТРУКЦИИ И ЦИКЛЫ
# ============================================================================

def control_structures_examples():
    """Условия и циклы"""
    
    print("\n=== УСЛОВНЫЕ КОНСТРУКЦИИ И ЦИКЛЫ ===")
    
    # Условные конструкции
    temperature = 25
    
    if temperature > 30:
        print("Жарко!")
    elif temperature > 20:
        print("Тепло!")
    elif temperature > 10:
        print("Прохладно!")
    else:
        print("Холодно!")
    
    # Тернарный оператор
    weather = "солнечно" if temperature > 20 else "пасмурно"
    print(f"Погода: {weather}")
    
    # Цикл for
    print("\nЦикл for:")
    fruits = ["яблоко", "банан", "апельсин"]
    for fruit in fruits:
        print(f"Фрукт: {fruit}")
    
    # Цикл for с индексами
    for i, fruit in enumerate(fruits):
        print(f"{i + 1}. {fruit}")
    
    # Цикл for с range
    print("\nЧисла от 1 до 5:")
    for i in range(1, 6):
        print(i, end=" ")
    print()
    
    # Цикл while
    print("\nЦикл while:")
    count = 0
    while count < 3:
        print(f"Счетчик: {count}")
        count += 1
    
    # List comprehensions (генераторы списков)
    squares = [x**2 for x in range(1, 6)]
    print(f"Квадраты чисел: {squares}")
    
    even_squares = [x**2 for x in range(1, 11) if x % 2 == 0]
    print(f"Квадраты четных чисел: {even_squares}")

# ============================================================================
# 4. ФУНКЦИИ
# ============================================================================

def functions_examples():
    """Примеры функций"""
    
    print("\n=== ФУНКЦИИ ===")
    
    # Простая функция
    def greet(name):
        return f"Привет, {name}!"
    
    # Функция с значением по умолчанию
    def greet_with_title(name, title="г-н"):
        return f"Привет, {title} {name}!"
    
    # Функция с переменным количеством аргументов
    def sum_all(*numbers):
        return sum(numbers)
    
    # Функция с именованными аргументами
    def create_person(**kwargs):
        return {
            "имя": kwargs.get("имя", "Неизвестно"),
            "возраст": kwargs.get("возраст", 0),
            "город": kwargs.get("город", "Неизвестно")
        }
    
    # Примеры использования
    print(greet("Анна"))
    print(greet_with_title("Петров", "г-н"))
    print(greet_with_title("Иванова", title="г-жа"))
    
    print(f"Сумма: {sum_all(1, 2, 3, 4, 5)}")
    
    person = create_person(имя="Иван", возраст=30, город="Москва")
    print(f"Персона: {person}")
    
    # Lambda функции (анонимные функции)
    square = lambda x: x**2
    print(f"Квадрат 5: {square(5)}")
    
    # Использование lambda с map, filter
    numbers = [1, 2, 3, 4, 5]
    squared = list(map(lambda x: x**2, numbers))
    even_numbers = list(filter(lambda x: x % 2 == 0, numbers))
    
    print(f"Квадраты: {squared}")
    print(f"Четные числа: {even_numbers}")

# ============================================================================
# 5. ОБЪЕКТНО-ОРИЕНТИРОВАННОЕ ПРОГРАММИРОВАНИЕ
# ============================================================================

class Animal:
    """Базовый класс животного"""
    
    def __init__(self, name, species):
        self.name = name
        self.species = species
        self._age = 0  # Защищенный атрибут
    
    def speak(self):
        return f"{self.name} издает звук"
    
    def get_info(self):
        return f"{self.name} - {self.species}, возраст: {self._age}"
    
    @property
    def age(self):
        return self._age
    
    @age.setter
    def age(self, value):
        if value >= 0:
            self._age = value
        else:
            raise ValueError("Возраст не может быть отрицательным")

class Dog(Animal):
    """Класс собаки, наследует от Animal"""
    
    def __init__(self, name, breed):
        super().__init__(name, "Собака")
        self.breed = breed
    
    def speak(self):
        return f"{self.name} гавкает: Гав-гав!"
    
    def fetch(self):
        return f"{self.name} приносит палку"

class Cat(Animal):
    """Класс кошки, наследует от Animal"""
    
    def __init__(self, name, color):
        super().__init__(name, "Кошка")
        self.color = color
    
    def speak(self):
        return f"{self.name} мяукает: Мяу!"
    
    def purr(self):
        return f"{self.name} мурлычет"

def oop_examples():
    """Примеры ООП"""
    
    print("\n=== ОБЪЕКТНО-ОРИЕНТИРОВАННОЕ ПРОГРАММИРОВАНИЕ ===")
    
    # Создание объектов
    dog = Dog("Бобик", "Лабрадор")
    cat = Cat("Мурка", "Рыжий")
    
    # Использование методов
    print(dog.speak())
    print(dog.fetch())
    print(dog.get_info())
    
    print(cat.speak())
    print(cat.purr())
    print(cat.get_info())
    
    # Использование свойств
    dog.age = 3
    cat.age = 2
    
    print(f"Возраст собаки: {dog.age}")
    print(f"Возраст кошки: {cat.age}")
    
    # Полиморфизм
    animals = [dog, cat]
    for animal in animals:
        print(animal.speak())

# ============================================================================
# 6. РАБОТА С ФАЙЛАМИ
# ============================================================================

def file_operations_examples():
    """Примеры работы с файлами"""
    
    print("\n=== РАБОТА С ФАЙЛАМИ ===")
    
    # Запись в файл
    filename = "example.txt"
    
    # Запись текста
    with open(filename, 'w', encoding='utf-8') as file:
        file.write("Привет, Python!\n")
        file.write("Это пример работы с файлами.\n")
        file.writelines(["Строка 1\n", "Строка 2\n", "Строка 3\n"])
    
    print(f"Данные записаны в файл {filename}")
    
    # Чтение файла
    with open(filename, 'r', encoding='utf-8') as file:
        content = file.read()
        print("Содержимое файла:")
        print(content)
    
    # Чтение построчно
    print("Чтение построчно:")
    with open(filename, 'r', encoding='utf-8') as file:
        for line_num, line in enumerate(file, 1):
            print(f"Строка {line_num}: {line.strip()}")

# ============================================================================
# 7. ОБРАБОТКА ИСКЛЮЧЕНИЙ
# ============================================================================

def exception_handling_examples():
    """Примеры обработки исключений"""
    
    print("\n=== ОБРАБОТКА ИСКЛЮЧЕНИЙ ===")
    
    # Базовая обработка исключений
    try:
        number = int(input("Введите число (для демонстрации введите 'test'): "))
        result = 10 / number
        print(f"Результат: {result}")
    except ValueError:
        print("Ошибка: Введено не число!")
    except ZeroDivisionError:
        print("Ошибка: Деление на ноль!")
    except Exception as e:
        print(f"Неожиданная ошибка: {e}")
    else:
        print("Операция выполнена успешно!")
    finally:
        print("Этот блок выполняется всегда")
    
    # Создание собственных исключений
    class CustomError(Exception):
        """Пользовательское исключение"""
        def __init__(self, message):
            self.message = message
            super().__init__(self.message)
    
    def validate_age(age):
        if age < 0:
            raise CustomError("Возраст не может быть отрицательным")
        if age > 150:
            raise CustomError("Возраст слишком большой")
        return True
    
    try:
        validate_age(-5)
    except CustomError as e:
        print(f"Ошибка валидации: {e.message}")

# ============================================================================
# 8. МОДУЛИ И ПАКЕТЫ
# ============================================================================

def modules_examples():
    """Примеры работы с модулями"""
    
    print("\n=== МОДУЛИ И ПАКЕТЫ ===")
    
    # Стандартные модули
    import os
    import datetime
    import random
    import math
    from collections import Counter
    
    # Использование модулей
    print(f"Текущая директория: {os.getcwd()}")
    print(f"Текущее время: {datetime.datetime.now()}")
    print(f"Случайное число: {random.randint(1, 100)}")
    print(f"Квадратный корень из 16: {math.sqrt(16)}")
    
    # Counter из collections
    words = ["python", "java", "python", "javascript", "python"]
    word_count = Counter(words)
    print(f"Подсчет слов: {word_count}")

# ============================================================================
# 9. ПОЛЕЗНЫЕ ВСТРОЕННЫЕ ФУНКЦИИ
# ============================================================================

def built_in_functions_examples():
    """Примеры встроенных функций"""
    
    print("\n=== ВСТРОЕННЫЕ ФУНКЦИИ ===")
    
    numbers = [1, 2, 3, 4, 5]
    
    # Математические функции
    print(f"Сумма: {sum(numbers)}")
    print(f"Минимум: {min(numbers)}")
    print(f"Максимум: {max(numbers)}")
    print(f"Длина: {len(numbers)}")
    
    # Функции для работы с итерируемыми объектами
    print(f"Все элементы True: {all([True, True, False])}")
    print(f"Хотя бы один True: {any([False, False, True])}")
    
    # zip - объединение списков
    names = ["Анна", "Иван", "Петр"]
    ages = [25, 30, 35]
    combined = list(zip(names, ages))
    print(f"Объединенные списки: {combined}")
    
    # enumerate - получение индексов
    for i, name in enumerate(names):
        print(f"{i}: {name}")
    
    # sorted - сортировка
    unsorted = [3, 1, 4, 1, 5, 9, 2, 6]
    sorted_list = sorted(unsorted)
    print(f"Отсортированный список: {sorted_list}")
    
    # reversed - обращение
    reversed_list = list(reversed(numbers))
    print(f"Обращенный список: {reversed_list}")

# ============================================================================
# 10. ЛУЧШИЕ ПРАКТИКИ И СТИЛЬ КОДА
# ============================================================================

def best_practices_info():
    """Информация о лучших практиках"""
    
    print("\n=== ЛУЧШИЕ ПРАКТИКИ И СТИЛЬ КОДА ===")
    
    practices = [
        "1. Используйте понятные имена переменных и функций",
        "2. Следуйте PEP 8 - стандарту стиля Python",
        "3. Пишите документацию для функций и классов",
        "4. Используйте list comprehensions для простых операций",
        "5. Обрабатывайте исключения правильно",
        "6. Используйте виртуальные окружения для проектов",
        "7. Тестируйте ваш код",
        "8. Избегайте глобальных переменных",
        "9. Используйте type hints для лучшей читаемости",
        "10. Рефакторьте код регулярно"
    ]
    
    for practice in practices:
        print(practice)
    
    print("\nПример хорошего стиля кода:")
    print("""
def calculate_circle_area(radius: float) -> float:
    '''
    Вычисляет площадь круга по радиусу.
    
    Args:
        radius (float): Радиус круга
        
    Returns:
        float: Площадь круга
        
    Raises:
        ValueError: Если радиус отрицательный
    '''
    if radius < 0:
        raise ValueError("Радиус не может быть отрицательным")
    
    return 3.14159 * radius ** 2
    """)

# ============================================================================
# ГЛАВНАЯ ФУНКЦИЯ
# ============================================================================

def main():
    """Главная функция программы"""
    
    print("ПОЛНОЕ РУКОВОДСТВО ПО ПРОГРАММИРОВАНИЮ НА PYTHON")
    print("=" * 50)
    
    # Выполнение всех примеров
    basic_syntax_examples()
    data_structures_examples()
    control_structures_examples()
    functions_examples()
    oop_examples()
    file_operations_examples()
    
    # Примеры с вводом пользователя - закомментированы для автоматического выполнения
    # exception_handling_examples()
    
    modules_examples()
    built_in_functions_examples()
    best_practices_info()
    
    print("\n" + "=" * 50)
    print("РУКОВОДСТВО ЗАВЕРШЕНО!")
    print("Теперь вы знаете основы программирования на Python!")

if __name__ == "__main__":
    main()
