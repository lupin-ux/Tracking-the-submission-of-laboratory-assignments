

#include "LabAccounting.h"
#include <iostream>
#include <string>
#include <stdexcept>
const int MAX_INPUT_ATTEMPTS = 5;

bool ReadInt(const std::string &prompt, int &out)
{
    std::string line;
    for (int attempt = 1; attempt <= MAX_INPUT_ATTEMPTS; ++attempt) {
        std::cout << prompt;
        std::cout.flush();

        if (!std::getline(std::cin, line)) {
            std::cout << "Ошибка: не удалось прочитать данные с клавиатуры.\n";
            continue;
        }
        if (line.empty()) {
            std::cout << "Ошибка: строка пуста, введите целое число.\n";
            continue;
        }
        try {
            size_t pos = 0;
            int value = std::stoi(line, &pos);
            if (pos != line.size()) {
                std::cout << "Ошибка: введено не целое число (обнаружен текст).\n";
                continue;
            }
            out = value;
            return true;
        } catch (const std::exception &) {
            std::cout << "Ошибка: введено не целое число.\n";
        }
    }
    std::cout << "Превышено число попыток ввода (" << MAX_INPUT_ATTEMPTS << ").\n";
    return false;
}

bool ReadDouble(const std::string &prompt, double &out)
{
    std::string line;
    for (int attempt = 1; attempt <= MAX_INPUT_ATTEMPTS; ++attempt) {
        std::cout << prompt;
        std::cout.flush();

        if (!std::getline(std::cin, line)) {
            std::cout << "Ошибка: не удалось прочитать данные с клавиатуры.\n";
            continue;
        }
        if (line.empty()) {
            std::cout << "Ошибка: строка пуста, введите число.\n";
            continue;
        }
        try {
            size_t pos = 0;
            double value = std::stod(line, &pos);
            if (pos != line.size()) {
                std::cout << "Ошибка: введено не число (обнаружен текст).\n";
                continue;
            }
            out = value;
            return true;
        } catch (const std::exception &) {
            std::cout << "Ошибка: введено не число.\n";
        }
    }
    std::cout << "Превышено число попыток ввода (" << MAX_INPUT_ATTEMPTS << ").\n";
    return false;
}

void PauseBeforeExit()
{
    std::cout << "\nНажмите Enter, чтобы закрыть программу...";
    std::cout.flush();
    std::cin.clear();
    std::string dummy;
    std::getline(std::cin, dummy);
}

/* ==================================================================
 * Главная функция
 * ================================================================== */
int main()
{
    std::cout << "================================================================\n";
    std::cout << " Лабораторная работа №2. Учёт сдачи лабораторных работ\n";
    std::cout << "================================================================\n";
    std::cout << "Программа демонстрирует класс LabAccounting: выбор дисциплины,\n";
    std::cout << "задание сложности работ, внесение оценок, подсчёт средней\n";
    std::cout << "оценки и обработку ошибок ввода при помощи исключений.\n\n";

    /* ---------- Шаг 1. Выбор дисциплины ---------- */
    std::cout << "Доступные дисциплины:\n";
    for (int i = 0; i < static_cast<int>(Discipline::COUNT); ++i) {
        std::cout << "  " << (i + 1) << " — "
                  << DisciplineToString(static_cast<Discipline>(i)) << "\n";
    }
    std::cout << "Номера, которых нет в списке, можно ввести намеренно, чтобы\n";
    std::cout << "проверить, как конструктор отвергает недопустимое значение.\n\n";

    int disciplineNumber = 0;
    if (!ReadInt("Введите номер дисциплины: ", disciplineNumber)) {
        PauseBeforeExit();
        return 1;
    }

    /* Приведение к типу перечисления выполняется без проверки
     * намеренно: корректность значения проверит сам класс.     */
    Discipline discipline = static_cast<Discipline>(disciplineNumber - 1);

    /* ---------- Шаг 2. Количество лабораторных работ ---------- */
    std::cout << "\nКоличество лабораторных работ должно быть от 1 до "
              << LabAccounting::MAX_LAB_COUNT << ".\n";
    int labCount = 0;
    if (!ReadInt("Введите количество лабораторных работ: ", labCount)) {
        PauseBeforeExit();
        return 1;
    }

    /* ---------- Шаг 3. Создание объекта ---------- */
    LabAccounting *tracker = nullptr;
    try {
        tracker = new LabAccounting(discipline, labCount);
    } catch (const std::exception &e) {
        std::cout << "\nНе удалось создать объект. Ошибка: " << e.what() << "\n";
        PauseBeforeExit();
        return 1;
    }

    std::cout << "\nОбъект создан. Начальное состояние:\n";
    tracker->Print(std::cout);

    /* ---------- Шаг 4. Задание сложности каждой работы ---------- */
    std::cout << "\nЗадайте сложность каждой лабораторной работы:\n";
    std::cout << "  1 — лёгкая, 2 — средняя, 3 — сложная\n";
    std::cout << "  0 — оставить значение по умолчанию (средняя)\n";
    std::cout << "Другие числа недопустимы и будут отвергнуты классом.\n";

    for (unsigned int number = 1; number <= tracker->GetLabCount(); ++number) {
        bool accepted = false;
        for (int attempt = 0; attempt < MAX_INPUT_ATTEMPTS && !accepted; ++attempt) {
            int value = 0;
            if (!ReadInt("  Сложность работы №" + std::to_string(number) + ": ", value)) {
                break;
            }
            if (value == 0) {
                accepted = true;   /* оставляем значение по умолчанию */
                continue;
            }
            try {
                tracker->SetDifficulty(number, static_cast<LabAccounting::Difficulty>(value));
                accepted = true;
            } catch (const std::exception &e) {
                std::cout << "  Ошибка: " << e.what() << " Повторите ввод.\n";
            }
        }
    }

    /* ---------- Шаг 5. Внесение оценок ---------- */
    std::cout << "\nВнесите оценки за сданные лабораторные работы.\n";
    std::cout << "Чтобы завершить ввод, введите номер работы 0.\n";

    while (true) {
        int number = 0;
        if (!ReadInt("\nНомер лабораторной работы (0 — закончить): ", number)) {
            break;
        }
        if (number == 0) {
            break;
        }

        double grade = 0.0;
        if (!ReadDouble("Оценка за эту работу (от " + FormatNumber(LabAccounting::MIN_GRADE) +
                        " до " + FormatNumber(LabAccounting::MAX_GRADE) + "): ", grade)) {
            break;
        }

        try {
            /* Номер работы в классе беззнаковый. Отрицательное число
             * при приведении станет очень большим и будет отвергнуто
             * проверкой диапазона внутри класса.                     */
            tracker->SubmitLab(static_cast<unsigned int>(number), grade);
            std::cout << "Оценка внесена успешно.\n";
        } catch (const std::exception &e) {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }

    /* ---------- Шаг 6. Итоговое состояние ---------- */
    std::cout << "\nИтоговое состояние:\n";
    tracker->Print(std::cout);

    try {
        /* Значение получаем до вывода: иначе подпись успела бы
         * напечататься прежде, чем селектор бросит исключение. */
        double average = tracker->GetCurrentGrade();
        std::cout << "Текущая (средняя) оценка по дисциплине: "
                  << FormatNumber(average, 2) << "\n";
    } catch (const std::exception &e) {
        std::cout << "Средняя оценка недоступна: " << e.what() << "\n";
    }

    /* ---------- Шаг 7. Проверка правила трёх ----------
     * Создаём копию объекта и убеждаемся, что она не связана
     * с оригиналом: изменение копии не затрагивает оригинал.  */
    std::cout << "\n--- Проверка корректности копирования объекта ---\n";
    {
        LabAccounting copy(*tracker);   /* работает конструктор копирования */

        std::cout << "Сдано работ в оригинале: " << tracker->GetDoneCount() << "\n";
        std::cout << "Сдано работ в копии:     " << copy.GetDoneCount() << "\n";

        /* Ищем в копии любую несданную работу и сдаём её */
        bool changed = false;
        for (unsigned int number = 1; number <= copy.GetLabCount() && !changed; ++number) {
            try {
                copy.SubmitLab(number, LabAccounting::MAX_GRADE);
                std::cout << "В копию внесена оценка за работу №" << number << ".\n";
                changed = true;
            } catch (const std::exception &) {
                /* работа уже сдана — пробуем следующую */
            }
        }

        if (changed) {
            std::cout << "После изменения копии:\n";
            std::cout << "  сдано работ в оригинале: " << tracker->GetDoneCount() << "\n";
            std::cout << "  сдано работ в копии:     " << copy.GetDoneCount() << "\n";
            std::cout << "Оригинал не изменился — копирование глубокое, "
                         "объекты независимы.\n";
        } else {
            std::cout << "Все работы уже сданы, изменить копию не удалось.\n";
        }
    }   /* здесь вызывается деструктор копии — освобождаются её массивы */

    delete tracker;   /* деструктор оригинала */

    std::cout << "\nРабота программы завершена.\n";
    PauseBeforeExit();
    return 0;
}
