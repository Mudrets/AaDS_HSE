#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm> 
#include <fstream>
#include <string>

class Point {

public:
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Point() {

    }

    bool operator == (Point p) {
        return p.getX() == x && p.getY() == y;
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    double getPolarAngel() const {
        return acos(x / sqrt(x * x + y * y)) * 180 / M_PI;
    }

    double getPolarAngel(Point point) const {
        Point *newPoint = new Point(x - point.getX(), y - point.getY());
        return newPoint->getPolarAngel();
    }

    double distance(Point point) const {
        return pow((x - point.getX()) * (x - point.getX()) + (y - point.getY()) * (y - point.getY()), 0.5);
    }

    std::string toString() {
        return std::to_string(x) + " " + std::to_string(y) ;
    }

    ~Point() {}

private:
    int x;
    int y;


};


template <typename T>
class Stack {
    
private:
    const int maxAllowedSize = 1000; // максимально возможное количество элементов в стеке
    int topIndex; // номер текущего элемента в стеке

public:
    int maxSize;
    std::vector<T> data;

    Stack(int size) {
        maxSize = size;
        if (maxSize > maxAllowedSize)
        {
            throw "Неверное значение размера стека";
        }
        data = std::vector<T>(size);
        topIndex = -1;
    }

    int Count() {
        return topIndex + 1;
    }

    bool isEmpty() {
        return topIndex + 1 == 0;
    }

    // передаем value по ссылке чтобы не тратить время на копирование переданного аргумента
    // в случае если это будет какой-то класс
    void push(T &value) {
        
        if (topIndex >= maxSize - 1)
        {
            throw "Превышен допустимый размер стека";
        }
        else
        {
            data[++topIndex] = value;
        }
    }

    T pop() {
        if (topIndex < 0)
        {
            throw "Стек пуст";
        }
        
        return data[topIndex--];
    }

    T top() {
        if (!isEmpty())
        {
            return data[topIndex];
        }
        else {
            throw "Стек пуст";
        }
    }

    T nextToTop() const {
        if (topIndex > 0)
        {
            return data[topIndex - 1];
        }
        else {
            throw "Невозможно получить предпоследний элемент стека";
        }
    }

    T first() const {
        if (!isEmpty())
        {
            return data[0];
        }
        else
        {
            throw "Стек пуст";
        }
    }

    std::vector<T> toVector() const {
        std::vector<T> result(topIndex + 1);
        for (int i = 0; i < topIndex + 1; i++)
        {
            result[i] = data[i];
        }

        return result;
    }

    ~Stack() {
        
    }
};

Point* startPoint = new Point(0, 0);

bool isLeftTurn(Point p1, Point p2, Point p3) {
    int ux = p2.getX() - p1.getX();
    int uy = p2.getY() - p1.getY();
    int vx = p3.getX() - p2.getX();
    int vy = p3.getY() - p2.getY();
    return ux * vy - uy * vx >= 0;
}

bool isLine(Point p1, Point p2, Point p3) {
    int ux = p2.getX() - p1.getX();
    int uy = p2.getY() - p1.getY();
    int vx = p3.getX() - p2.getX();
    int vy = p3.getY() - p2.getY();
    return ux * vy - uy * vx == 0;
}

bool isRightTurn(Point p1, Point p2, Point p3) {
    int ux = p2.getX() - p1.getX();
    int uy = p2.getY() - p1.getY();
    int vx = p3.getX() - p2.getX();
    int vy = p3.getY() - p2.getY();
    return ux * vy - uy * vx <= 0;
}

Point findMinYPoint(std::vector<Point> points) {    
    Point minPoint = points[0];
    for (int i = 0; i < points.size(); i++)
    {
        if (minPoint.getY() > points[i].getY())
        {
            minPoint = points[i];
        }
        else if (minPoint.getY() == points[i].getY() && minPoint.getX() > points[i].getX())
        {
            minPoint = points[i];
        }
    }

    return minPoint;
}

bool compAngleAndDist(const Point &p1, const Point &p2) {
    if (p1.getPolarAngel(*startPoint) < p2.getPolarAngel(*startPoint))
    {
        return false;
    }
    else if(p1.getPolarAngel(*startPoint) == p2.getPolarAngel(*startPoint))
    {
        return !(p1.distance(*startPoint) < p2.distance(*startPoint));
    }
    else
    {
        return true;
    }
}

std::vector<Point> sortPoints(std::vector<Point> points) {
    sort(points.begin(), points.end(), [](const Point& p1, const Point& p2) {
        bool result = p1.getPolarAngel(*startPoint) != p2.getPolarAngel(*startPoint) ?
            p1.getPolarAngel(*startPoint) < p2.getPolarAngel(*startPoint) :
            p1.distance(*startPoint) < p2.distance(*startPoint);
        return result;
    });
    return points;
}

Stack<Point> getPolygonCWS(std::vector<Point> points) {
    *startPoint = findMinYPoint(points);
    points.erase(std::remove(points.begin(), points.end(), *startPoint), points.end());
    points = sortPoints(points);
    Stack<Point> polygon(points.size());

    polygon.push(*startPoint);
    polygon.push(points[0]);
    for (int i = 1; i < points.size(); i++) 
    {
        while (!isLeftTurn(polygon.nextToTop(), polygon.top(), points[i]))
        {
            polygon.pop();
        }
        polygon.push(points[i]);
    }

    return polygon;
}

void printPlain(std::string path, std::vector<Point>& outputVec)
{
    std::ofstream out;
    out.open(path);
    out << outputVec.size() << "\n";
    for (int i = 0; i < outputVec.size(); i++)
    {
        out << outputVec[i].toString() << "\n";
    }
    out.close();
}

void printWkt(std::string path, std::vector<Point>& inputVec, std::vector<Point>& outputVec)
{
    std::ofstream out;
    out.open(path);
    out << "MULTIPOINT (";
    for (int i = 0; i < inputVec.size(); i++)
    {
        out << "(" << inputVec[i].toString() << ")";
        if (i != inputVec.size() - 1)
        {
            out << ", ";
        }
    }
    out << ")" << "\n" << "POLYGON ((";
    for (int i = 0; i < outputVec.size(); i++)
    {
        out << outputVec[i].toString();
        if (i != outputVec.size() - 1)
        {
            out << ", ";
        }
    }
    out << "))";
    out.close();
}

int main(int argc, char* argv[])
{
    if (argc == 5)
    {
        std::vector<Point> inputPoints;
        std::ifstream inputSt;

        inputSt.open(argv[3]);
        int n;
        inputSt >> n;

        for (int i = 0; i < n; i++)
        {
            int x, y;
            inputSt >> x >> y;
            inputPoints.push_back(Point(x, y));
        }

        inputSt.close();

        std::vector<Point> result = getPolygonCWS(inputPoints).toVector();
        n = result.size();
        std::vector<Point> resultVec = std::vector<Point>(result.size());

        resultVec[0] = result[0];
        if (std::string(argv[1]) == "cc")
        {
            int k = 1;
            for (int i = n - 1; i >= 1; i--)
            {
                resultVec[i] = result[k++];
            }
        }
        else if (std::string(argv[1]) == "cw")
        {
            for (int i = 1; i < n; i++)
            {
                resultVec[i] = result[i];
            }
        }

        if (std::string(argv[2]) == "plain")
        {
            printPlain(argv[4], resultVec);
        }
        else if (std::string(argv[2]) == "wkt")
        {
            printWkt(argv[4], inputPoints, resultVec);
        }
    }
}