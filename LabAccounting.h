/* ==================================================================
 * Лабораторная работа №2. Базовый синтаксис классов
 * Вариант 9. «Учёт сдачи лабораторных работ»
 *
 * Файл: LabAccounting.h — объявление класса LabAccounting.
 * Реализация методов находится в файле LabAccounting.cpp.
 *
 * Класс хранит сведения о процессе сдачи лабораторных работ
 * по одной дисциплине одним студентом.
 *
 * Поля класса (согласно заданию):
 *   - дисциплина (перечисление Discipline);
 *   - количество лабораторных работ;
 *   - оценки за лабораторные работы (динамический массив);
 *   - сложность лабораторных работ (динамический массив);
 *   - текущая оценка по дисциплине;
 *   - количество сданных лабораторных работ.
 *
 * Исключения по вводу данных (семь, требуется не менее шести):
 *   1) недопустимое значение дисциплины          — invalid_argument;
 *   2) количество работ меньше либо равно нулю   — invalid_argument;
 *   3) количество работ больше допустимого       — out_of_range;
 *   4) номер работы вне диапазона                — out_of_range;
 *   5) оценка вне диапазона [2; 5]               — invalid_argument;
 *   6) повторная сдача уже сданной работы        — logic_error;
 *   7) недопустимое значение сложности           — invalid_argument.
 * Дополнительно запрос средней оценки при отсутствии сданных работ
 * приводит к исключению runtime_error (это ошибка состояния объекта,
 * а не ввода, поэтому в число шести обязательных не включена).
 *
 * Класс владеет динамической памятью, поэтому помимо деструктора
 * определены конструктор копирования и оператор присваивания
 * («правило трёх»): без них копия объекта разделяла бы массивы
 * с оригиналом, что привело бы к двойному освобождению памяти.
 * ================================================================== */

#ifndef LABACCOUNTING_H
#define LABACCOUNTING_H

#include <iostream>
#include <string>

/* ---------- Перечисление дисциплин ----------
 * Оставлено вне класса: дисциплина — общее понятие предметной
 * области, а не часть учёта сдачи работ. В последующих работах
 * оно используется и другими классами.                          */
enum class Discipline {
    PROGRAMMING = 0,
    DATABASES,
    NETWORKS,
    MATH_ANALYSIS,
    PHYSICS,
    COUNT              /* служебное значение: количество дисциплин */
};

/* Название дисциплины на русском языке */
std::string DisciplineToString(const Discipline &discipline);

/* Форматирование вещественного числа для вывода:
 * std::to_string(2.0) дало бы "2.000000", что выглядит неряшливо. */
std::string FormatNumber(double value, int precision = 1);

/* ==================================================================
 * Класс LabAccounting
 * ================================================================== */
class LabAccounting {
public:
    /* ---------- Перечисление сложности работы ----------
     * Объявлено внутри класса: сложность имеет смысл только как
     * характеристика лабораторной работы в этом учёте. Снаружи
     * к нему обращаются по полному имени: LabAccounting::Difficulty.
     * Значения идут подряд (1, 2, 3) — на этом основана проверка
     * в ValidateDifficulty().                                      */
    enum class Difficulty {
        EASY = 1,
        MEDIUM = 2,
        HARD = 3
    };
    std::string DisciplineToString(const Discipline &discipline)
{
    switch (discipline) {
        case Discipline::PROGRAMMING:   return "Программирование"; // oui je dois remettre ca dans static const string
        case Discipline::DATABASES:     return "Базы данных";
        case Discipline::NETWORKS:      return "Компьютерные сети";
        case Discipline::MATH_ANALYSIS: return "Математический анализ";
        case Discipline::PHYSICS:       return "Физика";
        default:                        return "Неизвестная дисциплина";
    }
}


    inline static const double       MIN_GRADE = 2.0;      /* минимальная оценка           */
    inline static const double       MAX_GRADE = 5.0;      /* максимальная оценка          */
    inline static const double       NOT_GRADED = -1.0;     /* признак несданной работы     */
    inline static const unsigned int MAX_LAB_COUNT = 100;  /* разумный предел числа работ  */

   
    static std::string DifficultyToString(const Difficulty &difficulty);

    /* ---------- Конструктор, правило трёх, деструктор ---------- */
    // faire comme ca revient a faire un constructeur par defaut et un constructeur avec parametr
    LabAccounting(Discipline discipline, int labCount);
    LabAccounting(const LabAccounting &other);
    LabAccounting& operator=(const LabAccounting &other);
    ~LabAccounting();

    /* ---------- Селекторы на все поля, кроме массивов ---------- */
    const Discipline& GetDiscipline() const;
    unsigned int      GetLabCount() const;
    double            GetCurrentGrade() const;
    unsigned int      GetDoneCount() const;

    /* ---------- Методы ---------- */
    void SubmitLab(unsigned int number, const double &grade);
    void SetDifficulty(unsigned int number, const Difficulty &difficulty);
    void Print(std::ostream &out) const;

private:
    Discipline   discipline_;
    unsigned int labCount_;
    double      *grades_;
    Difficulty  *difficulty_;
    double       currentGrade_;
    unsigned int doneCount_;

    /* Пересчитывает число сданных работ и среднюю оценку.
     * Вызывается в момент изменения оценок, поэтому селекторы
     * лишь возвращают готовые значения.                          */
    void RecalculateStats();

    /* Проверки входных данных */
    void        ValidateIndex(unsigned int number) const;
    static void ValidateDiscipline(const Discipline &discipline);
    static void ValidateLabCount(int labCount);
    static void ValidateDifficulty(const Difficulty &difficulty);

    /* Выделяет пару массивов заданного размера. Если второе
     * выделение не удастся, первый массив освобождается, чтобы
     * не было утечки памяти.                                     */
    static void AllocateArrays(unsigned int count, 
                               double      *&grades, 
                               Difficulty  *&difficulty);
};

#endif /* LABACCOUNTING_H */
