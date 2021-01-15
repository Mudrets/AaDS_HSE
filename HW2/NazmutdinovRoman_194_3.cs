using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml.Schema;

namespace CHW1
{

    class Table
    {
        protected Dictionary<string, Array> columns = new Dictionary<string, Array>();
        protected Dictionary<string, Type> types;
        protected int countOfStrings = 0;

        /// <summary>
        /// Создает пустую таблицу
        /// </summary>
        public Table() { }

        /// <summary>
        /// Создает таблицу с колонкой под названием columnName
        /// и массивом значений values
        /// </summary>
        /// <param name="values">значения</param>
        /// <param name="columnName">названия колонки</param>
        public Table(List<string> values, string columnName)
        {
            countOfStrings = values.Count();
            columns.Add(columnName, values.ToArray());
        }

        /// <summary>
        /// Создает пустую колонку нижного типа
        /// </summary>
        /// <param name="columnName">Название колонки</param>
        /// <param name="countOfLines">Количество строк в колонке</param>
        protected void CreateColumns(string columnName, int countOfLines)
        {
            if (!columns.Keys.Contains(columnName))
            {
                if (types[columnName].Equals(typeof(int)))
                {
                    columns.Add(columnName, new int[countOfLines]);
                }
                else if (types[columnName].Equals(typeof(short)))
                {
                    columns.Add(columnName, new short[countOfLines]);
                }
                else if (types[columnName].Equals(typeof(byte)))
                {
                    columns.Add(columnName, new byte[countOfLines]);
                }
                else if (types[columnName].Equals(typeof(string)))
                {
                    columns.Add(columnName, new string[countOfLines]);
                }
            }
        }

        /// <summary>
        /// Заполняет колнку значениями колонки с названием columnName
        /// из строкового предтавления таблицы allTable
        /// </summary>
        /// <param name="columnName">название столбца</param>
        /// <param name="allTable">строковое представление всей таблицы</param>
        protected void FillInColumn(string columnName, string[] allTable)
        {
            int indexOfColumn = IndexOfName(columnName);
            if (indexOfColumn == -1)
            {
                throw new ArgumentException("The column does not exist");
            }
            for (int i = 0; i < allTable.Length; i++)
            {
                string[] values = allTable[i].Split('|');
                AddElem(columnName, i, values[indexOfColumn]);
            }
        }

        /// <summary>
        /// Заполняет всю колонку с названием columnName 
        /// значениями из массива allColumn
        /// </summary>
        /// <param name="columnName"></param>
        /// <param name="allColumn"></param>
        protected void FillInColumnFromColumn(string columnName, string[] allColumn)
        {
            for (int i = 0; i < allColumn.Length; i++)
            {
                AddElem(columnName, i, allColumn[i]);
            }
        }

        /// <summary>
        /// Добавляет элемент в таблицу
        /// </summary>
        /// <param name="columnName"></param>
        /// <param name="index"></param>
        /// <param name="value"></param>
        private void AddElem(string columnName, int index, string value)
        {
            if (types[columnName].Equals(typeof(int)))
            {
                columns[columnName].SetValue(int.Parse(value), index);
            }
            else if (types[columnName].Equals(typeof(short)))
            {
                columns[columnName].SetValue(short.Parse(value), index);
            }
            else if (types[columnName].Equals(typeof(byte)))
            {
                columns[columnName].SetValue(byte.Parse(value), index);
            }
            else if (types[columnName].Equals(typeof(string)))
            {
                columns[columnName].SetValue(value, index);
            }
        }

        /// <summary>
        /// Возвращает индекс колонки в таблице по ее имени. В случае если
        /// ее нет, то возвращает -1
        /// </summary>
        /// <param name="columnName">Имя колонки</param>
        /// <returns>индекс колонки с именем columnName</returns>
        private int IndexOfName(string columnName)
        {
            int i = 0;
            foreach (string name in types.Keys)
            {
                if (name.Equals(columnName))
                {
                    return i;
                }
                i++;
            }
            return -1;
        }

        /// <summary>
        /// Определяет индексы строчек, которые удововлетворяют фильтру
        /// </summary>
        /// <param name="columnName">колонка по которой производится фильтрация</param>
        /// <param name="filter">фильтр</param>
        /// <returns>Индексы подходящих строк</returns>
        public List<int> GetListIndexes(string columnName, Predicate<object> filter)
        {
            List<int> indexes = new List<int>();
            int i = 0;
            foreach (object value in columns[columnName])
            {
                if (filter(value))
                {
                    indexes.Add((int)columns.Values.ElementAt(0).GetValue(i));
                }
                i++;
            }
            return indexes;
        }

        /// <summary>
        /// составляет RoaringBitmap по листу подходящих индексов, проходясь
        /// по колонке расположенной по пути columnPath
        /// </summary>
        /// <param name="columnPath">путь к колонке</param>
        /// <param name="indexes">лист подходящих индексов</param>
        /// <returns>RoaringBitmap</returns>
        public static RoaringBitmap GetBitmap(string columnPath, List<int> indexes)
        {
            string[] column = File.ReadAllLines(columnPath);
            int sizeOfArr = column.Length % 64 == 0 ? column.Length / 64 : column.Length / 64 + 1;
            ulong[] bits64Arr = new ulong[sizeOfArr];
            for (int i = 0; i < sizeOfArr; i++)
            {
                ulong bits64 = 0;
                for (int j = 0; j < 64; j++)
                {
                    if (i * 64 + j >= column.Length)
                    {
                        break;
                    }
                    int intValue;
                    if (int.TryParse(column[i * 64 + j], out intValue))
                    {
                        bits64 += indexes.Contains(intValue) ? (ulong)Math.Pow(2, j) : 0;
                    }
                    else
                    {
                        throw new ArgumentException("Incorrect type of column");
                    }
                }
                bits64Arr[i] = bits64;
            }
            return new RoaringBitmap(bits64Arr);
        }

        /// <summary>
        /// Составляет RoaringBitmap по результатам фльтра
        /// </summary>
        /// <param name="filter">фильтер</param>
        /// <returns></returns>
        public RoaringBitmap GetBitmap(Predicate<object> filter)
        {
            int columnSize = columns.Values.ElementAt(0).Length;
            int sizeOfArr = columnSize % 64 == 0 ? columnSize / 64 : columnSize / 64 + 1;
            ulong[] bits64Arr = new ulong[sizeOfArr];
            for (int i = 0; i < sizeOfArr; i++)
            {
                ulong bits64 = 0;
                for (int j = 0; j < 64; j++)
                {
                    if (i * 64 + j >= columnSize)
                    {
                        break;
                    }
                    bits64 += filter(columns.Values.ElementAt(0).GetValue(i * 64 + j)) ? (ulong)Math.Pow(2, j) : 0;
                }
                bits64Arr[i] = bits64;
            }
            return new RoaringBitmap(bits64Arr);
        }

        /// <summary>
        /// Соединяет две таблицы в одну, склеивая их столбцы
        /// </summary>
        /// <param name="table1"></param>
        /// <param name="table2"></param>
        /// <returns></returns>
        public static Table operator +(Table table1, Table table2)
        {
            Table resultTable = new Table();
            int countOfColumn = table1.columns.Count;
            foreach (string columnName in table1.columns.Keys)
            {
                resultTable.columns.Add(columnName, table1.columns[columnName]);
            }
            foreach (string columnName in table2.columns.Keys)
            {
                if (!resultTable.columns.Keys.Contains(columnName))
                {
                    resultTable.columns.Add(columnName, table2.columns[columnName]);
                }
                else
                {
                    resultTable.columns.Add(columnName + countOfColumn, table2.columns[columnName]);
                }
            }
            resultTable.countOfStrings = Math.Max(table1.countOfStrings, table2.countOfStrings);
            return resultTable;
        }

        /// <summary>
        /// Возвращает тип колонки columnName
        /// </summary>
        /// <param name="columnName">название колонки</param>
        /// <returns></returns>
        public Type GetTypeOfColumn(string columnName)
        {
            if (!types.ContainsKey(columnName))
            {
                throw new ArgumentException("Column does not exist");
            }
            return types[columnName];
        }

        /// <summary>
        /// Выводит подходящие под bitmap строки таблицы, разделяя
        /// их \n
        /// </summary>
        /// <param name="bitmap"></param>
        /// <returns></returns>
        public string ToString(RoaringBitmap bitmap)
        {
            string result = string.Empty;
            for (int i = 0; i < countOfStrings; i++)
            {
                if (bitmap.Get(i))
                {
                    List<string> values = new List<string>();
                    foreach (Array arrays in columns.Values)
                    {
                        values.Add(arrays.GetValue(i).ToString());
                    }
                    result += $"{string.Join("|", values)}\n";
                }
            }
            return result;
        }

        /// <summary>
        /// Заменяет индексы на значения колонки
        /// </summary>
        /// <param name="factKeyColumn">колонка с индексами</param>
        /// <param name="keyValueTable">колонка со значениями</param>
        /// <returns>Колонка со значениями вместо индексов</returns>
        public static List<string> GetFactValuesColumn(Table factKeyColumn, Table keyValueTable)
        {
            List<string> values = new List<string>();
            int valuesColumnIndex = keyValueTable.columns.Keys.Count() - 1;
            string valueColumnName = keyValueTable.columns.Keys.ElementAt(valuesColumnIndex);
            foreach (object index in factKeyColumn.columns.First().Value)
            {
                int lineNum = Array.BinarySearch(keyValueTable.columns.First().Value, index);
                values.Add(keyValueTable.columns[valueColumnName].GetValue(lineNum).ToString());
            }
            return values;
        }
    }


    class FactResellerSales : Table
    {
        /// <summary>
        /// Добавляет колонку из файла по пути path
        /// </summary>
        /// <param name="path">путь к колонке</param>
        public void AddColumn(string path)
        {
            IniinitializeTypes();
            string[] allColumn = File.ReadAllLines(path);
            countOfStrings = countOfStrings < allColumn.Length ? allColumn.Length : countOfStrings;
            string[] directories = path.Split('/');
            string columnName = directories[directories.Length - 1].Split('.')[1];
            CreateColumns(columnName, allColumn.Length);
            FillInColumnFromColumn(columnName, allColumn);
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "ProductKey", typeof(int) },
                { "OrderDateKey", typeof(int) },
                { "ResellerKey", typeof(int) },
                { "EmployeeKey", typeof(int) },
                { "PromotionKey", typeof(int) },
                { "CurrencyKey", typeof(int) },
                { "SalesTerritoryKey", typeof(int) },
                { "SalesOrderNumber", typeof(string) },
                { "SalesOrderLineNumber", typeof(byte) },
                { "OrderQuantity", typeof(short) },
                { "CarrierTrackingNumber", typeof(string) },
                { "CustomerPONumber", typeof(string) },
            };
        }
    }

    class DimProduct : Table
    {
        /// <summary>
        /// Создает таблицу типа DimProduct
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimProduct(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "ProductKey", typeof(int) },
                { "ProductAlternateKey", typeof(string) },
                { "EnglishProductName", typeof(string) },
                { "Color", typeof(string) },
                { "SafetyStockLevel", typeof(short) },
                { "ReorderPoint", typeof(short) },
                { "SizeRange", typeof(string) },
                { "DaysToManufacture", typeof(int) },
                { "StartDate", typeof(string) }
            };
        }
    }

    class DimReseller : Table
    {
        /// <summary>
        /// Создает таблицу типа DimReseller
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimReseller(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "ResellerKey", typeof(int) },
                { "ResellerAlternateKey", typeof(string) },
                { "Phone", typeof(string) },
                { "BusinessType", typeof(string) },
                { "ResellerName", typeof(string) },
                { "NumberEmployees", typeof(int) },
                { "OrderFrequency", typeof(string) },
                { "ProductLine", typeof(string) },
                { "AddressLine1", typeof(string) },
                { "BankName", typeof(string) },
                { "YearOpened", typeof(int) }
            };
        }
    }

    class DimCurrency : Table
    {
        /// <summary>
        /// Создает таблицу типа DimCurrency
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimCurrency(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "CurrencyKey", typeof(int) },
                { "CurrencyAlternateKey", typeof(string) },
                { "CurrencyName", typeof(string) },
            };
        }
    }

    class DimPromotion : Table
    {
        /// <summary>
        /// Создает таблицу типа DimPromotion
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimPromotion(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "PromotionKey", typeof(int) },
                { "PromotionAlternateKey", typeof(int) },
                { "EnglishPromotionName", typeof(string) },
                { "EnglishPromotionType", typeof(string) },
                { "EnglishPromotionCategory", typeof(string) },
                { "StartDate", typeof(string) },
                { "EndDate", typeof(string) },
                { "MinQty", typeof(int) },
            };
        }
    }

    class DimSalesTerritory : Table
    {
        /// <summary>
        /// Создает таблицу типа DimSalesTerritory
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimSalesTerritory(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "SalesTerritoryKey", typeof(int) },
                { "SalesTerritoryAlternateKey", typeof(int) },
                { "SalesTerritoryRegion", typeof(string) },
                { "SalesTerritoryCountry", typeof(string) },
                { "SalesTerritoryGroup", typeof(string) },
            };
        }
    }

    class DimEmployee : Table
    {
        /// <summary>
        /// Создает таблицу типа DimEmployee
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimEmployee(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "EmployeeKey", typeof(int) },
                { "FirstName", typeof(string) },
                { "LastName", typeof(string) },
                { "Title", typeof(string) },
                { "BirthDate", typeof(string) },
                { "LoginID", typeof(string) },
                { "EmailAddress", typeof(string) },
                { "Phone", typeof(string) },
                { "MaritalStatus", typeof(string) },
                { "Gender", typeof(string) },
                { "PayFrequency", typeof(byte) },
                { "VacationHours", typeof(short) },
                { "SickLeaveHours", typeof(short) },
                { "DepartmentName", typeof(string) },
                { "StartDate", typeof(string) },
            };
        }
    }

    class DimDate : Table
    {

        /// <summary>
        /// Создает таблицу типа DimDate
        /// </summary>
        /// <param name="path">путь к файле с таблицей</param>
        /// <param name="columnsNames">название ячеек, которые нужно добавить</param>
        public DimDate(string path, string columnsNames)
        {
            IniinitializeTypes();
            string[] allTabel = File.ReadAllLines(path);
            countOfStrings = allTabel.Length;
            string[] columnsNameArr = columnsNames.Split(' ');
            foreach (string columnName in columnsNameArr)
            {
                CreateColumns(columnName, allTabel.Length);
                FillInColumn(columnName, allTabel);
            }
        }

        /// <summary>
        /// Инициализирует словарь типов
        /// </summary>
        private void IniinitializeTypes()
        {
            types = new Dictionary<string, Type>() {
                { "DateKey", typeof(int) },
                { "FullDateAlternateKey", typeof(string) },
                { "DayNumberOfWeek", typeof(byte) },
                { "EnglishDayNameOfWeek", typeof(string) },
                { "DayNumberOfMonth", typeof(byte) },
                { "DayNumberOfYear", typeof(short) },
                { "WeekNumberOfYear", typeof(byte) },
                { "EnglishMonthName", typeof(string) },
                { "MonthNumberOfYear", typeof(byte) },
                { "CalendarQuarter", typeof(byte) },
                { "CalendarYear", typeof(short) },
                { "CalendarSemester", typeof(byte) },
                { "FiscalQuarter", typeof(byte) },
                { "FiscalYear", typeof(short) },
                { "FiscalSemester", typeof(byte) },
            };
        }
    }

    public static class ArrExtension
    {
        /// <summary>
        /// Добавляет эелемент в массив ирасширяет его
        /// </summary>
        /// <param name="arr"></param>
        /// <param name="newElem"></param>
        public static void Add(this ushort[] arr, ushort newElem)
        {
            Array.Resize(ref arr, arr.Length + 1);
            arr[arr.Length - 1] = newElem;
        }

        /// <summary>
        /// Проверяет содержится ли элемент в массиве
        /// </summary>
        /// <param name="arr"></param>
        /// <param name="elem"></param>
        /// <returns></returns>
        public static bool Contains(this ushort[] arr, ushort elem)
        {
            for (int i = 0; i < arr.Length; i++)
            {
                if (arr[i] == elem)
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// Получает индекс элемента в массиве. В случае если элемент отсутствует
        /// в массиве возвращает -1
        /// </summary>
        /// <param name="arr"></param>
        /// <param name="elem"></param>
        /// <returns></returns>
        public static int IndexOf(this ushort[] arr, ushort elem)
        {
            if (arr.Contains(elem))
            {
                for (int i = 0; i < arr.Length; i++)
                {
                    if (arr[i] == elem)
                    {
                        return i;
                    }
                }
            }
            return -1;
        }

        /// <summary>
        /// Удаляет элемент из массива
        /// </summary>
        /// <param name="arr"></param>
        /// <param name="elem"></param>
        public static void Remove(this ushort[] arr, ushort elem)
        {
            if (arr.Contains(elem))
            {
                int indexElem = arr.IndexOf(elem);
                for (int i = indexElem; i < arr.Length - 1; i++)
                {
                    arr[i] = arr[i + 1];
                }
                Array.Resize(ref arr, arr.Length - 1);
            }
        }
    }

    abstract class Bitmap
    {
        protected const int POWER16 = 65536;
        public ulong cardinality;

        public abstract void And(Bitmap other);
        public abstract void Set(int i, bool value);
        public abstract bool Get(int i);
    }

    class BitmapContainer : Bitmap
    {
        public ulong[] bits64; //массив 64-битных чисел, хранящих 64 значения

        /// <summary>
        /// Создает пустой контейнер 
        /// </summary>
        public BitmapContainer(bool empty)
        {
            bits64 = new ulong[1024];
            if (!empty)
            {
                for (int i = 0; i < 1024; i++)
                {
                    bits64[i] = ulong.MaxValue;
                    cardinality = 1024 * 64;
                }
            }
        }

        /// <summary>
        /// Записывает в this пересечение с other
        /// </summary>
        /// <param name="other"></param>
        public override void And(Bitmap other)
        {
            if (other is BitmapContainer)
            {
                ulong c = 0;
                BitmapContainer otherBitmap = (BitmapContainer)other;
                for (int i = 0; i < 1024; i++)
                {
                    bits64[i] &= otherBitmap.bits64[i];
                    ulong num = bits64[i];
                    for (int j = 0; j < 64; j++)
                    {
                        c += num % 2;
                        num /= 2;
                    }
                }
                cardinality = c;
            }
            if (other is ArrayContainer)
            {
                ArrayContainer thisContainer = new ArrayContainer(this);
                other.And(thisContainer);
            }
        }

        /// <summary>
        /// Возвращает значение бита в позиции i
        /// </summary>
        /// <param name="i">индекс</param>
        /// <returns></returns>
        public override bool Get(int i)
        {
            return bits64[i / 64] / (ulong)Math.Pow(2, i % 64) % 2 == 1;
        }

        /// <summary>
        /// Устанавливает в позицию i value
        /// </summary>
        /// <param name="i">индекс</param>
        /// <param name="value">значение</param>
        public override void Set(int i, bool value)
        {
            bool nowValue = Get(i);
            if (value && !nowValue)
            {
                cardinality++;
                bits64[i / 64] += (ulong)Math.Pow(2, i % 64);
            }
            else if (!value && nowValue)
            {
                cardinality--;
                bits64[i / 64] -= (ulong)Math.Pow(2, i % 64);
            }
        }
    }

    class ArrayContainer : Bitmap
    {
        public ushort[] arrayContainer; //хранит индексы единичных бит

        /// <summary>
        /// Создает пустой AarayContainer
        /// </summary>
        public ArrayContainer()
        {
            arrayContainer = new ushort[0];
        }

        /// <summary>
        /// Создает ArrayContainer по существующему BitmapContainer
        /// </summary>
        /// <param name="bitmapCon"></param>
        public ArrayContainer(BitmapContainer bitmapCon)
        {
            arrayContainer = new ushort[bitmapCon.cardinality];
            int k = 0;
            for (int i = 0; i < bitmapCon.bits64.Length; i++)
            {
                for (int j = 0; j < 64; j++)
                {
                    if (bitmapCon.Get(j + i * 64))
                    {
                        arrayContainer[k++] = ((ushort)(j + i * 64));
                    }
                }
            }
            cardinality = (ulong)arrayContainer.Length;
        }

        /// <summary>
        /// Пересекает два контейнера
        /// </summary>
        /// <param name="other"></param>
        public override void And(Bitmap other)
        {
            if (other is BitmapContainer)
            {
                ArrayContainer otherArrayContainer = new ArrayContainer((BitmapContainer)other);
                And(otherArrayContainer);
                cardinality = (ulong)arrayContainer.Length;
            }
            else if (other is ArrayContainer)
            {
                ArrayContainer otherArrayCont = (ArrayContainer)other;

                arrayContainer = Intersect(arrayContainer, otherArrayCont.arrayContainer);
                cardinality = (ulong)arrayContainer.Length;
            }
        }

        /// <summary>
        /// Получает значение бита по индексу i
        /// </summary>
        /// <param name="i"></param>
        /// <returns></returns>
        public override bool Get(int i)
        {
            return arrayContainer.Contains((ushort)i);
        }

        /// <summary>
        /// Устанавливает значение value по индексу i
        /// </summary>
        /// <param name="i"></param>
        /// <param name="value"></param>
        public override void Set(int i, bool value)
        {
            if (value && !arrayContainer.Contains((ushort)i))
            {
                cardinality++;
                arrayContainer.Add((ushort)i);
            }
            else if (!value && arrayContainer.Contains((ushort)i))
            {
                cardinality--;
                arrayContainer.Remove((ushort)i);
            }
        }

        /// <summary>
        /// Пересекает два множества представленных массивами
        /// </summary>
        /// <param name="arr1"></param>
        /// <param name="arr2"></param>
        /// <returns>пересечение множеств</returns>
        private static ushort[] Intersect(ushort[] arr1, ushort[] arr2)
        {
            int minLength = Math.Min(arr1.Length, arr2.Length);
            ushort[] intersection = new ushort[minLength];
            int realLength = 0;
            for (int i = 0; i < arr1.Length; i++)
            {
                for (int j = 0; j < arr2.Length; j++)
                {
                    if (arr1[i] == arr2[j])
                    {
                        intersection[realLength++] = arr1[i];
                    }
                }
            }
            Array.Resize(ref intersection, realLength);
            return intersection;
        }

    }

    class RoaringBitmap : Bitmap
    {
        public Bitmap[] containers; //

        public RoaringBitmap(int countOfElems)
        {
            int countOfContainers = countOfElems / POWER16 + (countOfElems % POWER16 == 0 ? 0 : 1);
            containers = new Bitmap[countOfContainers];
        }

        public RoaringBitmap(ulong[] bits64)
        {
            int countOfElems = bits64.Length * 64;
            int countOfContainers = countOfElems / POWER16 + (countOfElems % POWER16 == 0 ? 0 : 1);
            containers = new Bitmap[countOfContainers];
            for (int i = 0; i < countOfContainers; i++)
            {
                //Заполняем контейнер
                int chankCardinality = 0;
                BitmapContainer bitmapCont = new BitmapContainer(true);
                for (int j = 0; j < 1024; j++)
                {
                    //Если эелементы кончились а контейнер не заполнен, то контейнер дозаполняется нулями
                    ulong num = i * 1024 + j < bits64.Length ? bits64[i * 1024 + j] : 0;
                    for (int k = 0; k < 64; k++)
                    {
                        chankCardinality += (int)(num % 2);
                        bitmapCont.Set(j * 64 + k, num % 2 == 1);
                        num /= 2;
                    }
                }
                //Записываем контейнер в массив
                if (chankCardinality < 4096)
                {
                    containers[i] = chankCardinality > 0 ? new ArrayContainer(bitmapCont) : new ArrayContainer();
                }
                else
                {
                    containers[i] = bitmapCont;
                }
            }

        }

        public override void And(Bitmap other)
        {
            if (other is RoaringBitmap)
            {
                RoaringBitmap roaringBitmapOther = (RoaringBitmap)other;
                for (int i = 0; i < roaringBitmapOther.containers.Count(); i++)
                {
                    if (roaringBitmapOther.containers[i] is BitmapContainer)
                    {
                        containers[i].And(roaringBitmapOther.containers[i]);
                        if (containers[i].cardinality < 4096 && containers[i] is BitmapContainer)
                        {
                            containers[i] = new ArrayContainer((BitmapContainer)containers[i]);
                        }
                    }
                    else if (containers[i] is ArrayContainer)
                    {
                        containers[i].And(roaringBitmapOther.containers[i]);
                    }
                    else
                    {
                        containers[i].And(roaringBitmapOther.containers[i]);
                        ArrayContainer res = (ArrayContainer)roaringBitmapOther.containers[i];
                        containers[i] = res;
                    }
                }
            }
        }

        public override bool Get(int i)
        {
            return containers[i / (int)Math.Pow(2, 16)].Get(i % (int)Math.Pow(2, 16));
        }

        public override void Set(int i, bool value)
        {
            containers[i / (int)Math.Pow(2, 16)].Set(i % (int)Math.Pow(2, 16), value);
        }

        public static RoaringBitmap OnesRoaringBitmap(int countOfElem)
        {
            RoaringBitmap bitmap = new RoaringBitmap(countOfElem);
            for (int i = 0; i < bitmap.containers.Length; i++)
            {
                bitmap.containers[i] = new BitmapContainer(false);
            }
            return bitmap;
        }
    }

    class Program
    {

        static string pathToData;
        static string pathToTests;
        static string pathToAns;
        const string factTableName = "FactResellerSales";


        /// <summary>
        /// По строковому представлению операции возвращает функцию,
        /// сравнивающую два числа типа int 
        /// </summary>
        /// <param name="sign"></param>
        /// <returns></returns>
        public static Func<int, int, bool> identifySignInt(string sign)
        {
            if (sign == "<")
            {
                return (x, y) => x < y;
            }
            else if (sign == ">")
            {
                return (x, y) => x > y;
            }
            else if (sign == "<=")
            {
                return (x, y) => x <= y;
            }
            else if (sign == ">=")
            {
                return (x, y) => x >= y;
            }
            else if (sign == "<>")
            {
                return (x, y) => x != y;
            }
            else if (sign == "=")
            {
                return (x, y) => x == y;
            }
            throw new ArgumentException("Incorrect sign");
        }

        /// <summary>
        /// По строковому представлению операции возвращает функцию,
        /// сравнивающую два числа типа short 
        /// </summary>
        /// <param name="sign"></param>
        /// <returns></returns>
        public static Func<short, short, bool> identifySignShort(string sign)
        {
            if (sign == "<")
            {
                return (x, y) => x < y;
            }
            else if (sign == ">")
            {
                return (x, y) => x > y;
            }
            else if (sign == "<=")
            {
                return (x, y) => x <= y;
            }
            else if (sign == ">=")
            {
                return (x, y) => x >= y;
            }
            else if (sign == "<>")
            {
                return (x, y) => x != y;
            }
            else if (sign == "=")
            {
                return (x, y) => x == y;
            }
            throw new ArgumentException("Incorrect sign");
        }

        /// <summary>
        /// По строковому представлению операции возвращает функцию,
        /// сравнивающую два числа типа byte 
        /// </summary>
        /// <param name="sign"></param>
        /// <returns></returns>
        public static Func<byte, byte, bool> identifySignByte(string sign)
        {
            if (sign == "<")
            {
                return (x, y) => x < y;
            }
            else if (sign == ">")
            {
                return (x, y) => x > y;
            }
            else if (sign == "<=")
            {
                return (x, y) => x <= y;
            }
            else if (sign == ">=")
            {
                return (x, y) => x >= y;
            }
            else if (sign == "<>")
            {
                return (x, y) => x != y;
            }
            else if (sign == "=")
            {
                return (x, y) => x == y;
            }
            throw new ArgumentException("Incorrect sign");
        }

        /// <summary>
        /// Создает фильтр по его строковому представлению
        /// </summary>
        /// <param name="strFilter">строковое представление фильтра</param>
        /// <param name="columnType">тип колонки</param>
        /// <returns></returns>
        public static Predicate<object> GetPredicate(string strFilter, Type columnType)
        {
            string[] parts = strFilter.Split(' ');
            int intNum;
            short shortNum;
            byte byteNum;
            string sign = parts[1];
            string value;
            if (parts.Length > 3)
            {
                string[] valueParts = new string[parts.Length - 2];
                Array.Copy(parts, 2, valueParts, 0, parts.Length - 2);
                value = string.Join(" ", valueParts);
            }
            else
            {
                value = parts[2];
            }
            if (value.Contains('\'') && columnType.Equals(typeof(string)))
            {
                value = value.Trim('\'');
                if (sign == "<>")
                {
                    return str => !value.Equals(str);
                }
                else if (sign == "=")
                {
                    return str => value.Equals(str);
                }
            }
            else if (int.TryParse(value, out intNum) && columnType.Equals(typeof(int)))
            {
                return x => identifySignInt(sign)((int)x, intNum);
            }
            else if (short.TryParse(value, out shortNum) && columnType.Equals(typeof(short)))
            {
                return x => identifySignShort(sign)((short)x, shortNum);
            }
            else if (byte.TryParse(value, out byteNum) && columnType.Equals(typeof(byte)))
            {
                return x => identifySignByte(sign)((byte)x, byteNum);
            }
            throw new ArgumentException("Incorrect filter");
        }

        /// <summary>
        /// Создает таблицу с одним или двумя столбцами (зависит от значения
        /// withKey) один из которых с именем fullColumnName. В случае если
        /// withKey = true, то добавляет столбец индексов таблицы
        /// </summary>
        /// <param name="fullColumnName">полное имя таблицы</param>
        /// <param name="withKey"></param>
        /// <returns>созданная таблица</returns>
        public static Table CreateTable(string fullColumnName, bool withKey)
        {
            string tableName = fullColumnName.Split('.')[0];
            string columnName = fullColumnName.Split('.')[1];
            switch (tableName)
            {
                case "DimProduct":
                    return new DimProduct($"{pathToData}/DimProduct.csv", $"{(withKey ? "ProductKey " : "")}{columnName}");
                case "DimReseller":
                    return new DimReseller($"{pathToData}/DimReseller.csv", $"{(withKey ? "ResellerKey " : "")}{columnName}");
                case "DimCurrency":
                    return new DimCurrency($"{pathToData}/DimCurrency.csv", $"{(withKey ? "CurrencyKey " : "")}{columnName}");
                case "DimPromotion":
                    return new DimPromotion($"{pathToData}/DimPromotion.csv", $"{(withKey ? "PromotionKey " : "")}{columnName}");
                case "DimSalesTerritory":
                    return new DimSalesTerritory($"{pathToData}/DimSalesTerritory.csv", $"{(withKey ? "SalesTerritoryKey " : "")}{columnName}");
                case "DimEmployee":
                    return new DimEmployee($"{pathToData}/DimEmployee.csv", $"{(withKey ? "EmployeeKey " : "")}{columnName}");
                case "DimDate":
                    return new DimDate($"{pathToData}/DimDate.csv", $"{(withKey ? "DateKey " : "")}{columnName}");
                case "FactResellerSales":
                    FactResellerSales table = new FactResellerSales();
                    table.AddColumn($"{pathToData}/{fullColumnName}.csv");
                    return table;
                default:
                    throw new ArgumentException("Table does not exist");
            }
        }

        /// <summary>
        /// Возвращает название колонки с кулчами по названию Dim таблицы
        /// </summary>
        /// <param name="tableName">название Dim таблицы</param>
        /// <returns></returns>
        public static string GetNameOfKeysColumn(string tableName)
        {
            if (tableName == "DimDate")
            {
                return "OrderDateKey";
            }
            return $"{tableName.Substring(3)}Key";
        }

        /// <summary>
        /// Создает таблицу значений по столбцу ключей
        /// </summary>
        /// <param name="fullColumnName"></param>
        /// <returns></returns>
        public static Table GetFactColumnWithValues(string fullColumnName)
        {
            string tableName = fullColumnName.Split('.')[0];
            string columnName = fullColumnName.Split('.')[1];
            string factKeysColumnName = GetNameOfKeysColumn(tableName);
            string factKeyColumn = $"{factTableName}.{factKeysColumnName}";
            Table keysColumn = CreateTable(factKeyColumn, false);
            Table keyValueTable = CreateTable(fullColumnName, true);
            List<string> values = Table.GetFactValuesColumn(keysColumn, keyValueTable);
            return new Table(values, columnName);

        }

        /// <summary>
        /// Реализует первую и вторую фазы Invisible Join
        /// </summary>
        /// <param name="filters"></param>
        /// <returns></returns>
        public static RoaringBitmap Phase1AndPhase2(string[] filters)
        {
            RoaringBitmap resBitmap = null;
            for (int i = 0; i < filters.Length; i++)
            {
                string tableAndColumn = filters[i].Split(' ')[0];
                string tableName = tableAndColumn.Split('.')[0];
                string columnName = tableAndColumn.Split('.')[1];
                Table table = CreateTable(tableAndColumn, true);
                Type columnType = table.GetTypeOfColumn(columnName);
                Predicate<object> filter = GetPredicate(filters[i], columnType);
                RoaringBitmap columnBitmap;
                if (tableName == factTableName)
                {
                    columnBitmap = table.GetBitmap(filter);
                }
                else
                {
                    List<int> indexes = table.GetListIndexes(columnName, filter);
                    string columnNameInFactTable = tableName == "DimDate" ? "OrderDateKey" : $"{tableName.Substring(3)}Key";
                    string pathToFactTableColumn = $"{pathToData}/{factTableName}.{columnNameInFactTable}.csv";
                    columnBitmap = Table.GetBitmap(pathToFactTableColumn, indexes);
                }
                resBitmap = resBitmap == null ? columnBitmap : resBitmap;
                    resBitmap.And(columnBitmap);
            }
            if (resBitmap == null)
            {
                int countLinesInTable = File.ReadAllLines($"{pathToData}/FactResellerSales.CarrierTrackingNumber.csv").Length;
                resBitmap = RoaringBitmap.OnesRoaringBitmap(countLinesInTable);
            }
            return resBitmap;
        }

        /// <summary>
        /// Реализует третью фазу Invisible Join
        /// </summary>
        /// <param name="neededColumns">массив полных названий нужных колонок</param>
        /// <returns></returns>
        public static Table Phase3(string[] neededColumns)
        {
            Table resultTable = new Table();
            for (int i = 0; i < neededColumns.Length; i++)
            {
                if (neededColumns[i].Split('.')[0] != factTableName)
                {
                    resultTable += GetFactColumnWithValues(neededColumns[i]);
                }
                else
                {
                    resultTable += CreateTable(neededColumns[i], false);
                }
            }
            return resultTable;
        }

        static void Main(string[] args)
        {
            if (args.Length != 3)
            {
                Console.WriteLine("Incorrect args");
            }
            else
            {
                pathToData = args[0].Trim('/');
                pathToTests = args[1];
                pathToAns = args[2];

                //Get input data
                string[] input = File.ReadAllLines(pathToTests);
                int countOfFilters = int.Parse(input[1]);
                string[] filters = new string[countOfFilters];
                //Invisible join
                Array.Copy(input, 2, filters, 0, input.Length - 2);
                RoaringBitmap resBitmap = Phase1AndPhase2(filters);
                string[] neededColumns = input[0].Split(',');
                Table resultTable = Phase3(neededColumns);
                //Print result
                string result = resultTable.ToString(resBitmap);
                File.WriteAllText(pathToAns, result);
            }
        }
    }
}
