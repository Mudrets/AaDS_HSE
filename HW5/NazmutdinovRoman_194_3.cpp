#include <iostream>
#include "gdal.h"
#include "ogrsf_frmts.h"
#include <boost/geometry.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

typedef boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>> box;
typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point;

class Node {
private:
    int minCountBoxes; //минимальное количество боксов
    int maxCountBoxes; //максимальное количество боксов
    bool isLeaf;
    std::vector<box> boxes;
    std::vector<int> keys;
    std::vector<Node> children;

    /// <summary>
    /// Находит площадь прямоугольника по координатам правого верхнего угла
    /// и левого нижнего угла
    /// </summary>
    /// <param name="minX">минимальная координата X</param>
    /// <param name="minY">минимальная координата Y</param>
    /// <param name="maxX">максимальная координата X</param>
    /// <param name="maxY">максимальная координата Y</param>
    /// <returns>площадь прямоугольника</returns>
    static double getArea(double minX, double minY, double maxX, double maxY) {
        return (maxX - minX) * (maxY - minY);
    }

    /// <summary>
    /// Находит площадь прямоугольника по точкам правого верхнего угла 
    /// и левого нижнего угла
    /// </summary>
    /// <param name="minCorner">точка левого нижнего угла</param>
    /// <param name="maxCorner">точка правого нижнего угла</param>
    /// <returns>площадь прямоугольника</returns>
    static double getArea(point minCorner, point maxCorner) {
        return (maxCorner.get<0>() - minCorner.get<0>()) * (maxCorner.get<1>() - minCorner.get<1>());
    }

    /// <summary>
    /// Находит площадь прямоугольника box
    /// </summary>
    /// <param name="box">прямоугольник</param>
    /// <returns>плозадь прямоугольника</returns>
    static double getArea(box box) {
        return (box.max_corner().get<0>() - box.min_corner().get<0>()) * 
            (box.max_corner().get<1>() - box.min_corner().get<1>());
    }

    /// <summary>
    /// Находит ограничивающий прямоугольник для box1 и box2
    /// </summary>
    /// <param name="box1"></param>
    /// <param name="box2"></param>
    /// <returns>ограничивающий прямоугольник</returns>
    static box getMBR(box box1, box box2) {
        double minX, minY, maxX, maxY;
        minX = std::min(box1.min_corner().get<0>(), box2.min_corner().get<0>());
        minY = std::min(box1.min_corner().get<1>(), box2.min_corner().get<1>());
        maxX = std::max(box1.max_corner().get<0>(), box2.max_corner().get<0>());
        maxY = std::max(box1.max_corner().get<1>(), box2.max_corner().get<1>());
        point maxPoint(maxX, maxY);
        point minPoint(minX, minY);
        box mbr(minPoint, maxPoint);
        return mbr;
    }

    /// <summary>
    /// Находит ограничивающий прямоугольник для вектора прямоугольников
    /// </summary>
    /// <param name="boxes">вектор прямоугольников</param>
    /// <returns>ограничивающий прямоугольник</returns>
    static box getBoxesMBR(std::vector<box> boxes) {
        double minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;
        //Находим минимальные и максимальные координаты
        for (box loopBox : boxes) {
            minX = std::min(minX, loopBox.min_corner().get<0>());
            minY = std::min(minY, loopBox.min_corner().get<1>());
            maxX = std::max(maxX, loopBox.max_corner().get<0>());
            maxY = std::max(maxY, loopBox.max_corner().get<1>());
        }
        //Создаем точки правого верхнего и левого нижнего угла 
        point maxPoint(maxX, maxY);
        point minPoint(minX, minY);
        //Создаем ограничивающий прямоугольник
        box mbr(minPoint, maxPoint);
        return mbr;
    }

    /// <summary>
    /// Создает два узла, являющиеся результатом разделения одного
    /// из узлов
    /// </summary>
    /// <param name="index">индекс разделяемого узла</param>
    /// <param name="ind1">индекс первого опорного бокса узла</param>
    /// <param name="ind2">индекс второго опорного бокса узла</param>
    /// <returns></returns>
    std::pair<Node, Node> createSplitNodes(int index, int ind1, int ind2) {
        std::vector<int> group1;
        std::vector<box> boxesNode1;
        std::vector<int> keys1;
        std::vector<int> group2;
        std::vector<box> boxesNode2;
        std::vector<int> keys2;
        std::vector<Node> children1;
        std::vector<Node> children2;

        //Добавляем опорные боксы и данные связанные с ними
        group1.push_back(ind1); //индексы боксов первой группы основанной на первом опорном боксе
        group2.push_back(ind2); //индексы боксов первой группы основанной на втором опорном боксе
        boxesNode1.push_back(children[index].boxes[ind1]); //сами боксы первой группы
        boxesNode2.push_back(children[index].boxes[ind2]); //сами боксы второй группы
        if (!children[index].isLeaf)
        {
            children1.push_back(children[index].children[ind1]); //дети боксов первой группы
            children2.push_back(children[index].children[ind2]); //дети боксов второй группы
        }
        else {
            keys1.push_back(children[index].keys[ind1]); //ключи боксов первой группы
            keys2.push_back(children[index].keys[ind2]); //ключи боксов второй группы
        }

        //Ищем боксы, которые наименьшим образом влияют на изменения MBR первой группы боксов
        for (int i = 0; i < maxCountBoxes / 2 - 1; i++)
        {
            int minMBR = INT32_MAX;
            int minAreaBoxInd = -1;
            for (int j = 0; j < children[index].boxes.size(); j++) {
                box curBox = children[index].boxes[j];
                if (j != ind2 && std::find(group1.begin(), group1.end(), j) == group1.end())
                    if (getArea(getBoxesMBR(boxesNode1)) < minMBR) {
                        minMBR = getArea(getBoxesMBR(boxesNode1));
                        minAreaBoxInd = j;
                    }
            }
            //Добавляем найденный бокс и все его данные в соответствующие вектора
            group1.push_back(minAreaBoxInd);
            boxesNode1.push_back(children[index].boxes[minAreaBoxInd]);
            if (!children[index].isLeaf)
                children1.push_back(children[index].children[minAreaBoxInd]);
            else
                keys1.push_back(children[index].keys[minAreaBoxInd]);
        }
        
        //Добавляем во вторую группу все оставшиеся боксы
        for (int i = 0; i < maxCountBoxes; i++)
        {
            if (std::find(group1.begin(), group1.end(), i) == group1.end() &&
                std::find(group2.begin(), group2.end(), i) == group2.end()) {
                group2.push_back(i);
                boxesNode2.push_back(children[index].boxes[i]);
                if (!children[index].isLeaf)
                    children2.push_back(children[index].children[i]);
                else
                    keys2.push_back(children[index].keys[i]);
            }
        }
        //Создаем узлы и возвращаем их
        Node node1(minCountBoxes, maxCountBoxes, children1, boxesNode1, keys1);
        Node node2(minCountBoxes, maxCountBoxes, children2, boxesNode2, keys2);
        return std::pair<Node, Node>(node1, node2);
    }

    /// <summary>
    /// Находит подходящий бокс для добавляемого бокса, не содержащегося ни в одном дочернем боксе
    /// </summary>
    /// <param name="insertBox"></param>
    /// <param name="key"></param>
    void insertOutBox(box insertBox, int key) {
        double minDeltaMBRArea = INT32_MAX;
        int index = 0;
        int i = 0;
        for (box& loopBox : boxes) {
            double area = getArea(loopBox);
            double newArea = getArea(getMBR(loopBox, insertBox));
            if (newArea - area < minDeltaMBRArea) { //ищем бокс дельта площади которого минимальна
                minDeltaMBRArea = newArea - area;
                index = i;
            }
            else if (newArea - area == minDeltaMBRArea)
                if (getArea(boxes[i]) > area) { //выбираем бокс с минимальной площадью
                    minDeltaMBRArea = newArea - area;
                    index = i;
                }
            i++;
        }
        boxes[index] = getMBR(boxes[index], insertBox); //изменяем наш MBR
        children[index].insert(insertBox, key);
        if (children[index].boxes.size() == maxCountBoxes)
            split(index);
    }

    /// <summary>
    /// Проверяем, что box1 содержит в себе box2
    /// </summary>
    /// <param name="box1"></param>
    /// <param name="box2"></param>
    /// <returns></returns>
    static bool contains(box box1, box box2) {
        return box1.min_corner().get<0>() <= box2.min_corner().get<0>() &&
               box1.min_corner().get<1>() <= box2.min_corner().get<1>() &&
               box1.max_corner().get<0>() >= box2.max_corner().get<0>() &&
               box1.max_corner().get<1>() >= box2.max_corner().get<1>();
    }

    /// <summary>
    /// Проверяет, что box1 и box2 пересекаются
    /// </summary>
    /// <param name="box1"></param>
    /// <param name="box2"></param>
    /// <returns></returns>
    static bool intersects(box box1, box box2) {
        return  box1.min_corner().get<0>() <= box2.max_corner().get<0>() &&
                box1.max_corner().get<0>() >= box2.min_corner().get<0>() &&
                box1.min_corner().get<1>() <= box2.max_corner().get<1>() &&
                box1.max_corner().get<1>() >= box2.min_corner().get<1>();
    }

public:
    Node(int minCountBoxes, int maxCountBoxes) : children(), boxes(), keys() {
        this->minCountBoxes = minCountBoxes;
        this->maxCountBoxes = maxCountBoxes;
        isLeaf = true;
    }

    Node(int minCountBoxes, int maxCountBoxes, std::vector<Node> children,
        std::vector<box> boxes) : children(), boxes(), keys() {
        this->minCountBoxes = minCountBoxes;
        this->maxCountBoxes = maxCountBoxes;
        this->children = children;
        this->boxes = boxes;
        this->isLeaf = children.size() == 0;
    }

    Node(int minCountBoxes, int maxCountBoxes, std::vector<Node> children,
        std::vector<box> boxes, std::vector<int> keys) : children(), boxes(), keys() {
        this->minCountBoxes = minCountBoxes;
        this->maxCountBoxes = maxCountBoxes;
        this->children = children;
        this->boxes = boxes;
        this->isLeaf = children.size() == 0 && keys.size() != 0;
        this->keys = keys;
    }

    /// <summary>
    /// Возвращает количество боксов
    /// </summary>
    /// <returns></returns>
    int getCountOfBoxes() {
        return boxes.size();
    }

    /// <summary>
    /// Находит минимальный ограничивающий прямоугольник
    /// </summary>
    /// <returns></returns>
    box getNodeMBR() {
        return getBoxesMBR(boxes);
    }

    /// <summary>
    /// Разделяет дочерний узел с индексом index на два
    /// </summary>
    /// <param name="index">индекс дочернего узла, который нужно разделить</param>
    void split(int index) {
        double maxEmptyArea = 0;
        int ind1 = 0, ind2 = 1;
        //Находим индексы опорных боксов
        for (size_t i = 0; i < children[index].boxes.size(); i++) {
            box box1 = children[index].boxes[i];
            for (size_t j = 0; j < children[index].boxes.size(); j++) {
                box box2 = children[index].boxes[j];
                if (i != j) {
                    double emptyArea = getArea(getMBR(box1, box2)) - getArea(box1) - getArea(box2);
                    if (emptyArea > maxEmptyArea) {
                        maxEmptyArea = emptyArea;
                        ind1 = i;
                        ind2 = j;
                    }
                }
            }
        }
        //Разделяет узел на два новых узла и сохраняет их данные
        std::pair<Node, Node> nodes = createSplitNodes(index, ind1, ind2);
        boxes.erase(boxes.cbegin() + index);
        children.erase(children.cbegin() + index);
        boxes.push_back(nodes.first.getNodeMBR());
        boxes.push_back(nodes.second.getNodeMBR());
        children.push_back(nodes.first);
        children.push_back(nodes.second);
    }

    /// <summary>
    /// Вставляет insertBox с ключем key в поддерево 
    /// </summary>
    /// <param name="insertBox"></param>
    /// <param name="key"></param>
    void insert(box insertBox, int key) {
        if (!isLeaf) {
            bool containsFlag = false; //флаг наличия ребенка, содержащего добавляемый бокс
            int index = -1;
            double minArea = INT32_MAX;
            int i = 0;
            //проходимся по всем боксам и находим ребенка, содержащего добавляемый бокс
            //с минимальной площадью
            for (box& loopBox : boxes) {
                if (contains(loopBox, insertBox))
                    if (getArea(loopBox) < minArea) {
                        minArea = getArea(loopBox);
                        index = i;
                        containsFlag = true;
                    }
                i++;
            }
            if (containsFlag) {//если был найден подходящий ребенок, то вставляем в него
                children[index].insert(insertBox, key);
                if (children[index].boxes.size() == maxCountBoxes)
                    split(index);
            }
            else
                insertOutBox(insertBox, key); //добавления бокса вне детей узла
        } else { //если лист, то просто вставляем бокс
            boxes.push_back(insertBox);
            keys.push_back(key);
        }
    }

    /// <summary>
    /// Определяет все ключи элементов, пересекающихся с searchedBox
    /// </summary>
    /// <param name="searchedBox"></param>
    /// <returns>вектор всех ключей</returns>
    std::vector<int> search(box searchedBox) {
        std::vector<int> result;
        int i = 0;
        for (box& loopBox : boxes) {
            if (intersects(loopBox, searchedBox)) {
                if (isLeaf)
                    result.push_back(keys[i]);
                else {
                    std::vector<int> searchedID = children[i].search(searchedBox);
                    result.insert(result.cend(), searchedID.begin(), searchedID.end());
                }
            }
            i++;
        }
        return result;
    }
};

class Rtree {
private:
    int minCountBoxes; //минимальное количество боксов
    int maxCountBoxes; //максимальное количество боксов
    Node root; //корень

public:
    Rtree(int minCountBoxes, int maxCountBoxes) : root(minCountBoxes, maxCountBoxes) {
        this->minCountBoxes = minCountBoxes;
        this->maxCountBoxes = maxCountBoxes;
    }

    /// <summary>
    /// Вставляет прямоугольник в дерево
    /// </summary>
    /// <param name="insertBox">вставляемый прямоугольник</param>
    /// <param name="key">ключ</param>
    void insert(box insertBox, int key) {
        root.insert(insertBox, key);
        if (root.getCountOfBoxes() == maxCountBoxes) {
            std::vector<Node> children;
            children.push_back(root);
            std::vector<box> boxes;
            boxes.push_back(root.getNodeMBR());
            Node newNode(minCountBoxes, maxCountBoxes, children, boxes);
            newNode.split(0);
            root = newNode;
        } 
    }

    /// <summary>
    /// Определяет все ключи элементов, пересекающихся с searchedBox 
    /// </summary>
    /// <param name="searchedBox"></param>
    /// <returns></returns>
    std::vector<int> search(box searchedBox) {
        return root.search(searchedBox);
    }
};

/// <summary>
/// Разбивает строку на массив строк по некоторому разделителю
/// </summary>
/// <param name="str">разделяемая строка</param>
/// <param name="delimiter"></param>
/// <returns></returns>
std::vector<std::string> split(std::string& str, std::string delimiter) 
{
    size_t pos = 0;
    std::string token;
    std::vector<std::string> result;
    while ((pos = str.find(delimiter)) != std::string::npos)
    {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
    return result;
};

/// <summary>
/// Переводит строковое представление точек в box
/// </summary>
/// <param name="strBox"></param>
/// <returns></returns>
box toBox(std::string strBox)
{
    std::vector<std::string> coordinates = split(strBox, " ");
    double minX = std::atof(coordinates[0].c_str());
    double minY = std::atof(coordinates[1].c_str());
    boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> minPoint(minX, minY);
    double maxX = std::atof(coordinates[2].c_str());
    double maxY = std::atof(coordinates[3].c_str());
    boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> maxPoint(maxX, maxY);
    boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>> box(minPoint, maxPoint);
    return box;
}

int main(int argc, char* argv[])
{
    if (argc != 4) { //Проверка корректности аргументов
        std::cout << "Incorrect input" << std::endl;
        return 1;
    }

    GDALAllRegister();
    std::string dataPath = std::string(argv[1]) + "/building-polygon.shp";
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(
        dataPath.c_str(),
        GDAL_OF_VECTOR,
        nullptr, nullptr, nullptr));
    if (dataset == nullptr)
    {
        std::cout << "cannot open file" << std::endl;
        return -1;
    }
    Rtree rtree(4, 8);
    int i = 0;
    for (auto&& layer : dataset->GetLayers())
    {
        for (auto&& feature : layer)
        {
            auto* geometry = feature->GetGeometryRef();
            OGREnvelope* e = new OGREnvelope();
            geometry->getEnvelope(e);
            boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> maxPoint(e->MaxX, e->MaxY);
            boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> minPoint(e->MinX, e->MinY);
            boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>> box(minPoint, maxPoint);
            rtree.insert(box, feature->GetFieldAsInteger("OSM_ID"));
            i++;
        }
    }
    //Создаем поток и проверяем, что он получил доступ к файлу
    std::ifstream fs(argv[2]);
    if (!fs)
    {
        std::cout << "Could not open input file";
        return -1;
    }

    std::string strBox;
    getline(fs, strBox);
    fs.close();
    box inputBox = toBox(strBox);
    std::vector<int> id = rtree.search(inputBox);
    std::sort(id.begin(), id.end());
    std::ofstream out(argv[3]);
    for (size_t i = 0; i < id.size(); i++)
        out << id[i] << std::endl;
    out.close();
    return 0;
}
