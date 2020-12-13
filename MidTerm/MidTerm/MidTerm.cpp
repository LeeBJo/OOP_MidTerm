#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string.h>
#include<stdlib.h>
#define MAX_BYTE 71		//�⺻���� ��µǴ� ���� ��ȣ�� ���鹮��("��ȣ| " = 4����Ʈ) ������ 71����Ʈ�� ����
#define MAX_LINE 20

std::vector<std::string> word;				//�ܾ���� ���� �� ����
int first_word_of_line[MAX_LINE]{ -1 };		//�������� ���庰 ù��° �ܾ��� ��ġ(�ε���)�� �����ϴ� �迭

int first_word_of_previous_page = -1;		//���� �������� ù��° �ܾ��� ��ġ(�ε���) ���� ��ġ = -1
int first_word_of_next_line = 0;			//���� ������ ù ��° �ܾ��� ��ġ(�ε���)

void run(int start, std::string Message);	//�����Լ�
bool isnotnum(std::string arg);				//�Էµ� ���ڿ��� �������� �Ǻ��ϴ� �Լ�


class Strategy			//Strategy ���� class
{
public:
	virtual void doOperation(const std::string cmdline) {  }
};

class Context {
private:
	Strategy* strategy;
public:
	Context(Strategy* strategy) {
		this->strategy = strategy;
	}
	~Context() { if (strategy) delete strategy; }		//�Ҹ���

	void executeStrategy(const std::string cmdline) {
		(*strategy).doOperation(cmdline);
	}
};

Context urCammand(const std::string cmdline);

class OperationInsert : public Strategy {						//���� �˰��� class
public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("�߸��� �Է��Դϴ�.(��ȣ ������)");			//�߸��� ���� ���� ó��

		std::string temp = cmdline.substr(2, ((std::string)cmdline).size() - 3);	//cmd ��ȣ �� ����
		std::istringstream parser(temp);
		std::string stringBuffer;
		std::vector<std::string> args;	//���� ���� ����� ����

		while (getline(parser, stringBuffer, ','))			//�Լ� ���� �Ľ�
			args.push_back(stringBuffer);					//args[0] = ���� ��ȣ, args[1] = ���ο��� ������ �ܾ� ��ġ, args[2] = ������ �ܾ�

		if (args.size() != 3 || isnotnum(args[0]) || isnotnum(args[1]) ||						//���ڰ� 3���� �ƴҶ�, ù ��°, �ι�° ���ڰ� ���ڰ� �ƴҶ�,
			stoi(args[0]) <= 0 || stoi(args[0]) > MAX_LINE || args[2].size() > MAX_BYTE ||			//ù ��° ���ڰ� ����� �ƴϰų� ��� �� �� ���� ū�� �϶�, �ܾ��� ���̰� ��� ����Ʈ ������ �� ��
			stoi(args[1]) <= 0 || stoi(args[1]) >= first_word_of_line[stoi(args[0]) + 1])			//�� ��° ���ڰ� ����� �ƴҶ�, �� ��° ���ڰ� �Էµ� �� ��ȣ�� ���� ������ ���� ��
			throw std::invalid_argument("�߸��� �Է��Դϴ�.");									//���� �߻�

		int target_index = first_word_of_line[std::stoi(args[0]) - 1] + std::stoi(args[1]) - 1;	//�ܾ ������ ��ġ
		int aim = 0;
		for (std::vector<std::string>::iterator it = word.begin(); it != word.end(); it++) {	//target_index�� �ܾ� ���� �� ����
			if (aim == target_index) {
				it = word.insert(it, args[2]);
				break;
			}
			aim++;
		}

		run(first_word_of_line[0], "������ ��ġ�� " + args[2] + "�� �߰� �Ǿ����ϴ�.");			//�޽����� �Բ� �ش� ������ �����
	}
};

class OperationDelete : public Strategy {						//���� �˰��� class
public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("�߸��� �Է��Դϴ�.(��ȣ ������)");					//�߸��� ���� ���� ó��

		std::string temp = cmdline.substr(2, ((std::string)cmdline).size() - 3);			//cmd ��ȣ �� ����
		std::istringstream parser(temp);
		std::string stringBuffer;
		std::vector<int> args;							//���� ���� ����� ����

		while (getline(parser, stringBuffer, ',')) {
			if (isnotnum(stringBuffer)) throw std::invalid_argument("�߸��� �Է��Դϴ�.");	//�Լ� ���ڰ� ���ڰ� �ƴϸ� ���� �߻�
			args.push_back(std::stoi(stringBuffer));										//args[0] = ���� ��ȣ, args[1] ���ο��� ������ �ܾ� ��ġ
		}


		if (args.size() != 2 || (args[0]) <= 0 || (args[0]) > MAX_LINE ||				//�Լ����ڰ� 2���� �ƴ� ��, ���� ��ȣ�� ����� �ƴϰų� �ִ� ��� ���� ��ȣ���� ū ���� ��� �� ��
			args[1] <= 0 || first_word_of_line[args[0]] + args[1] >= first_word_of_line[args[0] + 1])		//�ܾ��� ��ġ�� ����� �ƴϰų� �ش� ���ο� ���� ������ ��ġ�� ��
			throw std::invalid_argument("�߸��� �Է��Դϴ�.");							//���� �߻�

		int target_index = first_word_of_line[args[0] - 1] + args[1] - 1;				//������ �ܾ��� ��ġ
		int aim = 0;
		for (std::vector<std::string>::iterator it = word.begin(); it != word.end(); it++) {
			if (aim == target_index) {
				it = word.erase(it);					//�ܾ� ����
				break;
			}
			aim++;
		}

		run(first_word_of_line[0], "������ ��ġ�� �ܾ ���� �Ǿ����ϴ�.");		//�޽����� �Բ� �ش� ������ �����
	}
};

class OperationChange : public Strategy {						//�ܾ� ���� �˰��� class

public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("�߸��� �Է��Դϴ�.(��ȣ ������)");				//�߸��� ���� ���� ó��

		std::string temp = cmdline.substr(2, ((std::string)cmdline).size() - 3);			//cmd ��ȣ �� ����
		std::istringstream parser(temp);
		std::string stringBuffer;
		std::vector<std::string> args;					//�Լ� ���� ����� ����

		while (getline(parser, stringBuffer, ','))
			args.push_back(stringBuffer);

		if (args.size() != 2 || args[1].size() > MAX_BYTE)				//�Լ��� ���ڰ� 2���� �ƴ� ��, �ٲٷ��� �ܾ��� ũ�Ⱑ ��� ����Ʈ���� Ŭ��
			throw std::invalid_argument("�߸��� �Է��Դϴ�.");			//���� �߻�

		std::string target = args[0];					//�ٲٷ��� �ܾ�
		std::string src = args[1];						//�ٲ� �ܾ�
		bool neverchanged = true;						//�ܾ� ��ü ����
		for (int i = 0; i < word.size(); i++) {
			if (word[i].compare(target) == 0) {				//�ܾ� �߰�
				word[i] = src;								//�ܾ� ��ü
				if (neverchanged)  neverchanged = false;		//�ܾ� ��ü�� �� ���̶� �߻��ϸ� ���� ����
			}
		}
		if (neverchanged) throw std::invalid_argument("�ٲٷ��� �ܾ �����ϴ�.");	//�ܾ� ��ü�� �� ���� �߻����� ������ ���� �߻�
		run(first_word_of_line[0], target + " -> " + src + " ���� �Ϸ�");				//�޽����� �Բ� �ش� ������ �����
	}
};

class OperationSearch : public Strategy {

public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("�߸��� �Է��Դϴ�.(��ȣ ������)");				//�߸��� ���� ���� ó��

		std::string target = cmdline.substr(2, ((std::string)cmdline).size() - 3);			//cmd ��ȣ �� ����(ã������ �ܾ�)

		int index = (std::find(word.begin(), word.end(), target)) - word.begin();				//ã������ �ܾ��� ��ġ
		if (index == word.size()) throw std::invalid_argument("ã���ô� �ܾ �����ϴ�.");		//�ܾ� �߰� ���� �� ���� �߻�

		run(index, "");				//�޽����� �Բ� �ش� ������ �����
	}
};

class OperationNext : public Strategy {			//���� ������ �˰��� calss

public:
	void doOperation(const std::string cmdline) {
		if (cmdline.size() > 1)throw std::invalid_argument("�߸��� �Է��Դϴ�.");				//�߸��� ���� ���� ó��

		run(first_word_of_next_line, "");				//�޽����� �Բ� �ش� ������ �����

	}
};

class OperationPrev : public Strategy {			//���� ������ �˰��� calss

public:
	void doOperation(const std::string cmdline) {
		if (cmdline.size() > 1) throw std::invalid_argument("�߸��� �Է��Դϴ�.");					//�߸��� ���� ���� ó��
		if (first_word_of_previous_page < 0) throw std::invalid_argument("���� �������� �����ϴ�.");	//ù���������� 

		run(first_word_of_previous_page, "");				//�޽����� �Բ� �ش� ������ �����
	}
};

class OperationTerminate : public Strategy {	//���� �� ���� �˰��� class

public:
	void doOperation(const std::string cmdline) {
		if (cmdline.size() > 1)throw std::invalid_argument("�߸��� �Է��Դϴ�.");	//�߸��� �Է� ����ó��

		std::ofstream out;
		out.open("test.txt");							//���� �ؽ�Ʈ ����

		std::string newTxt;
		for (int i = 0; i < word.size(); i++)
			newTxt += (word[i] + " ");					//������ �ؽ�Ʈ ���� ����
		if (out.is_open())
			out << newTxt;								//������ ������ ���� ���Ͽ� ����
	}
};


int main() {

	std::ifstream in;
	in.open("test.txt");		//�о���̴� ����
	std::string temp;			//�ӽ� ���� ���ڿ�


	if (in.is_open()) {
		while (in >> temp)
			word.push_back(temp);					//��� �ܾ ���Ϳ� ����
	}

	run(0, "");						//�ؽ�Ʈ ������ ����

	return 0;
}

void run(int start, std::string Message) {					//�����ϴ� �ܾ��� ��ġ�� ����� �޽����� ���ڷ� �޴´�

	std::string str = "";									//�ӽ� ����
	first_word_of_previous_page = first_word_of_line[0];	//������ ���� �� ���� �������� ù���� �ܾ� ����(���� �Ŀ��� ���� �������� ù ��° �ܾ �ȴ�.)
	std::string cmdline;									//����� �Է� ��ɾ�
	first_word_of_line[0] = start;							//������ ù ���� ù ��° �ܾ�

	for (int line = 0, i = start; line < MAX_LINE && i < word.size();) {		//MAX_LINE���� ���常 ����
		if ((str + word[i]).size() < MAX_BYTE) {			//(�ӽ� ����+���� �ܾ�)�� ���̰� MAX_BYTE���� ª���� 
			str += (word[i++] + " ");						//���� �ܾ �ӽù��忡 �߰�
			if (i == word.size()) {
				printf("%2d| ", line + 1);
				std::cout << str << std::endl;
			}	//������ ������ ���̰� MAX_BYTE���� ª�� ������ ��� �ܾ ���������� ������ ��������� ����� �ȵǹǷ� �� ���ǹ��� �߰��� ������ ������ ���
		}
		else {												//(�ӽ� ����+���� �ܾ�)�� ���̰� MAX_BYTE���� ��� ���� ���� �Ϸ�
			printf("%2d| ", line + 1);							//�����ȣ
			std::cout << str << std::endl;					//�������
			str = "";										//�ӽ� ���� �ʱ�ȭ

			if (line + 1 < MAX_LINE)
				first_word_of_line[line + 1] = i;				//������ ù��° �ܾ��� ��ġ(�ε���)�� �ش� �迭�� ����
			first_word_of_next_line = i;					//���� ������ ù��° �ܾ��� ��ġ�� �� ������ ��� �Ŀ��� ������������ ù ��° �ܾ��� ��ġ�� ����ȴ�.
			line++;											//���� ����
		}

	}
	puts("-----------------------------------------------------------------------------------");
	std::cout << "n: ����������, p: ����������, i:����, d:����, c:����, s:ã��, t:����������" << std::endl;
	puts("-----------------------------------------------------------------------------------");
	std::cout << "(�ָܼ޼���) " << Message << std::endl;		//�ܼ� �޽��� ���
	puts("-----------------------------------------------------------------------------------");
	std::cout << "�Է�:";
	std::cin >> cmdline;		//���� ��ɾ� �Է�
	puts("-----------------------------------------------------------------------------------");

	try {
		Context cxt = urCammand(cmdline);					//Ŀ�ǵ忡 �´� context
		cxt.executeStrategy(cmdline);						//context�� strategy���� ����
	}
	catch (std::invalid_argument& e) { run(start, e.what()); }	//Ŀ�ǵ� �Է��� �߸��Ǹ� ���α׷� �޽����� �Բ� â �����

	puts("-----------------------------------------------------------------------------------");

}


Context urCammand(const std::string cmdline) {					//Ŀ�ǵ忡 ���� �׿� �´� strategy�� ���� context ����
	char cmd = cmdline[0];										//���� �Է� ��ɾ� �� ù��° ����

	switch (cmd) {
	case 's': return Context(new OperationSearch);				//�ܾ� Ž�� �˰��� Context ����
	case 'd': return Context(new OperationDelete);				//�ܾ� ���� �˰��� Context ����
	case 'i': return Context(new OperationInsert);				//�ܾ� ���� �˰��� Context ����
	case 'c': return Context(new OperationChange);				//�ܾ� ���� �˰��� Context ����
	case 'n': return Context(new OperationNext);				//���� ������ �̵� �˰��� Context ����
	case 'p': return Context(new OperationPrev);				//���� ������ �̵� �˰��� Context ����
	case 't': return Context(new OperationTerminate);			//���� �˰��� Context ����
	default: throw std::invalid_argument("Invalid Input �ٽ� �Է����ּ���.");	//���� �߻�: �߸��� ��ɾ� 
	}
}


bool isnotnum(std::string arg) { return (atoi(arg.c_str()) == 0) && (arg.compare("0") != 0); }	//���ڰ� �������� �ƴ��� �Ǻ�