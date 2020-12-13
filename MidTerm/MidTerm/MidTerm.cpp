#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<string.h>
#include<stdlib.h>
#define MAX_BYTE 71		//기본으로 출력되는 라인 번호와 공백문자("번호| " = 4바이트) 때문에 71바이트로 설정
#define MAX_LINE 20

std::vector<std::string> word;				//단어들을 저장 할 백터
int first_word_of_line[MAX_LINE]{ -1 };		//페이지의 문장별 첫번째 단어의 위치(인덱스)를 저장하는 배열

int first_word_of_previous_page = -1;		//이전 페이지의 첫번째 단어의 위치(인덱스) 최초 위치 = -1
int first_word_of_next_line = 0;			//다음 문장의 첫 번째 단어의 위치(인덱스)

void run(int start, std::string Message);	//실행함수
bool isnotnum(std::string arg);				//입력된 문자열이 숫자인지 판별하는 함수


class Strategy			//Strategy 패턴 class
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
	~Context() { if (strategy) delete strategy; }		//소멸자

	void executeStrategy(const std::string cmdline) {
		(*strategy).doOperation(cmdline);
	}
};

Context urCammand(const std::string cmdline);

class OperationInsert : public Strategy {						//삽입 알고리즘 class
public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("잘못된 입력입니다.(괄호 미포함)");			//잘못된 내용 오류 처리

		std::string temp = cmdline.substr(2, ((std::string)cmdline).size() - 3);	//cmd 괄호 안 내용
		std::istringstream parser(temp);
		std::string stringBuffer;
		std::vector<std::string> args;	//힘수 인자 저장용 백터

		while (getline(parser, stringBuffer, ','))			//함수 인자 파싱
			args.push_back(stringBuffer);					//args[0] = 라인 번호, args[1] = 라인에서 삽입할 단어 위치, args[2] = 삽입할 단어

		if (args.size() != 3 || isnotnum(args[0]) || isnotnum(args[1]) ||						//인자가 3개가 아닐때, 첫 번째, 두번째 인자가 숫자가 아닐때,
			stoi(args[0]) <= 0 || stoi(args[0]) > MAX_LINE || args[2].size() > MAX_BYTE ||			//첫 번째 인자가 양수가 아니거나 허용 줄 수 보다 큰값 일때, 단어의 길이가 허용 바이트 수보다 길 때
			stoi(args[1]) <= 0 || stoi(args[1]) >= first_word_of_line[stoi(args[0]) + 1])			//두 번째 인자가 양수가 아닐때, 두 번째 인자가 입력된 줄 번호에 없는 순서의 값일 때
			throw std::invalid_argument("잘못된 입력입니다.");									//오류 발생

		int target_index = first_word_of_line[std::stoi(args[0]) - 1] + std::stoi(args[1]) - 1;	//단어를 삽입할 위치
		int aim = 0;
		for (std::vector<std::string>::iterator it = word.begin(); it != word.end(); it++) {	//target_index에 단어 삽입 후 종료
			if (aim == target_index) {
				it = word.insert(it, args[2]);
				break;
			}
			aim++;
		}

		run(first_word_of_line[0], "지정한 위치에 " + args[2] + "가 추가 되었습니다.");			//메시지와 함께 해당 페이지 재출력
	}
};

class OperationDelete : public Strategy {						//삭제 알고리즘 class
public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("잘못된 입력입니다.(괄호 미포함)");					//잘못된 내용 오류 처리

		std::string temp = cmdline.substr(2, ((std::string)cmdline).size() - 3);			//cmd 괄호 안 내용
		std::istringstream parser(temp);
		std::string stringBuffer;
		std::vector<int> args;							//힘수 인자 저장용 백터

		while (getline(parser, stringBuffer, ',')) {
			if (isnotnum(stringBuffer)) throw std::invalid_argument("잘못된 입력입니다.");	//함수 인자가 숫자가 아니면 예외 발생
			args.push_back(std::stoi(stringBuffer));										//args[0] = 라인 번호, args[1] 라인에서 삭제할 단어 위치
		}


		if (args.size() != 2 || (args[0]) <= 0 || (args[0]) > MAX_LINE ||				//함수인자가 2개가 아닐 때, 라인 번호가 양수가 아니거나 최대 허용 라인 번호보다 큰 값이 들어 올 때
			args[1] <= 0 || first_word_of_line[args[0]] + args[1] >= first_word_of_line[args[0] + 1])		//단어의 위치가 양수가 아니거나 해당 라인에 없는 순서의 위치일 때
			throw std::invalid_argument("잘못된 입력입니다.");							//예외 발생

		int target_index = first_word_of_line[args[0] - 1] + args[1] - 1;				//삭제할 단어의 위치
		int aim = 0;
		for (std::vector<std::string>::iterator it = word.begin(); it != word.end(); it++) {
			if (aim == target_index) {
				it = word.erase(it);					//단어 삭제
				break;
			}
			aim++;
		}

		run(first_word_of_line[0], "지정한 위치에 단어가 삭제 되었습니다.");		//메시지와 함께 해당 페이지 재출력
	}
};

class OperationChange : public Strategy {						//단어 변경 알고리즘 class

public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("잘못된 입력입니다.(괄호 미포함)");				//잘못된 내용 오류 처리

		std::string temp = cmdline.substr(2, ((std::string)cmdline).size() - 3);			//cmd 괄호 안 내용
		std::istringstream parser(temp);
		std::string stringBuffer;
		std::vector<std::string> args;					//함수 인자 저장용 백터

		while (getline(parser, stringBuffer, ','))
			args.push_back(stringBuffer);

		if (args.size() != 2 || args[1].size() > MAX_BYTE)				//함수의 인자가 2개가 아닐 때, 바꾸려는 단어의 크기가 허용 바이트보다 클때
			throw std::invalid_argument("잘못된 입력입니다.");			//오류 발생

		std::string target = args[0];					//바꾸려는 단어
		std::string src = args[1];						//바꿀 단어
		bool neverchanged = true;						//단어 교체 여부
		for (int i = 0; i < word.size(); i++) {
			if (word[i].compare(target) == 0) {				//단어 발견
				word[i] = src;								//단어 교체
				if (neverchanged)  neverchanged = false;		//단어 교체가 한 번이라도 발생하면 상태 변경
			}
		}
		if (neverchanged) throw std::invalid_argument("바꾸려는 단어가 없습니다.");	//단어 교체가 한 번도 발생하지 않으면 예외 발생
		run(first_word_of_line[0], target + " -> " + src + " 변경 완료");				//메시지와 함께 해당 페이지 재출력
	}
};

class OperationSearch : public Strategy {

public:
	void doOperation(const std::string cmdline) {
		if (cmdline[1] != '(' || cmdline[cmdline.size() - 1] != ')')
			throw std::invalid_argument("잘못된 입력입니다.(괄호 미포함)");				//잘못된 내용 오류 처리

		std::string target = cmdline.substr(2, ((std::string)cmdline).size() - 3);			//cmd 괄호 안 내용(찾으려는 단어)

		int index = (std::find(word.begin(), word.end(), target)) - word.begin();				//찾으려는 단어의 위치
		if (index == word.size()) throw std::invalid_argument("찾으시는 단어가 없습니다.");		//단어 발견 실패 시 예외 발생

		run(index, "");				//메시지와 함께 해당 페이지 재출력
	}
};

class OperationNext : public Strategy {			//다음 페이지 알고리즘 calss

public:
	void doOperation(const std::string cmdline) {
		if (cmdline.size() > 1)throw std::invalid_argument("잘못된 입력입니다.");				//잘못된 내용 오류 처리

		run(first_word_of_next_line, "");				//메시지와 함께 해당 페이지 재출력

	}
};

class OperationPrev : public Strategy {			//이전 페이지 알고리즘 calss

public:
	void doOperation(const std::string cmdline) {
		if (cmdline.size() > 1) throw std::invalid_argument("잘못된 입력입니다.");					//잘못된 내용 오류 처리
		if (first_word_of_previous_page < 0) throw std::invalid_argument("이전 페이지가 없습니다.");	//첫페이지에서 

		run(first_word_of_previous_page, "");				//메시지와 함께 해당 페이지 재출력
	}
};

class OperationTerminate : public Strategy {	//저장 후 종료 알고리즘 class

public:
	void doOperation(const std::string cmdline) {
		if (cmdline.size() > 1)throw std::invalid_argument("잘못된 입력입니다.");	//잘못된 입력 오류처리

		std::ofstream out;
		out.open("test.txt");							//기존 텍스트 파일

		std::string newTxt;
		for (int i = 0; i < word.size(); i++)
			newTxt += (word[i] + " ");					//수정된 텍스트 파일 내용
		if (out.is_open())
			out << newTxt;								//수정된 내용을 기존 파일에 저장
	}
};


int main() {

	std::ifstream in;
	in.open("test.txt");		//읽어들이는 파일
	std::string temp;			//임시 저장 문자열


	if (in.is_open()) {
		while (in >> temp)
			word.push_back(temp);					//모든 단어를 백터에 저장
	}

	run(0, "");						//텍스트 편집기 실행

	return 0;
}

void run(int start, std::string Message) {					//시작하는 단어의 위치와 출력할 메시지를 인자로 받는다

	std::string str = "";									//임시 문장
	first_word_of_previous_page = first_word_of_line[0];	//페이지 갱신 전 현재 페이지의 첫번재 단어 저장(갱신 후에는 이전 ㅍ이지의 첫 번째 단어가 된다.)
	std::string cmdline;									//사용자 입력 명령어
	first_word_of_line[0] = start;							//페이지 첫 줄의 첫 번째 단어

	for (int line = 0, i = start; line < MAX_LINE && i < word.size();) {		//MAX_LINE개의 문장만 생산
		if ((str + word[i]).size() < MAX_BYTE) {			//(임시 문장+다음 단어)의 길이가 MAX_BYTE보다 짧으면 
			str += (word[i++] + " ");						//다음 단어를 임시문장에 추가
			if (i == word.size()) {
				printf("%2d| ", line + 1);
				std::cout << str << std::endl;
			}	//마지막 문장의 길이가 MAX_BYTE보다 짧고 백터의 모든 단어를 꺼냈을때는 문장은 생산되지만 출력이 안되므로 위 조건문을 추가해 마지막 문장을 출력
		}
		else {												//(임시 문장+다음 단어)의 길이가 MAX_BYTE보다 길면 문장 생산 완료
			printf("%2d| ", line + 1);							//문장번호
			std::cout << str << std::endl;					//문장출력
			str = "";										//임시 문장 초기화

			if (line + 1 < MAX_LINE)
				first_word_of_line[line + 1] = i;				//문장의 첫번째 단어의 위치(인덱스)를 해당 배열에 저장
			first_word_of_next_line = i;					//다음 문장의 첫번째 단어의 위치로 한 페이지 출력 후에는 다음페이지의 첫 번째 단어의 위치가 저장된다.
			line++;											//문장 증가
		}

	}
	puts("-----------------------------------------------------------------------------------");
	std::cout << "n: 다음페이지, p: 이전페이지, i:삽입, d:삭제, c:변경, s:찾기, t:저장후종료" << std::endl;
	puts("-----------------------------------------------------------------------------------");
	std::cout << "(콘솔메세지) " << Message << std::endl;		//콘솔 메시지 출력
	puts("-----------------------------------------------------------------------------------");
	std::cout << "입력:";
	std::cin >> cmdline;		//유저 명령어 입력
	puts("-----------------------------------------------------------------------------------");

	try {
		Context cxt = urCammand(cmdline);					//커맨드에 맞는 context
		cxt.executeStrategy(cmdline);						//context의 strategy연산 실행
	}
	catch (std::invalid_argument& e) { run(start, e.what()); }	//커맨드 입력이 잘못되면 프로그램 메시지와 함께 창 재출력

	puts("-----------------------------------------------------------------------------------");

}


Context urCammand(const std::string cmdline) {					//커맨드에 따라 그에 맞는 strategy를 갖는 context 리턴
	char cmd = cmdline[0];										//유저 입력 명령어 중 첫번째 글자

	switch (cmd) {
	case 's': return Context(new OperationSearch);				//단어 탐색 알고리즘 Context 리턴
	case 'd': return Context(new OperationDelete);				//단어 삭제 알고리즘 Context 리턴
	case 'i': return Context(new OperationInsert);				//단어 삽입 알고리즘 Context 리턴
	case 'c': return Context(new OperationChange);				//단어 변경 알고리즘 Context 리턴
	case 'n': return Context(new OperationNext);				//다음 페이지 이동 알고리즘 Context 리턴
	case 'p': return Context(new OperationPrev);				//이전 페이지 이동 알고리즘 Context 리턴
	case 't': return Context(new OperationTerminate);			//종료 알고리즘 Context 리턴
	default: throw std::invalid_argument("Invalid Input 다시 입력해주세요.");	//예외 발생: 잘못된 명령어 
	}
}


bool isnotnum(std::string arg) { return (atoi(arg.c_str()) == 0) && (arg.compare("0") != 0); }	//인자가 숫자인지 아닌자 판별