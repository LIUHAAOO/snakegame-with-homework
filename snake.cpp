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
using namespace std; //使用命名空间std	
const int board_len = 20; //棋盘的长度
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
bool board[board_len + 10][board_len + 10]; //棋盘初始值都为false，用bool类型表示是否有物体/蛇身体
bool have_food; //是否棋盘上有食物 - 因为棋盘上必须要有一个食物
bool is_wujin = false;
bool auto_move = false;
int is_diff = 0; //极限模式的判定，初始为0
int speed = 100; //初始刷新率（睡眠多少毫秒）
int jud = 0;
stack<int> next_direction; //存方向 
string dis_stand = "adws"; //方向键的索引
vector<string> filename = { "normal.txt", "jixian.txt", "wujin.txt" };
set<inf_node> information[4];
int xx[] = { 0, 0, -1, 1 };
int yy[] = { -1, 1, 0, 0 }; // 配合移动的，只需要特定的索引，从dis_stand搜索到
deque<pair<int, int>> snake_body; //蛇身 -- 一个双端队列
pair<int, int> food;  //食物的位置(x, y)
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
				int len = dis_calc(now, food); //贪心最优解求 （启发式）实测比A*快（因为是曼哈顿路径） 
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
	while (!q.empty()) { //从后往前，只要有路，一定是来的路，所以一定能回去 
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
				next_direction.push(i); //得反转一下 
				vis[nx][ny] = false;
				q.push(it);
				break; //直接退出 
			}
		}
	}
	board[snake_body.front().first][snake_body.front().second] = true;
}
void board_print() { //打印棋盘
	for (int i = 0; i <= board_len + 1; i++) cout << "-";
	cout << "\n";
	for (int i = 1; i <= board_len; i++) {//双重循环遍历整个棋盘
		cout << "|";
		for (int j = 1; j <= board_len; j++) {
			pair<int, int> now = { i, j };
			if (board[i][j] && food == now) cout << "*"; //食物的坐标是当前i，j
			else if ((board[i][j])) cout << "■";
			else cout << " ";
		}
		cout << "|\n"; //棋盘边界的处理/美化
	}
	for (int i = 0; i <= board_len + 1; i++) cout << "-";//棋盘边界的处理/美化
	cout << "\n";
}
void print_jinggao() {
	cout << "\n警告：游戏前请详阅\n\n"
		<< "有极少数人在观看某些视觉影像时，\n\n"
		<< "可能会突然发生癫痫。这包括电子游戏\n\n"
		<< "中的闪光或图形。游玩时，他们可能遇\n\n"
		<< "到癫痫症状。即使无癫痫史，也有可能\n\n"
		<< "出现类似反应。若您或家人有癫痫史，\n\n"
		<< "请先咨询医生。\n\n"
		<< "若在游戏中遇到以下情况：眼睛疼、视\n\n"
		<< "觉异常、偏头痛、痉挛或意识障碍（例\n\n"
		<< "如昏迷），请立即停止并咨询医生。\n\n"
		<< "此外，若感到头痛、晕眩、恶心或不适，\n\n"
		<< "亦应停止游戏。症状持续，请及时就医。\n\n";
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
	cout << "\n游戏正在退出...";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
	cout << "\n游戏已退出，祝您生活愉快！";
	this_thread::sleep_for(chrono::milliseconds(500));

}
void print_jiemian() {
	//cout << YELLOW;
	cout << "\n          贪吃蛇游戏主界面          \n\n\n";
	std::string s;
	s = "----------------------------------";
	len = s.size();
	std::cout << s << "\n\n\n";

	string s1 = "1.简单模式";
	int pre = calc(s1.size());
	int bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "2.中等模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "3.困难模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "4.极限模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "5.无尽模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "6.演示模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "7.排行榜";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "8.游戏说明";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	s1 = "0.退出游戏";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);

	std::cout << s << "\n\n";
	cout << "\n请选择你需要使用的功能:\n\n";
}
void do_game() {
	int initial_cnt = 3; //初始小蛇的身体长度
	struct _timeb T;
    _ftime(&T);
    srand(T.millitm);
	int pos = 3; //小蛇当前移动方向，默认3，即x+xx[3](0) y+yy[3](1) 向右移动
	while (initial_cnt--) { //初始的身体数量
		int x = board_len / 2, y = board_len / 2 + initial_cnt; //初始的身体的坐标
		snake_body.push_back({ x, y }); //放到尾部，使y值最小的 在deque的最尾部（因为初始向右，所以头部的y因该最大）
		board[x][y] = true;//进队一个，就标记一个为true，以便扫描棋盘时打印
	}
	char user_cin = 'd', old_cin = 'd'; int ran;
	int qiangcnt = 1; //极限模式中墙的操作
	while (1) {
		int x = snake_body.back().first; //把双端队列尾部取出来备用
		int y = snake_body.back().second;
		board[x][y] = false; // 先消失

		qiangcnt++; //每次进入就让墙++（极限模式中）
		old_cin = user_cin;
		int updata_speed = speed; //模式不同速度不同
		while (!have_food) { //生成食物
			while (!next_direction.empty()) next_direction.pop(); //确保是空栈
			ran = rand() % (board_len * board_len); //生产0-N*N的随机数
			food.first = ran / board_len + 1; //把数变成坐标形式
			food.second = ran % board_len + 1;
			if (auto_move) find_next(food);
			if (!board[food.first][food.second]) { //防止把食物生成在蛇身里
				have_food = true;
				board[food.first][food.second] = true;
			}
		}
		if (is_diff && (qiangcnt % 10 == 0))/*每刷新10次生成一个*/ { //极限模式的生成
			int difcnt = rand() % (board_len * board_len);
			board[difcnt / board_len + 1][difcnt % board_len + 1] = true;
		}
		if (_kbhit() && !auto_move) { //chrono内置函数，kbhit()判断是否当前有输入
			user_cin = _getch(); //无缓冲数入，输入即读入
			if (dis_stand.find(user_cin) != std::string::npos) { //防止索引溢出
				pos = dis_stand.find(user_cin); //找到映射
			}
			if (user_cin == old_cin) updata_speed = 1; //瞬移加速 
		}
		else {
			if (auto_move) {
				if (!next_direction.empty()) {
					//cout << next_direction.top() << " ";
					user_cin = dis_stand[next_direction.top()], next_direction.pop();
				}
			}
			updata_speed = speed; //保持原有的速度
		}
		// cout << user_cin << "\n";
		if (dis_stand.find(user_cin) != string::npos) pos = dis_stand.find(user_cin); //改变方向

		int nx = snake_body.front().first + xx[pos];
		int ny = snake_body.front().second + yy[pos]; //利用新的头坐标实现方向的变化和移动
		pair<int, int>now = { nx, ny }; //当前坐标

		if ((nx <= board_len && ny <= board_len && nx >= 1 && ny >= 1 && !board[nx][ny]) || (board[nx][ny] && food == now)) {
			snake_body.push_front({ nx, ny }); //将下一步压入队列，并标记
			board[nx][ny] = true;
			if (!(board[nx][ny] && food == now)) snake_body.pop_back();
			else {
				have_food = false, board[x][y] = true; // 先消失; //吃到食物，尾部不退出
			}
			if (auto_move && snake_body.size() == 10 * 10) {
				cout << "\n演示结束\n\n";
				this_thread::sleep_for(chrono::milliseconds(1000));
				return;
			}
			if (snake_body.size() == 15 * 15) { // 通关
				cout << "真牛逼，通关了！";  return;
			}
		}
		else {
			if (!auto_move) board_print(),/*让玩家知道自己的死法*/  cout << "\n\n游戏结束" << "\n";
			else cout << "\n\n出现系统BUG,等待后续修复";
			//cout << "\n" << user_cin << "\n";
			//this_thread::sleep_for(chrono::milliseconds(1000));  system("cls");
			return; //直接退出循环，即退出程序
		}
		//cout<< "\n" << user_cin << "\n";
		board_print();	//打印棋盘
		this_thread::sleep_for(chrono::milliseconds(updata_speed));	 //睡眠，让玩家看到棋盘并进行反应，进行操作
		system("cls"); //清除界面
		if (is_wujin && !(qiangcnt % 10)) { //无尽模式
			board[food.first][food.second] = false;
			have_food = false;
		}
		/*
		* BUG说明：1.在吃到食物后会自动判定死亡
		//注意，理论上不需要后面这段代码，此代码为补丁，仅在自动模式中使用提供一种也许可行的修复方式
		if (auto_move) { //可以称之为check函数
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
	cout << "\n\n欢迎进入低速模式，该模式移动速度较慢，加油得到更多的分数吧！\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_mid() {
	cout << "\n\n欢迎进入中速模式，该模式移动速度中等，加油得到更多的分数吧！\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_fast() {
	cout << "\n\n欢迎进入高速模式，该模式移动速度较快，祝你好运！\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_diff() {
	cout << "\n\n欢迎进入极限模式，该模式下，每十步会随机生成一个障碍物\n\n，移动速度为中，避开障碍物并努力获得更高分数吧！祝你好运！\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_wujin() {
	cout << "\n\n欢迎进无尽模式，该模式下，每十步食物会“逃走”\n\n，移动速度为中，努力获得更高分数吧！祝你好运！\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
}
void print_auto() {
	cout << "\n\n欢迎进入演示模式，会自动移动并吃到食物\n\n，但存在BUG，出现撞墙等错误，期待它能聪明点吧！\n\n";
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
	cout << "\n\n你的用时：" << sec << "s" << "  " << "你的等分：" << goal << "分\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));
	system("cls");
	cout << "\n\n正在返回主菜单...";
	this_thread::sleep_for(chrono::milliseconds(1000));
	system("cls");
}
void print_paihang() {
	cout << "\n\n排行榜只有三种，分别为普通模式（低中高速度），极限模式和无尽模式\n\n";
	cout << "\n              排行榜          \n\n\n";
	std::string s;
	s = "----------------------------------";
	len = s.size();
	std::cout << s << "\n\n\n";
	string s1 = "1.普通模式";
	int pre = calc(s1.size());
	int bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);
	s1 = "2.极限模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);
	s1 = "3.无尽模式";
	pre = calc(s1.size());
	bac = len - 6 - s1.size() - pre;
	pat(pre, bac, s1);
	std::cout << s << "\n\n";
	cout << "\n请选择需要查看的排行榜:\n\n";

}
void file_gan(string s, int x, bool run) {
	FILE* file = fopen(s.c_str(), "r");
	if (file == NULL) {
		cout << "\n未找到文件\n\n";
	}
	else {
		int data;  float sec;
		while (fscanf(file, "%d %f", &data, &sec) == 2) {
			information[x].insert({ data, sec });
		}
		fclose(file);
		for (auto it : information[x]) {
			if (run) cout << "\n得分：" << it.goal << "分  用时：" << it.sec << "秒\n\n";
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
	cout << "\n声明：该游戏处于未开发完全状态存在许多BUG以及设计的不合理性\n\n";
	this_thread::sleep_for(chrono::milliseconds(2000));  system("cls");
	print_jinggao();
	this_thread::sleep_for(chrono::milliseconds(4000));  system("cls");
	while (1) {
		string file;
		deal_end();
		print_jiemian();
		initial_board();
		cin >> jud; //模式的选择
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
				cout << "\n输入0回到主界面\n\n";
				int mod_cin;
				cin >> mod_cin;
				if (!mod_cin) {
					cout << "\n正在返回主界面...\n\n";
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
				cout << "\n输入任意正数回车返回排行榜，输入0回到主界面\n\n";
				cin >> mod_cin;
				if (!mod_cin) {
					cout << "\n正在返回主界面...\n\n";
					break;
				}
			}
		}
		this_thread::sleep_for(chrono::milliseconds(1000));  system("cls");
	}
}
