#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>

static std::vector<std::string> split(std::string* str, std::string delimiter) {
    size_t pos = 0;
    std::string token;
    std::vector<std::string> result;
    while ((pos = str->find(delimiter)) != std::string::npos)
    {
        token = str->substr(0, pos);
        result.push_back(token);
        str->erase(0, pos + delimiter.length());
    }
    result.push_back(*str);
    return result;
};

class Node {
private:
    static unsigned long long numFile;
    int maxCountElems; //Максимальное число элементов
    int minCountElems; //Минимальное число элементов
    int keysCount; //Количество ключей в узле
    bool isLeaf; //Является ли узел листом
    std::string path; //Путь к файлу с ифнормацией о узле
    std::vector<int> keys; //Хранящиеся ключи в узле
    std::vector<int> values; //Хранящиеся значения по этим ключам
    std::vector<std::string> childNodesPath; //путь к файлам

    Node (std::vector<int>* keys, std::vector<int>* values,
          std::vector<std::string>* childNodesPath, int minBranchingDegree, std::string* path) {
        this->keys = *keys;
        this->values = *values;
        this->childNodesPath = *childNodesPath;
        this->path = *path;
        minCountElems = minBranchingDegree - 1;
        maxCountElems = 2 * minBranchingDegree - 1;
        isLeaf = childNodesPath->size() == 0;
        keysCount = keys->size();
    }

    Node (std::vector<int>* keys, std::vector<int>* values,
          std::vector<std::string>* childNodesPath, int minBranchingDegree) {
        this->keys = *keys;
        this->values = *values;
        this->childNodesPath = *childNodesPath;
        this->path = dirPath + std::to_string(numFile++) + ".bin";
        minCountElems = minBranchingDegree - 1;
        maxCountElems = 2 * minBranchingDegree - 1;
        isLeaf = childNodesPath->size() == 0;
        keysCount = keys->size();
        write();
    }

    static std::vector<std::string> split(std::string* str, std::string delimiter) {
        size_t pos = 0;
        std::string token;
        std::vector<std::string> result;
        while ((pos = str->find(delimiter)) != std::string::npos)
        {
            token = str->substr(0, pos);
            result.push_back(token);
            str->erase(0, pos + delimiter.length());
        }
        result.push_back(*str);
        return result;
    };

    static std::string join(std::vector<int>* vec, std::string delimiter) {
        std::string result = "";
        for (const auto &num : *vec) result += std::to_string(num) + delimiter;
        return result.substr(0, result.length() - delimiter.length()); //убираем лишний разделитель
    }

    static std::string join(std::vector<std::string>* vec, std::string delimiter) {
        std::string result = "";
        for (const auto &str : *vec) result += str + delimiter;
        return result.substr(0, result.length() - 1); //убираем лишний разделитель
    }

    /**
     * Записывает узел в память
     */
    void write() {
        std::string data;
        int t = minCountElems + 1; //Минимальная степень ветвления дерева
        std::vector<std::string> dataVec;
        dataVec.push_back(std::to_string(t)); //записываем минимальную степень ветвления
        dataVec.push_back(join(&keys, ";"));
        dataVec.push_back(join(&values, ";"));
        dataVec.push_back(join(&childNodesPath, ";"));
        data = join(&dataVec, "|");
        //Записываю значения полей в бинарный файл
        std::ofstream out(path, std::ios::binary);
        size_t dataSize = data.size();
        out.write((const char*)&dataSize, sizeof(dataSize));
        out.write(data.c_str(), dataSize);
        out.close();
    }

    void setKeys(std::vector<int>* keys) {
        this->keys = *keys;
        keysCount = this->keys.size();
    }

    void setValues(std::vector<int>* values) {
        this->values = *values;
    }

    void setChildNodesPath(std::vector<std::string>* childNodesPath) {
        isLeaf = childNodesPath->size() == 0;
        this->childNodesPath = *childNodesPath;
    }

    Node* merge(Node* child1, Node* child2, int index) {
        child1->keys.push_back(keys[index]);
        child1->values.push_back(values[index]);

        for (int i = 0; i < child2->keysCount; ++i) {
            child1->keys.push_back(child2->keys[i]);
            child1->values.push_back(child2->values[i]);
        }

        if (!child1->isLeaf)
            for (int i = 0; i <= child2->keysCount; ++i)
                child1->childNodesPath.push_back(child2->childNodesPath[i]);

        keys.erase(keys.cbegin() + index);
        values.erase(values.cbegin() + index);
        childNodesPath.erase(childNodesPath.cbegin() + index + 1);

        child1->keysCount += child2->keysCount + 1;
        keysCount--;
        child1->write();
        write();
        return child1;
    }

    std::pair<int, int> getMinKey(Node* child) {
        Node* cur = child;
        std::vector<Node*> nodesPtr; //вектор указателей
        while (!cur->isLeaf) {
            nodesPtr.push_back(cur);
            cur = read(cur->childNodesPath[0]);
        }
        std::pair<int, int> keyValue(cur->keys[0], cur->values[0]);

        //Удаляем созданные указатели
        for (int i = 0; i < nodesPtr.size(); ++i)
            delete nodesPtr[i];

        return keyValue;
    }

    std::pair<int, int> getMaxKey(Node* child) {
        Node* cur = child;
        std::vector<Node*> nodesPtr; //вектор указателей
        while (!cur->isLeaf) {
            nodesPtr.push_back(cur);
            cur = read(cur->childNodesPath[cur->keysCount]);
        }
        int lastIndex = cur->keysCount - 1;
        std::pair<int, int> keyValue(cur->keys[lastIndex], cur->values[lastIndex]);

        //Удаляем созданные указатели
        for (int i = 0; i < nodesPtr.size(); ++i)
            delete nodesPtr[i];

        return keyValue;
    }

    void deleteFromLeaf(int index) {
        keys.erase(keys.cbegin() + index);
        values.erase(values.cbegin() + index);
        keysCount--;
    }

    void deleteFromNotLeaf(int index) {
        int key = keys[index]; //получаем ключ по индесу
        //Удаляем ключ заменяя его на максимальный меньшего ребенка
        Node* child = read(childNodesPath[index]);
        if (child->keysCount > minCountElems) {
            std::pair<int, int> keyVal = getMaxKey(child);
            keys[index] = keyVal.first;
            values[index] = keyVal.second;
            child->deleteElem(keyVal.first);
            child->write();
        } else {
            //Удаляем ключ заменяя его на минимальный большего ребенка
            Node* nextChild = read(childNodesPath[index + 1]);
            if (nextChild->keysCount > minCountElems) {
                std::pair<int, int> keyVal = getMinKey(nextChild);
                keys[index] = keyVal.first;
                values[index] = keyVal.second;
                nextChild->deleteElem(keyVal.first);
                nextChild->write();
            } else { //Если nextChild и child содержат minCountElems, то объединяем их и удаляем ключ
                child = merge(child, nextChild, index);
                child->deleteElem(key);
            }
            delete nextChild;
        }
        delete child;
    }

    void getFromPrevChild(Node* receiveChild, Node* giverChild, int index) {
        //Добавляем место в начале под ключ и значение
        receiveChild->keys.insert(receiveChild->keys.cbegin(), keys[index - 1]);
        receiveChild->values.insert(receiveChild->values.cbegin(), values[index - 1]);
        //Добавляем место в начале под нового ребенка, если узел является листом
        if (!receiveChild->isLeaf) {
            receiveChild->childNodesPath.insert(receiveChild->childNodesPath.cbegin(),
                                                giverChild->childNodesPath[giverChild->keysCount]);
            giverChild->childNodesPath.erase(giverChild->childNodesPath.cend());
        }
        //Записываем ключ и значение в родителя
        keys[index - 1] = giverChild->keys[giverChild->keysCount - 1];
        values[index - 1] = giverChild->values[giverChild->keysCount - 1];
        giverChild->keys.resize(giverChild->keysCount - 1);
        giverChild->values.resize(giverChild->keysCount - 1);
        //Изменяем кол-во ключей
        receiveChild->keysCount++;
        giverChild->keysCount--;
        //Сохраняем измененные узлы
        receiveChild->write();
        giverChild->write();
        write();
    }

    void getFromNextChild(Node* receiveChild, Node* giverChild, int index) {
        //Добавляем в ребенка ключ и значение родителя
        receiveChild->keys.push_back(keys[index]);
        receiveChild->values.push_back(values[index]);
        //Добавляем ребенка отдающего ребенка
        if (!receiveChild->isLeaf)
            receiveChild->childNodesPath.push_back(giverChild->childNodesPath[giverChild->keysCount + 1]);
        //Записываем в родителя ключ и значение отдающего ребенка
        keys[index] = giverChild->keys[0];
        values[index] = giverChild->values[0];
        //Удаляем нулевые ключ, значение и ребенка отдающего ребенка
        giverChild->keys.erase(giverChild->keys.cbegin());
        giverChild->values.erase(giverChild->values.cbegin());
        if (!giverChild->isLeaf)
            giverChild->childNodesPath.erase(giverChild->childNodesPath.cbegin());
        //Изменяем кол-во ключей
        receiveChild->keysCount++;
        giverChild->keysCount--;
        //Сохраняем измененные узлы
        receiveChild->write();
        giverChild->write();
        write();
    }

    void increaseCountKeys(Node* child, int index) {
        bool prev = false;
        bool next = false;
        Node* prevChild;
        if (index != 0) {
            prev = true;
            prevChild = read(childNodesPath[index - 1]);
        }
        if (index != 0 && prevChild->keysCount > minCountElems)
            getFromPrevChild(child, prevChild, index);
        else {
            Node* nextChild;
            if (index != keysCount) {
                next = true;
                nextChild = read(childNodesPath[index + 1]);
            }
            if (index != keysCount && nextChild->keysCount > minCountElems)
                getFromNextChild(child, nextChild, index);
            else {
                if (index != keysCount)
                    merge(child, nextChild, index);
                else
                    child = merge(prevChild, child, index - 1);
            }
            if (next)
                delete nextChild;
        }
        if (prev)
            delete prevChild;
    }

public:
    static std::string dirPath;

    Node(Node* node) {
        minCountElems = node->minCountElems;
        maxCountElems = node->maxCountElems;
        path = node->path;
        keysCount = node->keysCount;
        isLeaf = node->isLeaf;
        keys = node->keys;
        values = node->values;
        childNodesPath = node->childNodesPath;
    }

    Node(int minBranchingDegree, std::string* directoryPath) : keys(), values(), childNodesPath() {
        this->minCountElems = minBranchingDegree - 1;
        this->maxCountElems = 2 * minBranchingDegree - 1;
        dirPath = *directoryPath;
        path = dirPath + std::to_string(numFile++) + ".bin";
        keysCount = 0;
        isLeaf = true;
        write();
    }

    Node(int minBranchingDegree) : keys(), values(), childNodesPath() {
        this->minCountElems = minBranchingDegree - 1;
        this->maxCountElems = 2 * minBranchingDegree - 1;
        path = dirPath + std::to_string(numFile++) + ".bin";
        keysCount = 0;
        isLeaf = true;
        write();
    }

    Node (int minBranchingDegree, bool isLeaf) : keys(), values(), childNodesPath() {
        this->minCountElems = minBranchingDegree - 1;
        this->maxCountElems = 2 * minBranchingDegree - 1;
        path = dirPath + std::to_string(numFile++) + ".bin";
        keysCount = 0;
        this->isLeaf = isLeaf;
        write();
    }

    /**
     * @return количество ключей в узле
     */
    int getKeysCount() {
        return keysCount;
    }

    /**
     * @return является ли листом
     */
    bool getIsLeaf() {
        return isLeaf;
    }

    std::vector<std::string> getChildNodePath() {
        return childNodesPath;
    }

    /**
     * Добавляет нового ребенка child в список детей под индексом index
     * @param child добавляемый ребенок
     * @param index индекс, куда будет добавлен ребенок
     */
    void addChild(Node *child, int index) {
        isLeaf = false;
        childNodesPath.insert(childNodesPath.cbegin() + index, child->path);
    }

    /**
    * Считывает элемент из памяти
    */
    static Node* read(std::string path) {
        //Считываем строку из бинарного фала
        std::string data;
        std::ifstream fin(path, std::ios::binary);
        if (fin)
        {
            int strSize = 0;
            fin.read((char*)&strSize, sizeof(strSize)); // читаем из файла размер записанной строки
            data.resize(strSize); // выделяем память нужного размера
            fin.read(&data[0], strSize); // читаем строку из файла
            fin.close();
        }
        //Парсим полученную строку в узел
        std::vector<std::string> dataVec = split(&data, "|");
        int t = std::stoi(dataVec[0]); //Минимальная степень витвления
        std::vector<std::string> strKeys = split(&dataVec[1], ";");
        std::vector<int> keys;
        keys.resize(strKeys.size());
        for (int i = 0; i < keys.size(); ++i)
            keys[i] = std::stoi(strKeys[i]);
        std::vector<std::string> strValues = split(&dataVec[2], ";");
        std::vector<int> values;
        values.resize(strValues.size());
        for (int i = 0; i < values.size(); ++i)
            values[i] = std::stoi(strValues[i]);
        std::vector<std::string> childNodesPath;
        if (dataVec[3].length() > 0)
            childNodesPath = split(&dataVec[3], ";");
        return new Node(&keys, &values, &childNodesPath, t, &path);
    }

    /**
     * Находит элемент с индексом index и возвращает его знчение
     * @param key ключ искомого элемента
     * @return значение элемента с индексом index
     */
    int find(int key) {
        int i = 0;
        while (i < keysCount && key > keys[i])
            i++;
        if (i < keysCount && key == keys[i])
            return values[i];
        if (!isLeaf) {
            std::unique_ptr<Node> child = (std::unique_ptr<Node>) read(childNodesPath[i]); //считываем из памяти
            return child->find(key);
        }
        return INT32_MAX; //в случае если элемента нет, то возвращаем 2^31-1
    }

    /**
     * Добавляет новый элемент value по ключу key
     * @param key ключ
     * @param value значение
     */
    void insert(int key, int value) {
        int i = keysCount;
        if (isLeaf) {
            keys.push_back(0); //расширяем лист на 1 элемент
            values.push_back(0); //расширяем лист на 1 элемент
            while (i >= 1 && key < keys[i - 1]) {
                keys[i] = keys[i - 1];
                values[i] = values[i - 1];
                i--;
            }
            keys[i] = key;
            values[i] = value;
            keysCount++;
            write();
        } else {
            while (i >= 1 && key < keys[i - 1]) //определяем индекс нужного узла
                i--;
            Node* child = read(childNodesPath[i]); //считываем узел из памяти
            if (child->getKeysCount() == maxCountElems) {
                Node* nextChild = splitChild(child, i); //разделяем дочерний узел
                //если добавленный в узел ключ меньше переданного, то переходим к следующему ребенку
                if (key > keys[i]) {
                    delete child;
                    child = nextChild;
                }
            }
            child->insert(key, value);
            delete child;
        }
    }



    /**
     * Удаляет элемент из дерева
     * @param key ключ удаляемого элемента
     * @return значение удаляемого элемента
     */
    void deleteElem(int key) {
        //Находим индекс подходящего ключа или ребенка
        int i = 0;
        while (i < keysCount && key > keys[i])
            i++;
        if (i < keysCount && key == keys[i]) {
            if (isLeaf)
                deleteFromLeaf(i);
            else
                deleteFromNotLeaf(i);
        } else {
            Node* child = read(childNodesPath[i]);
            if (child->keysCount < minCountElems + 1)
                increaseCountKeys(child, i);
            child->deleteElem(key);
            delete child;
        }
        write();
    }

    /**
    * Разделяет дочерний узел с индексом index
    * @param child разделяемый ребенок
    * @param index индекс разделяемого дочернего узла
    */
    Node* splitChild(Node* child, int index) {
        //Создаем нового ребенка и отдаем ему часть данных
        std::vector<int> newKeys;
        std::vector<int> newValues;
        for (int i = 0; i < minCountElems; ++i) {
            newKeys.push_back(child->keys[i + minCountElems + 1]); // + 1 так как средний эл. поднимаем на узел выше
            newValues.push_back(child->values[i + minCountElems + 1]);
        }
        std::vector<std::string> newChildNodesPath;
        if (!child->isLeaf) {
            for (int i = 0; i < minCountElems + 1; ++i)
                newChildNodesPath.push_back(child->childNodesPath[i + minCountElems + 1]);
            child->childNodesPath.resize(minCountElems + 1);
        }
        Node* newChild = new Node(&newKeys, &newValues, &newChildNodesPath,child->minCountElems + 1);
        //Перемещаем центральный ключ в родительский узел
        childNodesPath.insert(childNodesPath.cbegin() + index + 1, newChild->path);
        keys.insert(keys.cbegin() + index, child->keys[minCountElems]);
        values.insert(values.cbegin() + index, child->values[minCountElems]);
        child->keys.resize(minCountElems);
        child->values.resize(minCountElems);
        child->keysCount = minCountElems;
        keysCount++;
        //Сохраняем узлы
        child->write();
        newChild->write();
        write();
        return newChild;
    }

    ~Node() = default;
};

unsigned long long Node::numFile = 0;
std::string Node::dirPath = "";

class BTree {
private:
    int height; //Высота дерева (корень считается 0 высотой)
    int minBranchingDegree; //Минимальная степень витвления
    std::string dirPath;
    Node* root; //Корень дерева

public:
    BTree() {}

    BTree(int minBranchingDegree, std::string* dirPath) {
        this->dirPath = *dirPath;
        root = new Node(minBranchingDegree, dirPath);
        this->minBranchingDegree = minBranchingDegree;
        height = 0;
    }

    /**
     * @return высоту дерева
     */
    int getHeight() {
        return height;
    }

    /**
     * @return минимальную степень витвления
     */
    int getMinBranchingDegree() {
        return minBranchingDegree;
    }

    /**
     * @return корень дерева
     */
    Node getRoot() {
        return *root;
    }

    /**
     * Находит в дереве ключ и возвращает значение по этому ключу
     * @param key ключ
     * @return значение по ключу, если ключа нет, то возвращает INT32_MAX
     */
    int find(int key) {
        return root->find(key);
    }

    /**
     * Вставляет значение value по ключу key
     * @param key ключ
     * @param value значение
     * @return True - если значение было успешно добавенно
     * и False в ином случае
     */
    bool insert(int key, int value) {
        if (root == NULL)
            root = new Node(minBranchingDegree, &dirPath);
        if (root->getKeysCount() > 0 && root->find(key) != INT32_MAX)
            return false;

        if (root->getKeysCount() == 2 * minBranchingDegree - 1) {
            Node newRoot(minBranchingDegree);
            newRoot.addChild(root, 0);
            newRoot.splitChild(root, 0);
            delete root;
            root = new Node(&newRoot);
            height++;
        }
        root->insert(key, value);
        return true;
    }

    int deleteElem(int key) {
        int value = find(key);
        if (value == INT32_MAX)
            return value;

        root->deleteElem(key);
        if (root->getKeysCount() == 0) {
            Node* tmp = root;
            if (root->getIsLeaf())
                root = NULL;
            else
                root = Node::read(root->getChildNodePath()[0]);
            delete tmp;
        }
        return value;
    }

};

class Command {
protected:
    BTree bTree;
    std::string command; //Название команды
    int key;
    int value;

public:
    /// <summary>
    /// Создает команду по ее строковому представлению
    /// </summary>
    /// <param name="commandVec"></param>
    Command (std::vector<std::string>* commandVec, BTree* bTree) {
        this->bTree = *bTree;
        if ((*commandVec)[0] == "insert")
            value = std::stoi((*commandVec)[2]);
        key = std::stoi((*commandVec)[1]);
        command = (*commandVec)[0];
    }

    /// <summary>
    /// Выполянет команду
    /// </summary>
    /// <returns></returns>
    std::string Invoke() {
        if (command == "insert")
            return bTree.insert(key, value) ? "true" : "false";
        if (command == "find") {
            int value = bTree.find(key);
            return value == INT32_MAX ? "null" : std::to_string(value);
        }
        if (command == "delete") {
            int value = bTree.deleteElem(key);
            return value == INT32_MAX ? "null" : std::to_string(value);
        }
        return "unknown command";
    }
};

/// <summary>
/// Создает команду по ее строковому представлению и возвращает
/// ссылку на нее
/// </summary>
/// <param name="command">строковое представление команды</param>
/// <returns></returns>
std::unique_ptr<Command> createCommand(std::string command, BTree* bTree) {
    if (command.find('\r') != std::string::npos)
        command.erase(command.find('\r'), 1);
    std::vector<std::string> commandVec = split(&command, " ");
    if (commandVec.back() == "")
        commandVec.pop_back();

    std::unique_ptr<Command> commandPtr(new Command(&commandVec, bTree));
    return commandPtr;
}

int main(int argc, char* argv[]) {
    //Проверяем количество переданных аргументов
    if (argc != 5) {
        std::cout << "Incorrect input" << std::endl;
        return 1;
    }

    //Создаем поток и проверяем, что он получил доступ к файлу
    std::ifstream fs(argv[3], std::ios::in | std::ios::binary);
    if (!fs)
    {
        std::cout << "Could not open input file";
        return 1;
    }

    std::string dirPath(argv[2]);
    if (dirPath.find('/') != dirPath.length() - 1)
        dirPath += "/";
    BTree btree(std::stoi(argv[1]), &dirPath);

    //Считываем информацию из файла
    std::vector<std::unique_ptr<Command>> commands;
    int i = 0;
    while (!fs.eof())
    {
        i++;
        std::string str;
        getline(fs, str);
        if (str != "")
            commands.push_back(createCommand(str, &btree));
    }

    fs.close();

    //Открываем файл на запись
    std::ofstream out;
    out.open(argv[4]);
    if (!out) {
        std::cout << "Could not open output file";
        return 1;
    }

    //Записываем ответы команд в файл
    for (int i = 0; i < commands.size(); i++)
        out << commands[i]->Invoke() << std::endl;

    out.close();
    return 0;
}
