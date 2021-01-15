#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>

/// <summary>
/// �������� ���������� ������
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
unsigned long long StringHash(std::string str) {
	const int p = 59;
	unsigned long long hash = 0, p_pow = 1;
	for (int i = 0; i < str.length(); i++)
	{
		hash += (str[i] - 'a' + 1) * p_pow;
		p_pow *= p;
	}
	return hash;
}

class CuckooFilter {
private:
	//��������� �� �������
	const double fpr = 0.06;
	const double a = 0.95;
	const int b = 4;

	int countBuckets = 0; //���������� ���������
	unsigned int* buckets; //������ �������

	/// <summary>
	/// ������� ���������� ��������� �����, ������� �������� �������� ������
	/// � ��� ���� ������ ����������� �����.
	/// </summary>
	/// <param name="number"></param>
	/// <returns></returns>
	static int MinPow2MoreThan(int number) {
		int result = 1;
		while (result < number) {
			result <<= 1;
		}
		return result;
	}

	/// <summary>
	/// ���������� fingerprint ����������� �������
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	static int Fingerprint(const std::string str) {
		return StringHash(str) % 127 + 1; //���������� 1, ����� �� ����������� 0
	}

	/// <summary>
	/// ��������� ��� ������� �� 0 �� countBuckets �� ������������
	/// </summary>
	/// <param name="string"></param>
	/// <returns></returns>
	int Hash(const std::string string) {
		return std::hash<std::string>{}(string) % countBuckets;
	}

	/// <summary>
	/// ��������� ��� ������� �� 0 �� countBuckets �� ������������
	/// </summary>
	/// <param name="string"></param>
	/// <returns></returns>
	int Hash(const int num) {
		return std::hash<int>{}(num) % countBuckets;
	}

	/// <summary>
	/// �������� ��� ������
	/// </summary>
	void ClearBuckets() {
		for (int i = 0; i < countBuckets; i++)
			buckets[i] = 0;
	}

	/// <summary>
	/// ������� ������ ���������� �������� � bucket
	/// � ������������ ��������. � ������ ���� ��� ���
	/// ���������� -1
	/// </summary>
	/// <param name="bucketIndex">������ ������</param>
	/// <returns></returns>
	int EmptyBucketEntry(int bucketIndex) {
		int value = buckets[bucketIndex];
		int power2 = pow(2, 7);
		for (int i = 0; i < b; i++)
		{
			if (value % power2 == 0)
				return i;
			value /= power2;
		}
		return -1;
	}

	/// <summary>
	/// ��������� fingerprint � ������ � ������������ bucket
	/// � ���������� ��������.
	/// </summary>
	/// <param name="fingerprint">fingerprint ������� ����������</param>
	/// <param name="bucketIndex">������ ������� bucket</param>
	/// <param name="entryIndex">������ ������ ������ � ���������� bucket</param>
	void AddToBucket(int fingerprint, int bucketIndex, int entryIndex) {
		if (fingerprint > 127)
			throw "ArgumentException (AddToBucket): fingerprint must be below or equal 127";
		if (entryIndex > 3)
			throw "ArgumentException (AddToBucket): entryIndex must be below or equal 3";
		if (bucketIndex > countBuckets)
			throw "ArgumentException (AddToBucket): bucketIndex must be below countBuckets";

		for (int i = entryIndex * 7; i < entryIndex * 7 + 7; i++)
		{
			buckets[bucketIndex] += int(pow(2, i)) * (fingerprint % 2); //���������� ���� ��� fingerprint
			fingerprint >>= 1; //��������� � ���������� ����
		}
	}

	/// <summary>
	/// ���������� �������� ������ ��� �������� entryIndex � ������ � �������� bucketIndex
	/// </summary>
	/// <param name="bucketIndex"></param>
	/// <param name="entryIndex"></param>
	/// <returns></returns>
	int GetValueFromEntry(int bucketIndex, int entryIndex) {
		if (entryIndex > 3)
			throw "ArgumentException (GetValueFromEntry): entryIndex must be below or equal 3";
		if (bucketIndex > countBuckets)
			throw "ArgumentException (GetValueFromEntry): bucketIndex must be below countBuckets";

		return buckets[bucketIndex] / int(pow(2, 7 * entryIndex)) % int(pow(2, 7));
	}

	/// <summary>
	/// ������ �������� ������ ��� �������� entryIndex � ������ � �������� bucketIndex
	/// �� fingerprint � ���������� �������� ��������.
	/// </summary>
	/// <param name="fingerprint"></param>
	/// <param name="bucketIndex"></param>
	/// <param name="entryIndex"></param>
	/// <returns></returns>
	int SwapValues(int fingerprint, int bucketIndex, int entryIndex) {
		int entryValue = GetValueFromEntry(bucketIndex, entryIndex);
		buckets[bucketIndex] -= entryValue * int(pow(2, entryIndex * 7));
		AddToBucket(fingerprint, bucketIndex, entryIndex);
		return entryValue;
	}

	/// <summary>
	/// ���������� ���� �� ���������� fingerprint � bucket
	/// </summary>
	/// <param name="fingerprint"></param>
	/// <param name="bucketIndex"></param>
	/// <returns></returns>
	bool BucketHasFingerprint(int fingerprint, int bucketIndex) {
		for (int i = 0; i < 4; i++)
			if (GetValueFromEntry(bucketIndex, i) == fingerprint)
				return true;
		return false;
	}

public:
	/// <summary>
	/// ������� �������� ������ ������ �������, ����� ���� ����� �����������
	/// n ���������
	/// </summary>
	/// <param name="n"></param>
	CuckooFilter(int n) {
		int m = (1 + fpr) * n + 1; //���������� ��������� ���-�� ��-�� � �������
		countBuckets = MinPow2MoreThan(m);
		buckets = new unsigned int[countBuckets];
		ClearBuckets();
	}

	/// <summary>
	/// ���������� false ���� �������� ��� � ������� � true
	/// ���� �� �������� ����
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	bool Lookup(const std::string str) {
		int fingerprint = Fingerprint(str);
		int i1 = Hash(str);
		int i2 = i1 ^ Hash(fingerprint);

		return BucketHasFingerprint(fingerprint, i1) || BucketHasFingerprint(fingerprint, i2);
	}


	void PrintBucket() {
		for (int i = 0; i < countBuckets; ++i) {
			std::cout << buckets[i] << " ";
		}
		std::cout << std::endl;
	}

	/// <summary>
	/// ��������� ���������� ������� � �������� ������
	/// </summary>
	/// <param name="object"></param>
	void Add(const std::string str) {
		int fingerprint = Fingerprint(str);
		int i1 = Hash(str);
		int i2 = i1 ^ Hash(fingerprint);

		//��������� ���� �� � ������� ������� � ����� �� fingerprint-�� � ���� ����, ��
		//�� ��������� ���. ��� ��� � ��� �� ���������� �������� ���������, �� ��� ������
		//�� �������. (���� ��������� ������� ����������� � Teams)
		if (BucketHasFingerprint(fingerprint, i1) || BucketHasFingerprint(fingerprint, i2))
			return;

		//��������� �������
		if (EmptyBucketEntry(i1) > -1) {
			AddToBucket(fingerprint, i1, EmptyBucketEntry(i1));
			return;
		}

		if (EmptyBucketEntry(i2) > -1) {
			AddToBucket(fingerprint, i2, EmptyBucketEntry(i2));
			return;
		}

		//��������� ������������
		int bucketInd = i1;
		for (int i = 0; i < 500; i++)
		{
			fingerprint = SwapValues(fingerprint, bucketInd, 0);
			bucketInd ^= std::hash<int>{}(fingerprint);
			int entryInd = EmptyBucketEntry(bucketInd);
			if (entryInd > -1)
			{
				AddToBucket(fingerprint, bucketInd, entryInd);
				return;
			}
		}
	}

	~CuckooFilter() {
		delete[] buckets;
	}
};

class User {
private:
	CuckooFilter viewedVideo;

public:
	User(int countVideos) : viewedVideo(countVideos) {

	}

	/// <summary>
	/// ��������� ����� � ������ ������������� ������������� �����
	/// </summary>
	/// <param name="video"></param>
	void AddVideo(std::string video) {
		viewedVideo.Add(video);
	}

	/// <summary>
	/// ��������� ������� �� ������������ ���������� �����
	/// </summary>
	/// <param name="video"></param>
	/// <returns></returns>
	std::string CheckVideo(std::string video) {
		return viewedVideo.Lookup(video) ? "Probably" : "No";
	}
};

class Command {
protected:
	static std::map<std::string, std::unique_ptr<User>> users; //������������
	static int countVideos; //���������� ����� � ����� ��������
	std::string command; //�������� �������

public:
	/// <summary>
	/// ������� ������� �� �� ���������� �������������
	/// </summary>
	/// <param name="commandVec"></param>
	Command(std::vector<std::string> commandVec) {
		this->command = commandVec[0];
		if (this->command == "videos")
		{
			countVideos = std::stoi(commandVec[1]); //��������� ���������� �����
		}
	}

	/// <summary>
	/// ��������� �������
	/// </summary>
	/// <returns></returns>
	virtual std::string Invoke() {
		return "Ok";
	}
};

//�������������� ����������� ����
int Command::countVideos = 0;
std::map<std::string, std::unique_ptr<User>> Command::users = std::map<std::string, std::unique_ptr<User>>();

class UserCommand : public Command {
private:
	std::string user;
	std::string video;

public:
	/// <summary>
	/// ������� ������� ����������������� � �������������
	/// </summary>
	/// <param name="commandVec"></param>
	UserCommand(std::vector<std::string> commandVec) : Command(commandVec) {
		user = commandVec[1];
		video = commandVec[2];
		if (users.find(user) == users.end()) //���� user-� ��� � �������
			users.insert(std::pair<std::string, std::unique_ptr<User>>(user, new User(countVideos)));
	}

	/// <summary>
	/// ��������� �������
	/// </summary>
	/// <returns></returns>
	virtual std::string Invoke() {
		if (command == "watch") {
			users[user]->AddVideo(video);
			return "Ok";
		}
		else if (command == "check")
			return users[user]->CheckVideo(video);
		return "incorrect command";
	}
};

/// <summary>
/// ��������� ������ �� vector �� �����������
/// </summary>
/// <param name="str">������</param>
/// <param name="delimiter">�����������</param>
/// <returns>vector �������� ������</returns>
std::vector<std::string> Split(std::string str, std::string delimiter) {
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
/// ������� ������� �� �� ���������� ������������� � ����������
/// ������ �� ���
/// </summary>
/// <param name="command">��������� ������������� �������</param>
/// <returns></returns>
std::unique_ptr<Command> CreateCommand(std::string command) {
	if (command.find('\r') != std::string::npos)
		command.erase(command.find('\r'), 1);
	std::vector<std::string> commandVec = Split(command, " ");
	if (commandVec.back() == "")
		commandVec.pop_back();

	if (commandVec.size() == 2) {
		std::unique_ptr<Command> command(new Command(commandVec));
		return command;
	}
	else {
		std::unique_ptr<Command> command(new UserCommand(commandVec));
		return command;
	}
}

int main(int argc, char* argv[]) {

	//��������� ���������� ���������� ����������
	if (argc != 3) {
		std::cout << "Incorrect input" << std::endl;
		return 1;
	}

	//������� ����� � ���������, ��� �� ������� ������ � �����
	std::ifstream fs(argv[1], std::ios::in | std::ios::binary);
	if (!fs)
	{
		std::cout << "Could not open input file";
		return 1;
	}

	//��������� ���������� �� �����
	std::vector<std::unique_ptr<Command>> commands;
	int i = 0;
	while (!fs.eof())
	{
		i++;
		std::string str;
		getline(fs, str);
		if (str != "")
			commands.push_back(CreateCommand(str));
	}

	fs.close();

	//��������� ���� �� ������ 
	std::ofstream out;
	out.open(argv[2]);
	if (!out) {
		std::cout << "Could not open output file";
		return 1;
	}

	//���������� ������ ������ � ����
	for (int i = 0; i < commands.size(); i++)
		out << commands[i]->Invoke() << std::endl;

	out.close();

	return 0;
}