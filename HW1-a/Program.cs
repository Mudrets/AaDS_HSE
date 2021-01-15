using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Collections;

namespace HomeWork
{
    class Point
    {
        //Координаты точки
        private int x;
        private int y;

        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        /// <summary>
        /// Предоставляет доступ к привытному полю _x
        /// </summary>
        public int X
        {
            get => x;
        }

        /// <summary>
        /// Предоставляет доступ к привытному полю _y
        /// </summary>
        public int Y
        {
            get => y;
        }

        /// <summary>
        /// Возвращает полярный угол точки
        /// </summary>
        /// <returns></returns>
        public double GetPolarAngle()
        {
            return Math.Acos(x/Math.Pow(x*x + y*y, 0.5));
        }

        /// <summary>
        /// Возвращает расстояние до точки point
        /// </summary>
        /// <param name="point"></param>
        /// <returns></returns>
        public double Distance(Point point)
        {
            return Math.Pow((x - point.X) * (x - point.X) + (y - point.Y) * (y - point.Y), 0.5);
        }

        /// <summary>
        /// Возвращает полярный угол точки относительно точки point
        /// </summary>
        /// <param name="point"></param>
        /// <returns></returns>
        public double GetPolarAngle(Point point)
        {
            Point newPoint = new Point(x - point.X, y - point.Y);
            return newPoint.GetPolarAngle();
        }

        public override string ToString()
        {
            return $"({x} {y})";
        }
    }

    class Stack<T> : IEnumerable<T>
    {
        private List<T> data = new List<T>();

        public const int maxAllowSize = 1000;
        public int maxSize;

        public Stack(int size)
        {
            if (size > maxAllowSize || size < 0)
            {
                throw new ArgumentException("Неверное значение размера стека");
            }

            maxSize = size;
        }

        public int Count { get => data.Count(); }

        public bool IsEmpty() => data.Count() == 0;

        public void Push(T value)
        {
            if (Count < maxSize)
            {
                data.Add(value);
            }
            else
            {
                throw new ArgumentException("Превышен допустимый размер стека");
            }
        }

        public T Pop()
        {
            if (!IsEmpty())
            {
                T lastElem = data.Last();
                data.RemoveAt(data.Count() - 1);
                return lastElem;
            }
            else
            {
                throw new ArgumentException("Стек пуст");
            }
        }

        public T Top()
        {
            if (!IsEmpty())
            {
                return data.Last();
            }
            else
            {
                throw new ArgumentException("Стек пуст");
            }
        }

        public T NextToTop()
        {
            if (Count < 2)
            {
                throw new ArgumentException("Невозможно получить предпоследний элемент");
            }
            else
            {
                return data[data.Count() - 2];
            }
        }

        public T First() => data[0];

        public IEnumerator<T> GetEnumerator()
        {
            return data.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }


    }


    class Program
    {

        /// <summary>
        /// Считывает информацию из файла с путем testPath
        /// </summary>
        /// <returns></returns>
        static List<Point> ReadAndFill(string path)
        {
            using (StreamReader sr = new StreamReader(path))
            {
                try
                {
                    uint n = uint.Parse(sr.ReadLine());
                    List<Point> points = new List<Point>();

                    for (int i = 0; i < n; i++)
                    {
                        string[] args = sr.ReadLine().Split(' ');
                        if (args.Length != 2)
                        {
                            throw new ArgumentException();
                        }
                        int x = int.Parse(args[0]);
                        int y = int.Parse(args[1]);
                        points.Add(new Point(x, y));
                    }

                    return points;
                }
                catch (IOException e)
                {
                    Console.WriteLine(e.Message);
                }
                catch (ArgumentException e)
                {
                    Console.WriteLine(e.Message);
                }
                catch (Exception e)
                {
                    Console.WriteLine(e.Message);
                }
                return null;
            }
        }

        /// <summary>
        /// Определяет образуют ли точки p1, p2, p3 левый поворот
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        /// <returns></returns>
        static bool IsLeftTurn(Point p1, Point p2, Point p3)
        {
            int ux = p2.X - p1.X;
            int uy = p2.Y - p1.Y;
            int vx = p3.X - p2.X;
            int vy = p3.Y - p2.Y;
            return ux * vy - uy * vx >= 0;
        }

        /// <summary>
        /// Определяет образуют ли точки p1, p2, p3 прямую
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        /// <returns></returns>
        static bool IsLineTurn(Point p1, Point p2, Point p3)
        {
            int ux = p2.X - p1.X;
            int uy = p2.Y - p1.Y;
            int vx = p3.X - p2.X;
            int vy = p3.Y - p2.Y;
            return ux * vy - uy * vx == 0;
        }

        /// <summary>
        /// Определяет образуют ли точки p1, p2, p3 правый поворот
        /// </summary>
        /// <param name="p1"></param>
        /// <param name="p2"></param>
        /// <param name="p3"></param>
        /// <returns></returns>
        static bool IsRightTurn(Point p1, Point p2, Point p3)
        {
            int ux = p2.X - p1.X;
            int uy = p2.Y - p1.Y;
            int vx = p3.X - p2.X;
            int vy = p3.Y - p2.Y;
            return ux * vy - uy * vx <= 0;
        }

        /// <summary>
        /// Находит точку с минимальной координатой Y
        /// </summary>
        /// <param name="points">множество точек</param>
        /// <returns></returns>
        static Point FindMinYPoint(params Point[] points)
        {
            Point minYPoint = points[0];
            foreach (Point point in points)
            {
                if (point.Y < minYPoint.Y)
                {
                    minYPoint = point;
                }
                else if (point.Y == minYPoint.Y && point.X < minYPoint.X)
                {
                    minYPoint = point;
                }
            }

            return minYPoint;
        }

        /// <summary>
        /// Сортирует точки по полярному углу относительно startPoint.
        /// В случае если углы равны, то большей считается точка расстояние от которой до 
        /// startPoint больше
        /// </summary>
        /// <param name="points"></param>
        /// <param name="startPoint"></param>
        /// <param name="side"></param>
        /// <returns></returns>
        static List<Point> SortPoints(List<Point> points, Point startPoint, bool side) 
        {
            if (side)
            {
                return points.OrderByDescending(x => x.GetPolarAngle(startPoint)).ThenBy(x => x.Distance(startPoint)).ToList();
            }
            else
            {
                return points.OrderBy(x => x.GetPolarAngle(startPoint)).ThenBy(x => x.Distance(startPoint)).ToList();
            }
        }


        /// <summary>
        /// Строит выпуклую оболочку множества точек против часовой стрелки
        /// </summary>
        /// <param name="points"></param>
        /// <returns></returns>
        static Stack<Point> GetPolygonCWS(List<Point> points)
        {
            Point startPoint = FindMinYPoint(points.ToArray());
            points = SortPoints(points, startPoint, false);
            Stack<Point> polygon = new Stack<Point>(points.Count());
            
            polygon.Push(startPoint);
            polygon.Push(points[1]);
            for (int i = 2; i < points.Count(); i++)
            {
                while (!IsLeftTurn(polygon.NextToTop(), polygon.Top(), points[i]))
                {
                    polygon.Pop();
                }

                if (IsLineTurn(polygon.NextToTop(), polygon.Top(), points[i]))
                {
                    polygon.Pop();
                    polygon.Push(points[i]);
                }
                else
                {
                    polygon.Push(points[i]);
                }
            }

            return polygon;
        }

        /// <summary>
        /// Строит выпуклую оболочку множества точек по часовой стрелки
        /// </summary>
        /// <param name="points"></param>
        /// <returns></returns>
        static Stack<Point> GetPolygonCCS(List<Point> points)
        {
            Point startPoint = FindMinYPoint(points.ToArray());
            points.Remove(startPoint);
            points = SortPoints(points, startPoint, true);
            Stack<Point> polygon = new Stack<Point>(points.Count() + 1);

            polygon.Push(startPoint);
            polygon.Push(points[0]);
            for (int i = 1; i < points.Count(); i++)
            {
                while (!IsRightTurn(polygon.NextToTop(), polygon.Top(), points[i]))
                {
                    polygon.Pop();
                }

                if (IsLineTurn(polygon.NextToTop(), polygon.Top(), points[i]))
                {
                    polygon.Pop();
                    polygon.Push(points[i]);
                }
                else
                {
                    polygon.Push(points[i]);
                }
            }

            return polygon;
        }

        static string Plain(Stack<Point> stack)
        {
            string result = $"{stack.Count}\n";
            foreach (Point point in stack)
            {
                result += $"{point.X} {point.Y}\n";
            }
            return result;
        }

        static string WKTAllPoints(List<Point> points)
        {
            string result = $"MULTIPOINT ({string.Join(", ", points)})";
            return result;
        }

        static string WKTStack(Stack<Point> stack)
        {
            string result = "POLYGON ((";
            foreach (Point point in stack)
            {
                result += $"{point.X} {point.Y},";
            }
            result += $"{stack.First().X} {stack.First().Y}))";
            return result;
        }

        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                System.Console.WriteLine("Please enter a numeric argument.");
                return;
            }

            List<Point> points = ReadAndFill(args[2]);
            Stack<Point> polygon;
            string wktPoints = WKTAllPoints(points); ;

            if (args[0] == "cw")
            {
                polygon = GetPolygonCWS(points);
            }
            else
            {
                polygon = GetPolygonCCS(points);
            }

            if (args[1] == "plain")
            {
                File.WriteAllText(args[3], Plain(polygon));
            }
            else
            {
                File.WriteAllText(args[3], $"{wktPoints}\n{WKTStack(polygon)}");
            }
            
        }
    }
}
