/* ==================================================================
 * Лабораторная работа №2. Базовый синтаксис классов
 * Файл: LabAccounting.cpp — реализация методов класса LabAccounting
 * ================================================================== */

#include "LabAccounting.h"

#include <algorithm>   /* std::copy, std::fill */
#include <sstream>
#include <iomanip>
#include <stdexcept>

/* ---------- Определение статических константных полей ---------- 
const double       LabAccounting::MIN_GRADE     = 2.0; // et ca aussi ave c inline static const double MIN_GRADE = 2.0; dans le .h
const double       LabAccounting::MAX_GRADE     = 5.0;
const double       LabAccounting::NOT_GRADED    = -1.0;
const unsigned int LabAccounting::MAX_LAB_COUNT = 50;

/* ==================================================================
 * Свободные вспомогательные функции
 * ================================================================== */
/**/
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
?
std::string FormatNumber(double value, int precision)
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value; // чтобы округлить до нужной точности. через stream потому что, чтобы не было ошибок при округлении.
    return stream.str();
}

/* ==================================================================
 * Статические методы класса
 * ================================================================== */

std::string LabAccounting::DifficultyToString(const Difficulty &difficulty)
{
    switch (difficulty) {
        case Difficulty::EASY:   return "лёгкая";
        case Difficulty::MEDIUM: return "средняя";
        case Difficulty::HARD:   return "сложная";
        default:                 return "не задана";
    }
}

void LabAccounting::ValidateDiscipline(const Discipline &discipline)
{
    /* Исключение 1: значение вне перечисления */
    if (discipline < Discipline::PROGRAMMING || discipline >= Discipline::COUNT) {
        throw std::invalid_argument("недопустимое значение дисциплиныюю.");
    }
}

void LabAccounting::ValidateLabCount(int labCount)
{
   
    if (labCount <= 0) {
        throw std::invalid_argument("количество лабораторных работ должно быть "
                                    "больше нуля.");
    }
    /* Исключение 3: неправдоподобно большое количество работ.
     * Без этой проверки по запросу пользователя была бы сделана
     * попытка выделить память огромного объёма.               */
    if (static_cast<unsigned int>(labCount) > MAX_LAB_COUNT) {
        throw std::out_of_range("количество лабораторных работ не может превышать " +
                                std::to_string(MAX_LAB_COUNT) + ".");
    }
}

void LabAccounting::ValidateDifficulty(const Difficulty &difficulty)
{
    /* Исключение 7: значение вне перечисления Difficulty.
     * Сравнение через < и > допустимо, потому что значения
     * перечисления идут подряд: EASY = 1, MEDIUM = 2, HARD = 3. */
    if (difficulty < Difficulty::EASY || difficulty > Difficulty::HARD) {
        throw std::invalid_argument("недопустимое значение сложности работы.");
    }
}

void LabAccounting::AllocateArrays(unsigned int count, double *&grades,
                                   Difficulty *&difficulty)
{
    grades = new double[count];
    try {
        difficulty = new Difficulty[count];
    } catch (...) {
        /* Второй массив выделить не удалось — освобождаем первый
         * и передаём исключение дальше.                          */
        delete[] grades;
        grades = nullptr;
        throw;
    }
}

/* ==================================================================
 * Закрытые методы
 * ================================================================== */

void LabAccounting::ValidateIndex(unsigned int number) const
{
    /* Исключение 4: номер работы вне диапазона.
     * Номер беззнаковый, поэтому отрицательным быть не может;
     * отрицательное число, переданное снаружи, превратится в очень
     * большое и будет отвергнуто второй частью условия.            */
    if (number == 0 || number > labCount_) {
        throw std::out_of_range("номер лабораторной работы должен быть в диапазоне 1.." +
                                std::to_string(labCount_) + ".");
    }
}

void LabAccounting::RecalculateStats()
{
    unsigned int done = 0;
    double sum = 0.0;
    for (unsigned int i = 0; i < labCount_; ++i) {
        if (grades_[i] != NOT_GRADED) {
            ++done;
            sum += grades_[i];
        }
    }
    doneCount_ = done;
    currentGrade_ = (done > 0) ? (sum / done) : 0.0;
}

/* ==================================================================
 * Конструктор
 * ================================================================== */

/* Параметр labCount имеет знаковый тип int намеренно: если бы он был
 * беззнаковым, введённое пользователем -5 превратилось бы в число
 * 4294967291 и пользователь получил бы непонятное сообщение «больше
 * допустимого» вместо точного «должно быть больше нуля». После
 * проверки значение сохраняется в беззнаковое поле.                 */
LabAccounting::LabAccounting(Discipline discipline, int labCount)
    : discipline_   (discipline),
      labCount_     (0),
      grades_       (nullptr),
      difficulty_   (nullptr),
      currentGrade_ (0.0),
      doneCount_    (0)
{
    /* Проверки выполняются до выделения памяти, чтобы при
     * некорректных данных объект вообще не был создан.      */
    ValidateDiscipline(discipline);
    ValidateLabCount(labCount);

    labCount_ = static_cast<unsigned int>(labCount);
    AllocateArrays(labCount_, grades_, difficulty_);

    std::fill(grades_, grades_ + labCount_, NOT_GRADED);
    std::fill(difficulty_, difficulty_ + labCount_, Difficulty::MEDIUM);
}

/* ==================================================================
 * Правило трёх
 * ================================================================== */

/* Конструктор копирования: глубокое копирование — для копии
 * выделяются собственные массивы, а не используются чужие.    */
LabAccounting::LabAccounting(const LabAccounting &other)
    : discipline_   (other.discipline_),
      labCount_     (other.labCount_),
      grades_       (nullptr),
      difficulty_   (nullptr),
      currentGrade_ (other.currentGrade_),
      doneCount_    (other.doneCount_)
{
    AllocateArrays(labCount_, grades_, difficulty_);

    /* std::copy вместо поэлементного цикла: короче, работает для
     * любых типов элементов, а для простых типов компилятор сам
     * превращает его в быстрое копирование блока памяти.        */
    std::copy(other.grades_, other.grades_ + labCount_, grades_);
    std::copy(other.difficulty_, other.difficulty_ + labCount_, difficulty_);
}

/* Оператор присваивания */
LabAccounting& LabAccounting::operator=(const LabAccounting &other)
{
    if (this == &other) {
        return *this;   /* защита от самоприсваивания: a = a */
    }

    /* Если размеры массивов совпадают, повторно выделять память не
     * нужно — достаточно скопировать значения в уже имеющиеся
     * массивы. Новая память выделяется, только когда размер другой. */
    if (labCount_ != other.labCount_) {
        /* Сначала выделяем новую память и лишь затем освобождаем
         * старую: если выделение не удастся, объект останется
         * в прежнем корректном состоянии.                        */
        double *newGrades = nullptr;
        Difficulty *newDifficulty = nullptr;
        AllocateArrays(other.labCount_, newGrades, newDifficulty);

        delete[] grades_;
        delete[] difficulty_;

        grades_ = newGrades;
        difficulty_ = newDifficulty;
        labCount_ = other.labCount_;
    }

    std::copy(other.grades_, other.grades_ + labCount_, grades_);
    std::copy(other.difficulty_, other.difficulty_ + labCount_, difficulty_);

    discipline_ = other.discipline_;
    currentGrade_ = other.currentGrade_;
    doneCount_ = other.doneCount_;

    return *this;
}

/* Деструктор: освобождение массивов */
LabAccounting::~LabAccounting()
{
    delete[] grades_;
    delete[] difficulty_;
}

/* ==================================================================
 * Селекторы
 * ================================================================== */

const Discipline& LabAccounting::GetDiscipline() const
{
    return discipline_;
}

unsigned int LabAccounting::GetLabCount() const
{
    return labCount_;
}

double LabAccounting::GetCurrentGrade() const
{
    if (doneCount_ == 0) {
        throw std::runtime_error("ни одна лабораторная работа ещё не сдана, "
                                 "средняя оценка не определена.");
    }
    return currentGrade_;
}

unsigned int LabAccounting::GetDoneCount() const
{
    return doneCount_;
}

/* ==================================================================
 * Открытые методы
 * ================================================================== */

void LabAccounting::SubmitLab(unsigned int number, const double &grade)
{
    ValidateIndex(number);

    /* Исключение 5: оценка вне допустимого диапазона */
    if (grade < MIN_GRADE || grade > MAX_GRADE) {
        throw std::invalid_argument("оценка должна быть в диапазоне " +
                                    FormatNumber(MIN_GRADE) + ".." +
                                    FormatNumber(MAX_GRADE) + ".");
    }
    /* Исключение 6: работа уже была сдана ранее */
    if (grades_[number - 1] != NOT_GRADED) {
        throw std::logic_error("лабораторная работа №" + std::to_string(number) +
                               " уже была сдана, повторная сдача невозможна.");
    }

    grades_[number - 1] = grade;

    /* Оценки изменились — сразу обновляем среднюю оценку и число
     * сданных работ. Это единственное место, где меняется массив
     * оценок, поэтому значения в полях всегда актуальны.          */
    RecalculateStats();
}

void LabAccounting::SetDifficulty(unsigned int number, const Difficulty &difficulty)
{
    ValidateIndex(number);
    ValidateDifficulty(difficulty);
    difficulty_[number - 1] = difficulty;
}

void LabAccounting::Print(std::ostream &out) const
{
    out << "================================================================\n";
    out << " Дисциплина:            " << ::DisciplineToString(discipline_) << "\n";
    out << " Всего лабораторных:    " << labCount_ << "\n";
    out << " Сдано лабораторных:    " << doneCount_ << " из " << labCount_ << "\n";
    out << " Текущая оценка:        ";
    if (doneCount_ > 0) {
        out << FormatNumber(currentGrade_, 2) << "\n";
    } else {
        out << "(нет сданных работ)\n";
    }
    out << "----------------------------------------------------------------\n";
    for (unsigned int i = 0; i < labCount_; ++i) {
        out << " Работа №" << (i + 1)
            << " | сложность: " << DifficultyToString(difficulty_[i]);
        if (grades_[i] != NOT_GRADED) {
            out << " | оценка: " << FormatNumber(grades_[i], 1) << "\n";
        } else {
            out << " | не сдана\n";
        }
    }
    out << "================================================================\n";
}
