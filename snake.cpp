#define _CRT_SECURE_NO_WARNINGS
#include<bits/stdc++.h> 
#include <windows.h>
#include<iomanip>
#include <iostream>
#include <chrono>
#include <conio.h>
#include <deque>
#include <queue>
#include <stack>
#include <thread>
#include <sys/timeb.h>
#include <set>
#include<vector>
#include <fstream>
using namespace std; //ʹ�������ռ�std	
const int board_len = 20; //���̵ĳ���
struct inf_node {
	int goal;
	double sec;
	bool operator<(const inf_node& other) const {
		return goal > other.goal;
	}
};
struct auto_item {
	int data;
	pair<int, int> dis;
	bool operator<(const auto_item& other) const {
		return data > other.data;
	}
};
bool board[board_len + 10][board_len + 10]; //���̳�ʼֵ��Ϊfalse����bool���ͱ�ʾ�Ƿ�������/������
bool have_food; //�Ƿ���������ʳ�� - ��Ϊ�����ϱ���Ҫ��һ��ʳ��
bool is_wujin = false;
bool auto_move = false;
int is_diff = 0; //����ģʽ���ж�����ʼΪ0
int speed = 100; //��ʼˢ���ʣ�˯�߶��ٺ��룩
int jud = 0;
stack<int> next_direction; //�淽�� 
string dis_stand = "adws"; //�����������
vector<string> filename = { "normal.txt", "jixian.txt", "wujin.txt" };
set<inf_node> information[4];
int xx[] = { 0, 0, -1, 1 };
int yy[] = { -1, 1, 0, 0 }; // ����ƶ��ģ�ֻ��Ҫ�ض�����������dis_stand������
deque<pair<int, int>> snake_body; //���� -- һ��˫�˶���
pair<int, int> food;  //ʳ���λ��(x, y)
string star = "***";
int len = 20;
int calc(int x) {
	int a = len - 6 - x;
	a /= 2;
	return a;
}
int dis_calc(pair<int, int> Start, pair<int, int> End) {
	return abs(Start.first - End.first) + abs(Start.second - End.second);
}
void clearFileContent(const std::string& filename) {
	// Open the file in out mode which will truncate the file
	std::ofstream ofs(filename, std::ofstream::out | std::ofstream::trunc);
	// The file is now cleared
	ofs.close();
}
void initial_board() {
	for (int i = 0; i <= board_len + 1; i++) {
		board[0][i] = true;
		board[i][0] = true;
		board[i][board_len + 1] = true;
		board[board_len + 1][i] = true;
	}
}
void find_next(pair<int, int> food) {
	bool vis[board_len + 2][board_len + 2] = { false };
	int step[(board_len + 1)][(board_len + 1)] = { -1 };
	board[snake_body.front().first][snake_body.front().second] = false;
	step[snake_body.front().first][snake_body.front().second] = 0;
	bool key = false;
	priority_queue<auto_item> pq;
	int cnt = 0;
	int fx = food.first;
	int fy = food.second;
	for (int i = 0; i < 4; i++) {
		if (board[fx + xx[i]][fy + yy[i]]) cnt++;
	}
	if (cnt >= 3) food = snake_body.back();
	vis[snake_body.front().first][snake_body.front().second] = true;
	bool ket = false;
	int tmp = 0;
	pq.push({ 0, snake_body.front() });
	while (!pq.empty()) {
		auto_item old = pq.top();
		int x = old.dis.first;
		int y = old.dis.second;
		pq.pop();
		for (int i = 0; i < 4; i++) {
			int nnx = x + xx[i];
			int nny = y + yy[i];
			pair<int, int> now = { nnx, nny };
			if ((!board[nnx][nny] || food == pair<int, int>(nnx, nny)) && !vis[nnx][nny] && nnx >= 1 && nny >= 1 && nnx <= board_len && nny <= board_len) {
				//cout << nx << " " << ny << "\n";
				step[nnx][nny] = step[x][y] + 1;
				int len = dis_calc(now, food); //̰�����Ž��� ������ʽ��ʵ���A*�죨��Ϊ��������·���� 
				pq.push({ len, {nnx, nny} });
				vis[now.first][now.second] = true;
			}
			if (now == food) {
				break;
			}
		}
	}
	queue<pair<int, int>> q;
	q.push(food);
	vis[food.first][food.second] = false;
	int tmp_step = step[food.first][food.second];
	//cout << step[food.first][food.second] << "\n";
	while (!q.empty()) { //�Ӻ���ǰ��ֻҪ��·��һ��������·������һ���ܻ�ȥ 
		pair<int, int> old = q.front();
		int x = old.first;
		int y = old.second;
		q.pop();
		for (int i = 0; i < 4; i++) {
			int nx = x + xx[i];
			int ny = y + yy[i];
			pair<int, int> it = { nx, ny };
			if (step[nx][ny] == tmp_step - 1 && tmp_step - 1 >= 0) {
				tmp_step--;
				if (i <= 1) i == 0 ? i = 1 : i = 0;
				else i == 2 ? i = 3 : i = 2;
				next_direction.push(i); //�÷�תһ�� 
				vis[nx][ny] = false;
				q.push(it);
				break; //ֱ���˳� 
			}
		}
	}
	board[snake_body.front().first][snake_body.front().second] = true;
}
void board_print() { //��ӡ����
	for (int i = 0; i <= board_len + 1; i++) cout << "-";
	cout << "\n";
	for (int i = 1; i <= board_len; i++) {//˫��ѭ��������������
		cout << "|";
		for (int j = 1; j <= board_len; j++) {
			pair<int, int> now = { i, j };
			if (board[i][j] && food == now) cout << "*"; //ʳ��������ǵ�ǰi��j
			else if ((board[i][j])) cout << "��";
			else cout << " ";
		}
		cout << "|\n"; //���̱߽�Ĵ���/����
	}
	for (int i = 0; i <= board_len + 1; i++) cout << "-";//���̱߽�Ĵ���/����
	cout << "\n";
}
void print_jinggao() {
	cout << "\n���棺��Ϸǰ������\n\n"
		<< "�м��������ڹۿ�ĳЩ�Ӿ�Ӱ��ʱ��\n\n"
		<< "���ܻ�ͻȻ�����������������Ϸ\n\n"
		<< "�е������ͼ�Ρ�����ʱ�����ǿ�����\n\n"
		<< "�����֢״����ʹ�����ʷ��Ҳ�п���\n\n"
		<< "�������Ʒ�Ӧ����������������ʷ��\n\n"
		<< "������ѯҽ����\n\n"
		<< "������Ϸ����������������۾��ۡ���\n\n"
		<< "���쳣��ƫͷʹ�����λ���ʶ�ϰ�����\n\n"
		<< "����ԣ���������ֹͣ����ѯҽ����\n\n"
		<< "���⣬���е�ͷʹ����ѣ�����Ļ��ʣ�\n\n"
		<< "��Ӧֹͣ��Ϸ��֢״�������뼰ʱ��ҽ��\n\n";
}
void pat(int a, int b, string s) {
	cout << star;
	for (int i = 0; i < a; i++)cout << " ";
	cout << s;
	for (int i = 0; i < b; i++) cout << " ";
	cout << star << "\n\n\n";
}
void print_jiesu() {
	system("cls");
	cout << "\n��Ϸ�����˳�...";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
	cout << "\n��Ϸ���˳���ף��������죡";
	this_thread::sleep_for(chrono::milliseconds(500));

}
void print_jiemian() {
	//cout << YELLOW;
	cout << "\n          ̰������Ϸ������          \n\n\n";
	std::string s;
	s = "----------------------------------";
	len = s.size();
	std::cout << s << "\n\n\n";

	string s1 = "1.��ģʽ";
	int pre = calc(s1.size());
	int bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "2.�е�ģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "3.����ģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "4.����ģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "5.�޾�ģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "6.��ʾģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "7.���а�";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "8.��Ϸ˵��";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "0.�˳���Ϸ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	std::cout << s << "\n\n";
	cout << "\n��ѡ������Ҫʹ�õĹ���:\n\n";
}
void do_game() {
	int initial_cnt = 3; //��ʼС�ߵ����峤��
	struct _timeb T;
    _ftime(&T);
    srand(T.millitm);
	int pos = 3; //С�ߵ�ǰ�ƶ�����Ĭ��3����x+xx[3](0) y+yy[3](1) �����ƶ�
	while (initial_cnt--) { //��ʼ����������
		int x = board_len / 2, y = board_len / 2 + initial_cnt; //��ʼ�����������
		snake_body.push_back({ x, y }); //�ŵ�β����ʹyֵ��С�� ��deque����β������Ϊ��ʼ���ң�����ͷ����y������
		board[x][y] = true;//����һ�����ͱ��һ��Ϊtrue���Ա�ɨ������ʱ��ӡ
	}
	char user_cin = 'd', old_cin = 'd'; int ran;
	int qiangcnt = 1; //����ģʽ��ǽ�Ĳ���
	while (1) {
		int x = snake_body.back().first; //��˫�˶���β��ȡ��������
		int y = snake_body.back().second;
		board[x][y] = false; // ����ʧ

		qiangcnt++; //ÿ�ν������ǽ++������ģʽ�У�
		old_cin = user_cin;
		int updata_speed = speed; //ģʽ��ͬ�ٶȲ�ͬ
		while (!have_food) { //����ʳ��
			while (!next_direction.empty()) next_direction.pop(); //ȷ���ǿ�ջ
			ran = rand() % (board_len * board_len); //����0-N*N�������
			food.first = ran / board_len + 1; //�������������ʽ
			food.second = ran % board_len + 1;
			if (auto_move) find_next(food);
			if (!board[food.first][food.second]) { //��ֹ��ʳ��������������
				have_food = true;
				board[food.first][food.second] = true;
			}
		}
		if (is_diff && (qiangcnt % 10 == 0))/*ÿˢ��10������һ��*/ { //����ģʽ������
			int difcnt = rand() % (board_len * board_len);
			board[difcnt / board_len + 1][difcnt % board_len + 1] = true;
		}
		if (_kbhit() && !auto_move) { //chrono���ú�����kbhit()�ж��Ƿ�ǰ������
			user_cin = _getch(); //�޻������룬���뼴����
			if (dis_stand.find(user_cin) != std::string::npos) { //��ֹ�������
				pos = dis_stand.find(user_cin); //�ҵ�ӳ��
			}
			if (user_cin == old_cin) updata_speed = 1; //˲�Ƽ��� 
		}
		else {
			if (auto_move) {
				if (!next_direction.empty()) {
					//cout << next_direction.top() << " ";
					user_cin = dis_stand[next_direction.top()], next_direction.pop();
				}
			}
			updata_speed = speed; //����ԭ�е��ٶ�
		}
		// cout << user_cin << "\n";
		if (dis_stand.find(user_cin) != string::npos) pos = dis_stand.find(user_cin); //�ı䷽��

		int nx = snake_body.front().first + xx[pos];
		int ny = snake_body.front().second + yy[pos]; //�����µ�ͷ����ʵ�ַ���ı仯���ƶ�
		pair<int, int>now = { nx, ny }; //��ǰ����

		if ((nx <= board_len && ny <= board_len && nx >= 1 && ny >= 1 && !board[nx][ny]) || (board[nx][ny] && food == now)) {
			snake_body.push_front({ nx, ny }); //����һ��ѹ����У������
			board[nx][ny] = true;
			if (!(board[nx][ny] && food == now)) snake_body.pop_back();
			else {
				have_food = false, board[x][y] = true; // ����ʧ; //�Ե�ʳ�β�����˳�
			}
			if (auto_move && snake_body.size() == 10 * 10) {
				cout << "\n��ʾ����\n\n";
				this_thread::sleep_for(chrono::milliseconds(1000));
				return;
			}
			if (snake_body.size() == 15 * 15) { // ͨ��
				cout << "��ţ�ƣ�ͨ���ˣ�";  return;
			}
		}
		else {
			if (!auto_move) board_print(),/*�����֪���Լ�������*/  cout << "\n\n��Ϸ����" << "\n";
			else cout << "\n\n����ϵͳBUG,�ȴ������޸�";
			//cout << "\n" << user_cin << "\n";
			//this_thread::sleep_for(chrono::milliseconds(1000));  system("cls");
			return; //ֱ���˳�ѭ�������˳�����
		}
		//cout<< "\n" << user_cin << "\n";
		board_print();	//��ӡ����
		this_thread::sleep_for(chrono::milliseconds(updata_speed));	 //˯�ߣ�����ҿ������̲����з�Ӧ�����в���
		system("cls"); //�������
		if (is_wujin && !(qiangcnt % 10)) { //�޾�ģʽ
			board[food.first][food.second] = false;
			have_food = false;
		}
		/*
		* BUG˵����1.�ڳԵ�ʳ�����Զ��ж�����
		//ע�⣬�����ϲ���Ҫ������δ��룬�˴���Ϊ�����������Զ�ģʽ��ʹ���ṩһ��Ҳ����е��޸���ʽ
		if (auto_move) { //���Գ�֮Ϊcheck����
			int x = snake_body.front().first;
			int y = snake_body.front().second;
			board[snake_body.front().first][snake_body.front().second] = false;
			int tmp_pos;
			if (!next_direction.empty()) tmp_pos = next_direction.top();
			else tmp_pos = pos;
			int tx = x + xx[tmp_pos];
			int ty = y + yy[tmp_pos];
			if (board[tx][ty] && food != pair<int, int>(tx, ty)) {
				cout << "jinru";
				while (!next_direction.empty()) {
					next_direction.pop();
				}
				for (int i = 0; i < 4; i++) {
					if (!board[x + xx[i]][y + yy[i]]) {
						snake_body.push_front({ x + xx[i], y + yy[i] });
						snake_body.pop_back();
						find_next(food);
						break;
					}
				}
			}
			board[snake_body.front().first][snake_body.front().second] = true;
		}
		//
		*/
	}
}
void deal_end() {
	while (!snake_body.empty()) {
		snake_body.pop_back();
	}
	while (!next_direction.empty()) next_direction.pop();
	for (int i = 0; i <= board_len; i++) {
		for (int j = 0; j <= board_len; j++) {
			board[i][j] = false;
		}
	}
	have_food = false;
	is_wujin = false;
	auto_move = false;
	is_diff = 0;
	speed = 100;
}
void print_low() {
	cout << "\n\n��ӭ�������ģʽ����ģʽ�ƶ��ٶȽ��������͵õ�����ķ����ɣ�\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_mid() {
	cout << "\n\n��ӭ��������ģʽ����ģʽ�ƶ��ٶ��еȣ����͵õ�����ķ����ɣ�\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_fast() {
	cout << "\n\n��ӭ�������ģʽ����ģʽ�ƶ��ٶȽϿ죬ף����ˣ�\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_diff() {
	cout << "\n\n��ӭ���뼫��ģʽ����ģʽ�£�ÿʮ�����������һ���ϰ���\n\n���ƶ��ٶ�Ϊ�У��ܿ��ϰ��ﲢŬ����ø��߷����ɣ�ף����ˣ�\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_wujin() {
	cout << "\n\n��ӭ���޾�ģʽ����ģʽ�£�ÿʮ��ʳ��ᡰ���ߡ�\n\n���ƶ��ٶ�Ϊ�У�Ŭ����ø��߷����ɣ�ף����ˣ�\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_auto() {
	cout << "\n\n��ӭ������ʾģʽ�����Զ��ƶ����Ե�ʳ��\n\n��������BUG������ײǽ�ȴ����ڴ����ܴ�����ɣ�\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void run_and_calc(int x) {
	auto _start = chrono::high_resolution_clock::now();
	do_game();
	auto _end = chrono::high_resolution_clock::now();
	int all_time = chrono::duration_cast<chrono::milliseconds>(_end - _start).count();
	double sec = all_time * 1.0 / 1000;
	int goal = snake_body.size() - 3;
	goal = max(0, goal);
	information[x].insert({ goal, sec });
	system("cls");
	// cout << "\n" << sec << " " << goal << "\n";
	cout << "\n\n�����ʱ��" << sec << "s" << "  " << "��ĵȷ֣�" << goal << "��\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));
	system("cls");
	cout << "\n\n���ڷ������˵�...";
	this_thread::sleep_for(chrono::milliseconds(1000));
	system("cls");
}
void print_paihang() {
	cout << "\n\n���а�ֻ�����֣��ֱ�Ϊ��ͨģʽ�����и��ٶȣ�������ģʽ���޾�ģʽ\n\n";
	cout << "\n              ���а�          \n\n\n";
	std::string s;
	s = "----------------------------------";
	len = s.size();
	std::cout << s << "\n\n\n";
	string s1 = "1.��ͨģʽ";
	int pre = calc(s1.size());
	int bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);
	s1 = "2.����ģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);
	s1 = "3.�޾�ģʽ";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);
	std::cout << s << "\n\n";
	cout << "\n��ѡ����Ҫ�鿴�����а�:\n\n";

}
void file_gan(string s, int x, bool run) {
	FILE* file = fopen(s.c_str(), "r");
	if (file == NULL) {
		cout << "\nδ�ҵ��ļ�\n\n";
	}
	else {
		int data;  float sec;
		while (fscanf(file, "%d %f", &data, &sec) == 2) {
			information[x].insert({ data, sec });
		}
		fclose(file);
		for (auto it : information[x]) {
			if (run) cout << "\n�÷֣�" << it.goal << "��  ��ʱ��" << it.sec << "��\n\n";
		}
		file = fopen(s.c_str(), "w");
		for (auto it : information[x]) {
			fprintf(file, "%d %f\n", it.goal, it.sec);
		}
		fclose(file);
	}
}
int main() {
	cout << setprecision(3);
	cout << "\n����������Ϸ����δ������ȫ״̬�������BUG�Լ���ƵĲ�������\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
	print_jinggao();
	this_thread::sleep_for(chrono::milliseconds(4000));  system("cls");
	while (1) {
		string file;
		deal_end();
		print_jiemian();
		initial_board();
		cin >> jud; //ģʽ��ѡ��
		system("cls");
		switch (jud) {
		case 0:
			print_jiesu();
			// break;
			return 0;
		case 1:
			speed = 200;
			print_low();
			run_and_calc(0);
			file_gan("normal.txt", 0, false);
			break;
		case 2:
			speed = 100;
			print_mid();
			run_and_calc(0);
			file_gan("normal.txt", 0, false);
			break;
		case 3:
			speed = 50;
			print_fast();
			run_and_calc(0);
			file_gan("normal.txt", 0, false);
			break;
		case 4:
			speed = 100;
			is_diff = 1;
			print_diff();
			run_and_calc(1);
			do_game();
			file_gan("jixian.txt", 1, false);
			break;
		case 5:
			speed = 100;
			is_wujin = true;
			print_wujin();
			run_and_calc(2);
			file_gan("wujin.txt", 2, false);
			break;

		case 6:
			speed = 100;
			auto_move = true;
			print_auto();
			run_and_calc(3);
			break;

		case 7:
			while (1) {
				system("cls");
				print_paihang();
				cout << "\n����0�ص�������\n\n";
				int mod_cin;
				cin >> mod_cin;
				if (!mod_cin) {
					cout << "\n���ڷ���������...\n\n";
					break;
				}
				mod_cin--;
				system("cls");
				if (mod_cin == 0) {
					file_gan("normal.txt", mod_cin, 1);
				}
				else if (mod_cin == 1) {
					file_gan("jixian.txt", mod_cin, 1);
				}
				else if (mod_cin == 2) {
					file_gan("wujin.txt", mod_cin, 1);
				}
				cout << "\n�������������س��������а�����0�ص�������\n\n";
				cin >> mod_cin;
				if (!mod_cin) {
					cout << "\n���ڷ���������...\n\n";
					break;
				}
			}
		}
		this_thread::sleep_for(chrono::milliseconds(1000));  system("cls");
	}
}
